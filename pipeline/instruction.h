#ifndef RISC_V_SIMULATOR_INSTRUCTION_H
#define RISC_V_SIMULATOR_INSTRUCTION_H

#include "../modules/common.h"


namespace pipeline::utils {

    using modules::byte_;
    using modules::word_;

    enum class InstructionType : modules::byte_ {
        UNKNOWN,
        RType,
        IType,
        SType,
        BType,
        UType,
        JType
    };

    inline byte_ getFunct7(word_ instruction) {
        return static_cast<byte_>((instruction >> 25) & 0x0000007f);
    }

    inline byte_ getRs2(word_ instruction) {
        return static_cast<byte_>((instruction >> 20) & 0x0000001f);
    }

    inline byte_ getRs1(word_ instruction) {
        return static_cast<byte_>((instruction >> 15) & 0x0000001f);
    }

    inline byte_ getFunct3(word_ instruction) {
        return static_cast<byte_>((instruction >> 12) & 0x00000007);
    }

    inline byte_ getRd(word_ instruction) {
        return static_cast<byte_>((instruction >> 7) & 0x0000001f);
    }

    inline byte_ getOpcode(word_ instruction) {
        return static_cast<byte_>(instruction & 0x0000007f);
    }

    inline InstructionType getInstructionType(byte_ opcode) {
        switch (opcode) {
            case 0b1100011:
            case 0b0100011:
            case 0b0110011:
                return InstructionType::BType;
            case 0b0000011:
            case 0b0010011:
            case 0b1110011:
                return InstructionType::IType;
            case 0b0110111:
            case 0b0010111:
                return InstructionType::UType;
            case 0b1101111:
                return InstructionType::JType;
            default:
                return InstructionType::UNKNOWN;
                //throw std::logic_error("invalid opcode: " + std::to_string(opcode));
        }
    }

    struct ImmediateExtensionBlock {
//        modules::word_ operator()(modules::word_ instruction, utils::InstructionType instr_t) {
//            modules::byte_ sign = utils::getSign(instruction);
//            modules::word_ res = 0;
//            switch (instr_t) {
//                case utils::InstructionType::IType:
//                    res = (instruction >> 20) & 0x00000fff;
//                    if (sign) {
//                        res |= 0xfffff000;
//                    }
//                    break;
//                case utils::InstructionType::SType:
//                    res = ((instruction >> 20) & 0x00000fc) | ((instruction >> 7) & 0x0000001f);
//                    if (sign) {
//                        res |= 0xfffff000;
//                    }
//            }
//            return res;
//        }
        inline word_ operator ()(word_ instr) {
            auto instr_t = utils::getInstructionType(utils::getOpcode(instr));
            if (instr_t == InstructionType::UNKNOWN) {
                return 0;
            }
            union imm_layout {
                word_ instr;
                struct {
                    word_ _pad0 : 20;
                    word_ imm0 : 12;
                } __attribute__((packed)) fi;
                struct {
                    word_ imm0 : 12;
                    word_ se : 20;
                } __attribute__((packed)) di;
                struct {
                    word_ _pad0 : 7;
                    word_ imm0 : 5;
                    word_ _pad1 : 13;
                    word_ imm1 : 7;
                } __attribute__((packed)) fs;
                struct {
                    word_ imm0 : 5;
                    word_ imm1 : 7;
                    word_ se : 20;
                } __attribute__((packed)) ds;
                struct {
                    word_ _pad0 : 7;
                    word_ imm2 : 1;
                    word_ imm0 : 4;
                    word_ _pad2 : 13;
                    word_ imm1 : 6;
                    word_ sign : 1;
                } __attribute__((packed)) fb;
                struct {
                    word_ imm0 : 4;
                    word_ imm1 : 6;
                    word_ imm2 : 1;
                    word_ se : 21;
                } __attribute__((packed)) db;
                struct {
                    word_ _pad0 : 12;
                    word_ imm2 : 8;
                    word_ imm1 : 1;
                    word_ imm0 : 10;
                    word_ sign : 1;
                } __attribute__((packed)) fj;
                struct {
                    word_ imm0 : 10;
                    word_ imm1 : 1;
                    word_ imm2 : 8;
                    word_ se : 13;
                } __attribute__((packed)) dj;
            } in{.instr = instr}, out{.instr = 0};
            byte_ sign = (in.instr >> 31) & 1;

            switch (instr_t) {
                case utils::InstructionType::IType:
                    out.di.imm0 = in.fi.imm0;
                    if (sign) {
                        out.di.se--;
                    }
                    break;
                case utils::InstructionType::SType:
                    out.ds.imm0 = in.fs.imm0;
                    out.ds.imm1 = in.fs.imm1;
                    if (sign) {
                        out.ds.se--;
                    }
                    break;
                case utils::InstructionType::BType:
                    out.db.imm0 = in.fb.imm0;
                    out.db.imm1 = in.fb.imm1;
                    out.db.imm2 = in.fb.imm2;
                    if (sign) {
                        out.db.se--;
                    }
                    break;
                case utils::InstructionType::JType:
                    out.dj.imm0 = in.fj.imm0;
                    out.dj.imm1 = in.fj.imm1;
                    out.dj.imm2 = in.fj.imm2;
                    if (sign) {
                        out.dj.se--;
                    }
                    break;
                default:
                    throw std::logic_error("invalid instruction type");
            }

            return out.instr;
        }
    };
}

#endif //RISC_V_SIMULATOR_INSTRUCTION_H
