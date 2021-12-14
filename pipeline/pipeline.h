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
        DecodeState(word_ value) : v_de(static_cast<bool>(value)),
                                   pc_de(value),
                                   instr(value) {};

        bool v_de;
        word_ pc_de;
        word_ instr;
    };

    inline std::ostream& operator<<(std::ostream& os, const DecodeState& decode_state) {
        os << std::hex << decode_state.v_de << ", " << decode_state.pc_de << ", " << decode_state.instr;
        return os;
    }

    struct ExecuteState {
        ExecuteState(word_ value) : alu_op(static_cast<modules::ALUControl>(value)),
                                    alu_src2(static_cast<bool>(value)),
                                    mem_to_reg(static_cast<bool>(value)),
                                    wb_we(static_cast<bool>(value)),
                                    mem_we(static_cast<bool>(value)),
                                    cmp_control(static_cast<modules::CmpControl>(value)),
                                    brn_cond(static_cast<bool>(value)),
                                    jmp_cond(static_cast<bool>(value)),
                                    v_de(static_cast<bool>(value)),
                                    data1(value),
                                    data2(value),
                                    pc_de(value),
                                    instr(value) {};

        modules::ALUControl alu_op;
        bool alu_src2;
        bool mem_to_reg;
        bool wb_we;
        bool mem_we;
        modules::CmpControl cmp_control;
        bool brn_cond;
        bool jmp_cond;
        bool v_de;
        word_ data1;
        word_ data2;
        word_ pc_de;
        word_ instr;
    };

    inline std::ostream& operator<<(std::ostream& os, const ExecuteState& execute_state) {
        os << std::hex << execute_state.v_de << ", " << execute_state.alu_op << ", " << execute_state.alu_src2;
        os << ", " << execute_state.mem_to_reg << ", " << execute_state.wb_we << ", " << execute_state.mem_we;
        os << ", " << execute_state.cmp_control << ", ";
        os << execute_state.brn_cond << ", " << execute_state.jmp_cond << ", " << execute_state.data1;
        os << ", " << execute_state.data2 << ", " << execute_state.pc_de;
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
        os << std::hex << memory_state.mem_we << ", " << memory_state.mem_to_reg << ", " << memory_state.wb_we;
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
        os << std::hex << wb_state.wb_we << ", " << wb_state.wb_d << ", " << static_cast<word_>(wb_state.wb_a);
        return os;
    }

    class Pipeline {
    public:
        explicit Pipeline(const std::vector<word_>& instructions,
                          word_ start_instr_address) : instr_mem_unit(instructions, start_instr_address) {
            std::cout << std::hex;
        };

        Pipeline(const std::vector<word_>& instructions,
                 word_ start_instr_address,
                 const std::vector<word_>& data) : instr_mem_unit(instructions, start_instr_address),
                                                   data_mem_unit(data) {};

        virtual ~Pipeline() noexcept = default;

        virtual void setProgramCounter(word_ pc);

        virtual void tick();

        virtual void debug();

    private:
        virtual void doFetch();

        virtual void doDecode();

        virtual void doExecute();

        virtual void doMemory();

        virtual void doWriteBack();

        void tickRegisters();

        word_ bp_mem;
        word_ pc_disp;
        bool pc_r;

        modules::InstrMemUnit instr_mem_unit;
        modules::DataMemUnit data_mem_unit;
        modules::RegFile reg_file;
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
