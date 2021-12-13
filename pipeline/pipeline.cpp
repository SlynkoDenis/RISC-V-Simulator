#include "pipeline.h"
#include "instruction.h"
#include "../modules/gates.h"


namespace pipeline {

    void Pipeline::setProgramCounter(word_ pc) {
        program_counter.next = pc;
    }

    void Pipeline::tick() {
        // write back
        auto wb_reg_cur = write_back_register.getCurrent();
        DEBUG_LOG(write_back_register);

        // memory
        auto mem_reg_cur = memory_register.getCurrent();
        auto data_rd = data_mem_unit.getData();
        data_mem_unit.setNewSignals(mem_reg_cur.mem_we,
                                    mem_reg_cur.write_data,
                                    mem_reg_cur.alu_res);
        data_mem_unit.tick();
        // update next register
        write_back_register.next.wb_we = mem_reg_cur.wb_we;
        write_back_register.next.wb_d = modules::Multiplexer2<word_>{}(mem_reg_cur.mem_to_reg,
                                                                       data_rd,
                                                                       mem_reg_cur.alu_res);
        write_back_register.next.wb_a = mem_reg_cur.wb_a;
        auto bp_mem = mem_reg_cur.alu_res;
        DEBUG_LOG(memory_register);

        // execute
        auto exec_reg_cur = execute_register.getCurrent();
        alu.src_a = modules::Multiplexer3<word_>{}(hazard_unit.getRs1(),
                                                   exec_reg_cur.data1,
                                                   bp_mem,
                                                   wb_reg_cur.wb_d);
        auto rs2v = modules::Multiplexer3<word_>{}(hazard_unit.getRs2(),
                                                   exec_reg_cur.data1,
                                                   bp_mem,
                                                   wb_reg_cur.wb_d);
        auto pc_disp = utils::ImmediateExtensionBlock{}(exec_reg_cur.instr);
        alu.src_b = modules::Multiplexer2<word_>{}(exec_reg_cur.alu_src2,
                                                   rs2v,
                                                   pc_disp);
        bool cmp_res = modules::Cmp<word_>{}(exec_reg_cur.cmp_control,
                                             alu.src_a,
                                             rs2v);
        bool pc_r = modules::And<bool>{}(cmp_res,
                                         modules::Or<bool>{}(exec_reg_cur.brn_cond, exec_reg_cur.jmp_cond));
        // update next register
        alu.control_signal = exec_reg_cur.alu_op;
        memory_register.next.alu_res = alu.executeOperation();
        memory_register.next.wb_a = utils::getRd(exec_reg_cur.instr);
        // TODO: is it right? (note the errata 2)
        memory_register.next.write_data = rs2v;
        memory_register.next.mem_to_reg = exec_reg_cur.mem_to_reg;
        decode_register.next.pc_r = pc_r;
        DEBUG_LOG(execute_register);

        // decode
        auto decode_stage_instr = decode_register.getCurrent();
        reg_file.setNewSignals(utils::getRs1(decode_stage_instr.instr),
                               utils::getRs2(decode_stage_instr.instr),
                               wb_reg_cur.wb_a,
                          wb_reg_cur.wb_d,
                               wb_reg_cur.wb_we);
        reg_file.tick();

        control_unit.opcode = utils::getOpcode(decode_stage_instr.instr);
        control_unit.funct3 = utils::getFunct3(decode_stage_instr.instr);
        control_unit.funct7 = utils::getFunct7(decode_stage_instr.instr);
        control_unit.tick();
        // update next register
        execute_register.next.data1 = reg_file.getReadData1();
        execute_register.next.data2 = reg_file.getReadData2();
        execute_register.next.pc_de = decode_stage_instr.pc_de;
        execute_register.next.instr = decode_stage_instr.instr;
        execute_register.next.v_de = modules::Or<bool>{}(pc_r, decode_stage_instr.pc_r);
        DEBUG_LOG(decode_register);

        // fetch
        auto summand1 = modules::Multiplexer2<word_>{}(pc_r,
                                                       program_counter.getCurrent(),
                                                       exec_reg_cur.pc_de);
        auto summand2 = modules::Multiplexer2<word_>{}(pc_r, 4, pc_disp);
        program_counter.next = modules::Add<word_>{}(summand1, summand2);
        // update next register
        decode_register.next.instr = instr_mem_unit.getData();
        decode_register.next.pc_de = program_counter.getCurrent();
        instr_mem_unit.address = program_counter.getCurrent();
        instr_mem_unit.tick();
        DEBUG_LOG(program_counter);

        tickRegisters();
        program_counter.tick();

        DEBUG_LOG(instr_mem_unit);
        DEBUG_LOG(data_mem_unit);
        DEBUG_LOG(reg_file);
        DEBUG_LOG(control_unit);
        DEBUG_LOG(hazard_unit);

        std::cout << std::endl;
    }
}

/*
 * Errata2: data для записи в память должна выбираться после хазарда и со второго регистра
 * Errata3: для pc_r помимо BRN_COND должен проверяться через or аналогичный флажок для JMP
 * Errata4: для jr нужно до wb протолкнуть pc+4
 * Errata5: для ret нужно поменять fetch, понятно как
 */
