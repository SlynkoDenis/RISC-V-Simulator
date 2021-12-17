#include <vector>
#include "pipeline/pipeline.h"


int main() {
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

    return 0;
}
