#ifndef RISC_V_SIMULATOR_CONTROL_UNIT_H
#define RISC_V_SIMULATOR_CONTROL_UNIT_H

#include <tuple>
#include "instruction.h"
#include "../modules/alu.h"
#include "../modules/cmp.h"
#include "../modules/common.h"


namespace pipeline {

    using modules::ALUControl;

    class ControlUnit {
    public:
        virtual ~ControlUnit() noexcept = default;

        [[nodiscard]] auto getControlSignals() {
            return std::make_tuple(wb_we, mem_we, mem_to_reg, brn_cond,
                                   jmp_cond, cmp_control, alu_src2, alu_op);
        }

        virtual void tick() {
            if ((opcode == 0b0100011 && funct3 == 0b010) ||
                (opcode == 0b1100011) ||
                (opcode == 0b1110011)) {
                wb_we = false;
            } else {
                wb_we = true;
            }

            mem_we = (opcode == 0b0100011 && funct3 == 0b010);

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
            } else {
                alu_op = ALUControl::NOP;
            }

            brn_cond = (opcode == 0b1100011);

            if (opcode == 0b1101111 || opcode == 0b1100111) {
                jmp_cond = true;
            } else {
                jmp_cond = false;
            }

            mem_to_reg = (opcode != 0b0000011);

            if (opcode == 0b1100011) {
                switch (funct3) {
                    case 0:
                        cmp_control = modules::CmpControl::EQ;
                        break;
                    case 0b001:
                        cmp_control = modules::CmpControl::NE;
                        break;
                    case 0b100:
                        cmp_control = modules::CmpControl::LT;
                        break;
                    case 0b101:
                        cmp_control = modules::CmpControl::GE;
                        break;
                    case 0b110:
                        cmp_control = modules::CmpControl::LTU;
                        break;
                    case 0b111:
                        cmp_control = modules::CmpControl::GEU;
                        break;
                    default:
                        throw std::logic_error("invalid funct3 for b instruction: " + std::to_string(funct3));
                }
            } else {
                cmp_control = modules::CmpControl::NOP;
            }
        }

        virtual void debug() const {
            std::cout << "ControlUnit: opcode=" << static_cast<modules::word_>(opcode) << "; funct3=";
            std::cout << static_cast<modules::word_>(funct3) << "; funct7=" << static_cast<modules::word_>(funct7);
            std::cout << "; wb_we=" << wb_we;
            std::cout << "; mem_to_reg=" << mem_to_reg << "; brn_cond=" << brn_cond << "; jmp_cond=" << jmp_cond;
            std::cout << "; cmp_cond=" << cmp_control << "; alu_src2=" << alu_src2 << "; alu_op=" << alu_op << std::endl;
        }

        modules::byte_ opcode = 0;
        modules::byte_ funct3 = 0;
        modules::byte_ funct7 = 0;

    private:
        bool wb_we = false;
        bool mem_we = false;
        bool mem_to_reg = false;
        bool brn_cond = false;
        bool jmp_cond = false;
        modules::CmpControl cmp_control;
        bool alu_src2 = false;
        ALUControl alu_op = ALUControl::NOP;
    };
}

#endif //RISC_V_SIMULATOR_CONTROL_UNIT_H
