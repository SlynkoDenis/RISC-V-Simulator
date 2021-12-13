#ifndef RISC_V_SIMULATOR_ALU_H
#define RISC_V_SIMULATOR_ALU_H

#include <exception>
#include <iostream>
#include "common.h"


namespace modules {

    enum class ALUControl : byte_ {
        NOP,
        ADD,
        SUB,
        SLL,
        SLT,
        SLTU,
        XOR,
        SRL,
        SRA,
        OR,
        AND
    };

    inline std::ostream& operator<<(std::ostream& os, ALUControl alu_control) {
        os << std::hex << static_cast<char>(alu_control);
        return os;
    }

    class ALU {
    public:
        virtual ~ALU() noexcept = default;

        virtual word_ executeOperation() {
            switch (control_signal) {
                case ALUControl::ADD:
                    return src_a + src_b;
                case ALUControl::SUB:
                    return src_a - src_b;
                case ALUControl::AND:
                    return src_a & src_b;
                case ALUControl::OR:
                    return src_a | src_b;
                case ALUControl::XOR:
                    return src_a ^ src_b;
                case ALUControl::SLL:
                    return src_a << src_b;
                case ALUControl::SRL:
                    return src_a >> src_b;
                case ALUControl::SRA:
                    return (int32_t)src_a >> src_b;
                case ALUControl::SLT:
                    return ((int32_t)src_a < (int32_t)src_b);
                case ALUControl::SLTU:
                    return (src_a < src_b);
                default:
                    throw std::logic_error("unknown control signal");
            }
        }

        [[nodiscard]] word_ getResult() const {
            return result;
        }

        ALUControl control_signal = ALUControl::NOP;
        word_ src_a = 0;
        word_ src_b = 0;

    private:
        word_ result = 0;
    };
}

#endif //RISC_V_SIMULATOR_ALU_H
