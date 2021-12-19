#include <iostream>
#include <unordered_map>
#include <vector>
#include "../pipeline/pipeline.h"
#include "gtest/gtest.h"


namespace pipeline::tests {

    class IntegrationTest : public ::testing::Test {
    public:
        IntegrationTest() {};

        virtual ~IntegrationTest() {};
    };

    TEST_F(IntegrationTest, FibonacciTest) {
        /*
         * lui x2, 0x20
         * jal main
         * li a2, 255 == addi a2, x0, 255
         * lui a3, 0x30
         * sb a2, 4(a3)
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * main:
         * li a0, 1
         * li a1, 1
         * li a3, 55
         * loop:
         * add a2, a1, a0
         * mv a0, a1 == addi a0, a1, 0
         * mv a1, a2
         * bne a3, a0, loop
         * ret == jr 0
         */
        const std::vector<pipeline::word_> instructions = {
               0x00020137,
               0x04c000ef,
               0x0ff00613,
               0x000306b7,
               0x00c68223,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00000013,
               0x00100513,
               0x00100593,
               0x03700693,
               0x00a58633,
               0x00b00533,
               0x00c005b3,
               0xfea69ae3,
               0x00008067
        };

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

        const word_ reg_addr0 = 10;
        const word_ reg_addr1 = 13;
        const word_ magic_number = 55;
        ASSERT_EQ(cpu.getRegister(reg_addr1), magic_number);
        ASSERT_EQ(cpu.getRegister(reg_addr0), magic_number);
    }

    TEST_F(IntegrationTest, HazardTest1) {
        /*
         * addi a1, a1, 15
         * addi a2, a2, 16
         * add  a0, a1, a2
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         * nop
         */
        const std::vector<pipeline::word_> instructions = {             
                0x00f58593,
                0x01060613,
                0x00c58533,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013
        };

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

        const word_ reg_addr0 = 10;
        const word_ reg_addr1 = 11;
        const word_ reg_addr2 = 12;
        const word_ magic_number0 = 31;
        const word_ magic_number1 = 15;
        const word_ magic_number2 = 16;
        ASSERT_EQ(cpu.getRegister(reg_addr1), magic_number1);
        ASSERT_EQ(cpu.getRegister(reg_addr2), magic_number2);
        ASSERT_EQ(cpu.getRegister(reg_addr0), magic_number0);
    }

    TEST_F(IntegrationTest, HazardTest2) {
        /*
         * jal test
         * addi  a0, a0, 8
         * addi  a0, a0, 8
         * addi  a0, a0, 8
         * addi  a0, a0, 8
         * test:
         * addi  a0, a0, 42
         * nop
         * nop
         * nop
         * nop
         * nop
         */
        const std::vector<pipeline::word_> instructions = {
                0x014000ef,
                0x00850513,
                0x00850513,
                0x00850513,
                0x00850513,
                0x02a50513,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013
        };

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

        const word_ reg_addr = 10;
        const word_ magic_number = 42;
        ASSERT_EQ(cpu.getRegister(reg_addr), magic_number);
    }

    TEST_F(IntegrationTest, HazardTest3) {
        /*
         * lw    a0, 8
         * addi  a0, a0, 8
         * addi  a0, a0, 8
         * addi  a0, a0, 42
         * nop
         * nop
         * nop
         * nop
         * nop
         */
        const std::vector<pipeline::word_> instructions = {
                0x00802503,
                0x00850513,
                0x00850513,
                0x02a50513,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013
        };
        const word_ magic_number = 1;
        const word_ data_addr_offset = 8;
        std::unordered_map<pipeline::word_, pipeline::word_> data = {
                {data_addr_offset, magic_number}
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

        const word_ reg_addr = 10;
        const word_ res = 59;
        ASSERT_EQ(cpu.getRegister(reg_addr), res);
    }

    TEST_F(IntegrationTest, HazardTest4) {
        /*
         * beq a6, a7, test
         * addi  a0, a0, 8
         * addi  a0, a0, 8
         * addi  a0, a0, 8
         * addi  a0, a0, 8
         * test:
         * addi  a0, a0, 8
         * nop
         * nop
         * nop
         * nop
         * nop
         */
        const std::vector<pipeline::word_> instructions = {
                0x01180a63,
                0x00850513,
                0x00850513,
                0x00850513,
                0x00850513,
                0x00850513,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013,
                0x00000013
        };

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

        const word_ reg_addr = 10;
        const word_ magic_number = 8;
        ASSERT_EQ(cpu.getRegister(reg_addr), magic_number);
    }
}
