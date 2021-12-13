#ifndef RISC_V_SIMULATOR_CONTROL_UNIT_H
#define RISC_V_SIMULATOR_CONTROL_UNIT_H

#include <tuple>
#include "instruction.h"
#include "../modules/alu.h"
#include "../modules/common.h"


namespace pipeline {

    using modules::ALUControl;

    class ControlUnit {
    public:
        virtual ~ControlUnit() noexcept = default;

        [[nodiscard]] auto getControlSignals() {
            return std::make_tuple(mem_to_reg, brn_cond, jmp_cond, cmp_cond, alu_src2, alu_op);
        }

        virtual void tick() {
            if (opcode == 0b0010011) {
                alu_src2 = true;
            } else {
                alu_src2 = false;
            }

            if (opcode == 0b0010011 || opcode == 0b110011) {
                switch (funct3) {
                    case 0b000:
                        if (funct7 == 0) {
                            alu_op = ALUControl::ADD;
                        } else if (funct7 == 0b0100000) {
                            alu_op = ALUControl::SUB;
                        } else {
                            throw std::logic_error("invalid funct7 for ADD/SUB with funct3 == 0: " +\
                                std::to_string(funct7));
                        }
                        break;
                    case 0b001:
                        alu_op = ALUControl::SLL;
                        break;
                    case 0b010:
                        alu_op = ALUControl::SLT;
                        break;
                    case 0b011:
                        alu_op = ALUControl::SLTU;
                        break;
                    case 0b100:
                        alu_op = ALUControl::XOR;
                        break;
                    case 0b101:
                        if (funct7 == 0) {
                            alu_op = ALUControl::SRL;
                        } else if (funct7 == 0b0100000) {
                            alu_op = ALUControl::SRA;
                        } else {
                            throw std::logic_error("invalid funct7 for SRL/SRA with funct3 == 0: " +\
                                std::to_string(funct7));
                        }
                        break;
                    case 0b110:
                        alu_op = ALUControl::OR;
                        break;
                    case 0b111:
                        alu_op = ALUControl::AND;
                        break;
                    default:
                        throw std::logic_error("invalid funct3: " + std::to_string(funct3));
                }
            }

            brn_cond = (opcode == 0b1100011);

            if (opcode == 0b1101111 || opcode == 0b1100111) {
                jmp_cond = true;
            } else {
                jmp_cond = false;
            }

            mem_to_reg = (opcode != 0b0000011);

            cmp_cond = (opcode == 0b1100011);
        }

        virtual void debug() const {
            std::cout << "ControlUnit: opcode=" << opcode << "; funct3=" << funct3 << "; funct7=" << funct7;
            std::cout << "; mem_to_reg=" << mem_to_reg << "; brn_cond=" << brn_cond << "; jmp_cond=" << jmp_cond;
            std::cout << "; cmp_cond=" << cmp_cond << "; alu_src2=" << alu_src2 << "; alu_op=" << alu_op << std::endl;
        }

        modules::byte_ opcode = 0;
        modules::byte_ funct3 = 0;
        modules::byte_ funct7 = 0;

    private:
        bool mem_to_reg = false;
        bool brn_cond = false;
        bool jmp_cond = false;
        // TODO: is the type right?
        bool cmp_cond = false;
        bool alu_src2 = false;
        ALUControl alu_op = ALUControl::NOP;
    };
}

#endif //RISC_V_SIMULATOR_CONTROL_UNIT_H
