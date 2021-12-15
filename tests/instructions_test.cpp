#include <fstream>
#include <iostream>
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

        pipeline::Pipeline cpu{instructions, 16};
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

        pipeline::Pipeline cpu{instructions, 16};
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

        pipeline::Pipeline cpu{instructions, 16};
        try {
            while (true) {
                cpu.tick();
            }
        } catch (std::logic_error& e) {
            std::cout << e.what() << std::endl;
        }
    }
}
