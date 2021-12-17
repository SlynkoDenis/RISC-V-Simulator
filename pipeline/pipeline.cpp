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

        tickRegisters();
        program_counter.tick();

        debug();
    }

    void Pipeline::debug() {
        DEBUG_LOG(program_counter);
        DEBUG_LOG(decode_register);
        DEBUG_LOG(execute_register);
        DEBUG_LOG(memory_register);
        DEBUG_LOG(write_back_register);

        DEBUG_LOG(instr_mem_unit);
        DEBUG_LOG(data_mem_unit);
        DEBUG_LOG(reg_file);
        DEBUG_LOG(control_unit);
        DEBUG_LOG(hazard_unit);

#ifdef DEBUG
        std::cout << "\n\n";
#endif
    }

    void Pipeline::doFetch() {
        if (program_counter.getCurrent() >= instr_mem_unit.getEndOfInstructionSection()) {
            throw std::logic_error("out_of_instr_section");
        }
        instr_mem_unit.address = program_counter.getCurrent();
        auto summand1 = modules::Multiplexer2<word_>{}(pc_r,
                                                       program_counter.getCurrent(),
                                                       execute_register.getCurrent().pc_de);
        auto summand2 = modules::Multiplexer2<word_>{}(pc_r, 4, pc_disp);
        program_counter.next = modules::Add<word_>{}(summand1, summand2);
        // update next register
        decode_register.next.instr = instr_mem_unit.getData();
        decode_register.next.pc_de = program_counter.getCurrent();
        instr_mem_unit.address = program_counter.getCurrent();
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
        auto src_a = modules::Multiplexer3<word_>{}(hazard_unit.getRs1(),
                                                   exec_reg_cur.data1,
                                                   bp_mem,
                                                   write_back_register.getCurrent().wb_d);
        auto rs2v = modules::Multiplexer3<word_>{}(hazard_unit.getRs2(),
                                                   exec_reg_cur.data2,
                                                   bp_mem,
                                                   write_back_register.getCurrent().wb_d);
        pc_disp = utils::ImmediateExtensionBlock{}(exec_reg_cur.instr);
        auto src_b = modules::Multiplexer2<word_>{}(exec_reg_cur.alu_src2,
                                                    rs2v,
                                                    pc_disp);
        bool cmp_res = modules::Cmp<word_>{}(exec_reg_cur.cmp_control,
                                             src_a,
                                             rs2v);
        pc_r = modules::And<bool>{}(cmp_res,
                                modules::Or<bool>{}(exec_reg_cur.brn_cond, exec_reg_cur.jmp_cond));
        // update next registers
        memory_register.next.mem_we = exec_reg_cur.mem_we && (!exec_reg_cur.v_de);
        memory_register.next.mem_to_reg = exec_reg_cur.mem_to_reg;
        memory_register.next.wb_we = exec_reg_cur.wb_we && (!exec_reg_cur.v_de);
        // TODO: is it right? (note the errata 2)
        memory_register.next.write_data = rs2v;
        memory_register.next.alu_res = modules::ALU{}(exec_reg_cur.alu_op, src_a, src_b);
        memory_register.next.wb_a = utils::getRd(exec_reg_cur.instr);

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

    void Pipeline::tickRegisters() {
        decode_register.tick();
        execute_register.tick();
        memory_register.tick();
        write_back_register.tick();
    }
}

/*
 * Errata2: data для записи в память должна выбираться после хазарда и со второго регистра
 * Errata4: для jr нужно до wb протолкнуть pc+4
 * Errata5: для ret нужно поменять fetch, понятно как
 */
