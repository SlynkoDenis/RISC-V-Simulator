#ifndef RISC_V_SIMULATOR_PIPELINE_H
#define RISC_V_SIMULATOR_PIPELINE_H

#include <array>
#include <vector>
#include "control_unit.h"
#include "hazard_unit.h"
#include "../modules/alu.h"
#include "../modules/cmp.h"
#include "../modules/common.h"
#include "../modules/data_mem_unit.h"
#include "../modules/instr_mem_unit.h"
#include "../modules/register.h"
#include "../modules/regfile.h"


namespace pipeline {

    using modules::byte_;
    using modules::word_;

    struct DecodeState {
        DecodeState(word_ value) : pc_r(static_cast<bool>(value)),
                                   pc_de(value),
                                   instr(value) {};

        bool pc_r;
        word_ pc_de;
        word_ instr;
    };

    inline std::ostream& operator<<(std::ostream& os, const DecodeState& decode_state) {
        os << decode_state.pc_r << ", " << decode_state.pc_de << ", " << decode_state.instr;
        return os;
    }

    struct ExecuteState {
        ExecuteState(word_ value) : v_de(static_cast<bool>(value)),
                                    alu_op(static_cast<modules::ALUControl>(value)),
                                    alu_src2(static_cast<bool>(value)),
                                    mem_to_reg(static_cast<bool>(value)),
                                    cmp_control(static_cast<modules::CmpControl>(value)),
                                    brn_cond(static_cast<bool>(value)),
                                    jmp_cond(static_cast<bool>(value)),
                                    data1(value),
                                    data2(value),
                                    se(value),
                                    pc_de(value),
                                    instr(value) {};

        bool v_de;
        modules::ALUControl alu_op;
        bool alu_src2;
        bool mem_to_reg;
        modules::CmpControl cmp_control;
        bool brn_cond;
        bool jmp_cond;
        word_ data1;
        word_ data2;
        word_ se;
        word_ pc_de;
        word_ instr;
    };

    inline std::ostream& operator<<(std::ostream& os, const ExecuteState& execute_state) {
        os << execute_state.v_de << ", " << execute_state.alu_op << ", " << execute_state.alu_src2;
        os << ", " << execute_state.mem_to_reg << ", " << execute_state.cmp_control << ", ";
        os << execute_state.brn_cond << ", " << execute_state.jmp_cond << ", " << execute_state.data1;
        os << ", " << execute_state.data2 << ", " << execute_state.se << ", " << execute_state.pc_de;
        os << ", " << execute_state.instr;
        return os;
    }

    struct MemoryState {
        MemoryState(word_ value) : mem_we(static_cast<bool>(value)),
                                   mem_to_reg(static_cast<bool>(value)),
                                   wb_we(static_cast<bool>(value)),
                                   write_data(value),
                                   alu_res(value),
                                   wb_a(value) {};

        bool mem_we;
        bool mem_to_reg;
        bool wb_we;
        word_ write_data;
        word_ alu_res;
        word_ wb_a;
    };

    inline std::ostream& operator<<(std::ostream& os, const MemoryState& memory_state) {
        os << memory_state.mem_we << ", " << memory_state.mem_to_reg << ", " << memory_state.wb_we;
        os << ", " << memory_state.write_data << ", " << memory_state.alu_res << ", " << memory_state.wb_a;
        return os;
    }

    struct WriteBackState {
        WriteBackState(word_ value) : wb_we(static_cast<bool>(value)),
                                      wb_d(value),
                                      wb_a(value) {};

        bool wb_we;
        word_ wb_d;
        byte_ wb_a;
    };

    inline std::ostream& operator<<(std::ostream& os, const WriteBackState& wb_state) {
        os << wb_state.wb_we << ", " << wb_state.wb_d << ", " << wb_state.wb_a;
        return os;
    }

    class Pipeline {
    public:
        explicit Pipeline(const std::vector<word_>& instructions) : instr_mem_unit(instructions) {};

        Pipeline(const std::vector<word_>& instructions,
                 const std::vector<word_>& data) : instr_mem_unit(instructions), data_mem_unit(data) {};

        virtual ~Pipeline() noexcept = default;

        virtual void setProgramCounter(word_ pc);

        virtual void tick();

    private:
        void tickRegisters() {
            decode_register.tick();
            execute_register.tick();
            memory_register.tick();
            write_back_register.tick();
        }

        modules::InstrMemUnit instr_mem_unit;
        modules::DataMemUnit data_mem_unit;
        modules::RegFile reg_file;
        modules::ALU alu;
        ControlUnit control_unit;
        HazardUnit hazard_unit;
        modules::Register<word_> program_counter;

        modules::Register<DecodeState> decode_register;
        modules::Register<ExecuteState> execute_register;
        modules::Register<MemoryState> memory_register;
        modules::Register<WriteBackState> write_back_register;
    };
}

#endif //RISC_V_SIMULATOR_PIPELINE_H
