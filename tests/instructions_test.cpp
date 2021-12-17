#include <iostream>
#include <unordered_map>
#include <vector>
#include "../pipeline/pipeline.h"
#include "gtest/gtest.h"


namespace pipeline::tests {

    class InstructionsTest : public ::testing::Test {
    public:
        InstructionsTest() {};

        virtual ~InstructionsTest() {};
    };

    TEST_F(InstructionsTest, AddiTest) {
        std::vector<pipeline::word_> instructions = {
            0x00850513,     // addi a0, a0, 8
            0x00000013,     // addi x0, x0, 0 == nop
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x00000013,
            0x0000006f      // jr 0
        };

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr};
        try {
            while (true) {
                cpu.tick();
            }
        } catch (std::logic_error& e) {
            std::cout << e.what() << std::endl;
        }
    }

    TEST_F(InstructionsTest, AddTest) {
        std::vector<pipeline::word_> instructions = {
                0x00c58533,     // add  a0, a1, a2
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr};
        try {
            while (true) {
                cpu.tick();
            }
        } catch (std::logic_error& e) {
            std::cout << e.what() << std::endl;
        }
    }

    TEST_F(InstructionsTest, MovTest) {
        std::vector<pipeline::word_> instructions = {
                0x00b00533,     // mv a0, a1
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr};
        try {
            while (true) {
                cpu.tick();
            }
        } catch (std::logic_error& e) {
            std::cout << e.what() << std::endl;
        }
    }

    TEST_F(InstructionsTest, LwTest) {
        std::vector<pipeline::word_> instructions = {
                0x00402503,     // lw a0
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };
        const pipeline::word_ magic_number = 42;
        std::unordered_map<pipeline::word_, pipeline::word_> data = {
                {4, magic_number}
        };

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr, data};
        cpu.setProgramCounter(start_inst_addr);
        try {
            while (true) {
                cpu.tick();
            }
        } catch (std::logic_error& e) {
            std::cout << e.what() << std::endl;
        }

        ASSERT_EQ(cpu.getRegister(10), magic_number);
    }

    TEST_F(InstructionsTest, SwTest) {
        std::vector<pipeline::word_> instructions = {
                0x02a50513,     // addi a0, a0, 42
                0x00000013,     // addi x0, x0, 0 == nop
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00a02223,     // sw a0
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x0000006f      // jr 0
        };

        const pipeline::byte_ reg_addr = 10;
        const pipeline::word_ magic_number = 42;
        const pipeline::word_ data_addr = 4;

        const pipeline::word_ start_inst_addr = 16;
        pipeline::Pipeline cpu{instructions, start_inst_addr};
        cpu.setProgramCounter(start_inst_addr);
        try {
            while (true) {
                cpu.tick();
            }
        } catch (std::logic_error& e) {
            std::cout << e.what() << std::endl;
        }

        ASSERT_EQ(cpu.getRegister(reg_addr), magic_number);
        ASSERT_EQ(cpu.getDataWord(data_addr), magic_number);
    }
}
