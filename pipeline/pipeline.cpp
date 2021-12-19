#include "pipeline.h"
#include "instruction.h"
#include "../modules/gates.h"


namespace pipeline {

    void Pipeline::setProgramCounter(word_ pc) {
        program_counter.next = pc;
        program_counter.tick();
    }

    void Pipeline::tick() {
        doWriteBack();
        doMemory();
        doExecute();
        doDecode();
        doFetch();

        if (!program_counter.enable_flag) {
            memory_register.clear();
        }
        tickRegisters();
        program_counter.tick();
        hazardUnitTick();

        debug();
    }

#ifdef DEBUG
    void Pipeline::debug() {
        DEBUG_LOG(program_counter);
        DEBUG_LOG(decode_register);
        DEBUG_LOG(execute_register);
        DEBUG_LOG(memory_register);
        DEBUG_LOG(write_back_register);

        std::cout << "pc_redirect == " << pc_r << std::endl;
        std::cout << "hu_rs1 == " << hu_rs1 << std::endl;
        std::cout << "hu_rs2 == " << hu_rs2 << std::endl;

        DEBUG_LOG(instr_mem_unit);
        DEBUG_LOG(data_mem_unit);
        DEBUG_LOG(reg_file);
        DEBUG_LOG(control_unit);

        printRegisters();

        std::cout << "\n\n";
    }
#endif

    void Pipeline::doFetch() {
        if (program_counter.getCurrent() >= instr_mem_unit.getEndOfInstructionSection() &&
            !(execute_register.next.jmp_cond || execute_register.next.brn_cond)) {
            throw std::logic_error("out_of_instr_section");
        }
        instr_mem_unit.address = program_counter.getCurrent();
        auto summand1 = modules::Multiplexer2<word_>{}(pc_r,
                                                       program_counter.getCurrent(),
                                                       execute_register.getCurrent().pc_de);
        auto summand2 = modules::Multiplexer2<word_>{}(pc_r, 4, pc_disp);
        program_counter.next = modules::Add<word_>{}(summand1, summand2);
        if (program_counter.next % 4 != 0) {
            throw modules::AlignmentException("pc is not aligned: pc_next == " +\
                                              std::to_string(program_counter.next));
        }
        // update next register
        decode_register.next.instr = instr_mem_unit.getData();
        decode_register.next.pc_de = program_counter.getCurrent();
        // if there was jump, we have pc = pc + 4, which must be written into rd of instruction
        if (memory_register.getCurrent().jmp_cond) {
            write_back_register.next.wb_d = program_counter.next;
        }
    }

    void Pipeline::doDecode() {
        auto decode_stage_instr = decode_register.getCurrent();
        reg_file.setNewSignals(utils::getRs1(decode_stage_instr.instr),
                               utils::getRs2(decode_stage_instr.instr),
                               write_back_register.getCurrent().wb_a,
                               write_back_register.getCurrent().wb_we,
                               write_back_register.getCurrent().wb_d);
        reg_file.tick();

        control_unit.opcode = utils::getOpcode(decode_stage_instr.instr);
        control_unit.funct3 = utils::getFunct3(decode_stage_instr.instr);
        control_unit.funct7 = utils::getFunct7(decode_stage_instr.instr);
        control_unit.tick();
        // update next register
        auto tmp = control_unit.getControlSignals();
        execute_register.next.alu_op = std::get<7>(tmp);
        execute_register.next.alu_src2 = std::get<6>(tmp);
        execute_register.next.wb_we = std::get<0>(tmp);
        execute_register.next.mem_we = std::get<1>(tmp);
        execute_register.next.mem_to_reg = std::get<2>(tmp);
        execute_register.next.brn_cond = std::get<3>(tmp);
        execute_register.next.jmp_cond = std::get<4>(tmp);
        execute_register.next.cmp_control = std::get<5>(tmp);

        execute_register.next.data1 = reg_file.getReadData1();
        execute_register.next.data2 = reg_file.getReadData2();
        execute_register.next.pc_de = decode_stage_instr.pc_de;
        execute_register.next.instr = decode_stage_instr.instr;
        execute_register.next.v_de = modules::Or<bool>{}(pc_r, decode_stage_instr.v_de);
    }

    void Pipeline::doExecute() {
        auto exec_reg_cur = execute_register.getCurrent();
        auto src_a = modules::Multiplexer3<word_>{}(static_cast<byte_>(hu_rs1),
                                                    exec_reg_cur.data1,
                                                    bp_mem,
                                                    write_back_register.getCurrent().wb_d);
        auto rs2v = modules::Multiplexer3<word_>{}(static_cast<byte_>(hu_rs2),
                                                   exec_reg_cur.data2,
                                                   bp_mem,
                                                   write_back_register.getCurrent().wb_d);
        pc_disp = utils::ImmediateExtensionBlock{}(exec_reg_cur.instr);
#ifdef DEBUG
        std::cout << "======================== imm == " << pc_disp << std::endl;
#endif
        auto src_b = modules::Multiplexer2<word_>{}(exec_reg_cur.alu_src2,
                                                    rs2v,
                                                    pc_disp);
        bool cmp_res = modules::Cmp<word_>{}(exec_reg_cur.cmp_control,
                                             src_a,
                                             rs2v);
        pc_r = modules::Or<bool>{}(exec_reg_cur.jmp_cond,
                                   modules::And<bool>{}(cmp_res, exec_reg_cur.brn_cond));
        // update next registers
        memory_register.next.mem_we = exec_reg_cur.mem_we && (!exec_reg_cur.v_de);
        memory_register.next.mem_to_reg = exec_reg_cur.mem_to_reg;
        memory_register.next.wb_we = exec_reg_cur.wb_we && (!exec_reg_cur.v_de);
        memory_register.next.jmp_cond = exec_reg_cur.jmp_cond;
        memory_register.next.write_data = rs2v;
        memory_register.next.alu_res = modules::ALU{}(exec_reg_cur.alu_op, src_a, src_b);
#ifdef DEBUG
        std::cout << "======================== ALU(" << exec_reg_cur.alu_op;
        std::cout << ", " << src_a << ", " << src_b << ") == " << memory_register.next.alu_res << std::endl;
#endif
        memory_register.next.wb_a = utils::getRd(exec_reg_cur.instr);
        // if the instruction is jalr, then pc_disp must be changed
        if (utils::getOpcode(exec_reg_cur.instr) == 0b1100111) {
            pc_disp = memory_register.next.alu_res;
        }
        decode_register.next.v_de = pc_r;
    }

    void Pipeline::doMemory() {
        auto mem_reg_cur = memory_register.getCurrent();
        if (!mem_reg_cur.mem_to_reg || mem_reg_cur.mem_we) {
            data_mem_unit.write_enable = mem_reg_cur.mem_we;
            data_mem_unit.address = mem_reg_cur.alu_res;
            data_mem_unit.write_data = mem_reg_cur.write_data;
            data_mem_unit.tick();
        }
        // update next register
        write_back_register.next.wb_we = mem_reg_cur.wb_we;
        write_back_register.next.wb_d = modules::Multiplexer2<word_>{}(mem_reg_cur.mem_to_reg,
                                                                       data_mem_unit.getData(),
                                                                       mem_reg_cur.alu_res);
        write_back_register.next.wb_a = mem_reg_cur.wb_a;
        bp_mem = mem_reg_cur.alu_res;
    }

    void Pipeline::doWriteBack() {}

    void Pipeline::haltPipeline() {
        program_counter.enable_flag = false;
        decode_register.enable_flag = false;
        execute_register.enable_flag = false;
    }

    void Pipeline::restartPipeline() {
        program_counter.enable_flag = true;
        decode_register.enable_flag = true;
        execute_register.enable_flag = true;
    }

    void Pipeline::hazardUnitTick() {
        if (pc_r) {
            decode_register.clear();
            execute_register.clear();
            return;
        }

        auto execute_stage_instr = execute_register.getCurrent().instr;
        auto rs1 = utils::getRs1(execute_stage_instr);
        auto rs2 = utils::getRs2(execute_stage_instr);
        auto mem_wb_a = memory_register.getCurrent().wb_a;

        bool was_halted = false;
        if (rs1 != 0) {
            if (rs1 == mem_wb_a) {
                hu_rs1 = BypassOptionsEncoding::MEM;
                if (!memory_register.getCurrent().mem_to_reg) {
                    haltPipeline();
                    was_halted = true;
                }
            } else if (rs1 == write_back_register.getCurrent().wb_a) {
                hu_rs1 = BypassOptionsEncoding::WB;
            } else {
                hu_rs1 = BypassOptionsEncoding::REG;
            }
        } else {
            hu_rs1 = BypassOptionsEncoding::REG;
        }

        if (rs2 != 0) {
            if (rs2 == mem_wb_a) {
                hu_rs2 = BypassOptionsEncoding::MEM;
                if (!memory_register.getCurrent().mem_to_reg) {
                    haltPipeline();
                    was_halted = true;
                }
            } else if (rs2 == write_back_register.getCurrent().wb_a) {
                hu_rs2 = BypassOptionsEncoding::WB;
            } else {
                hu_rs2 = BypassOptionsEncoding::REG;
            }
        } else {
            hu_rs2 = BypassOptionsEncoding::REG;
        }
        if (!was_halted) {
            restartPipeline();
        }
    }

    void Pipeline::tickRegisters() {
        decode_register.tick();
        execute_register.tick();
        memory_register.tick();
        write_back_register.tick();
    }
}

/*
 * Errata5: для ret нужно поменять fetch, понятно как
 */
