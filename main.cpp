#include <vector>
#include "pipeline/pipeline.h"


int main() {
    std::vector<pipeline::word_> instructions = {
        0x00850513,     // addi a0, a0, 8
        0x00850513,
        0x00850513,
        0x00850513,
        0x00850513,
        0x00000013,     // addi x0, x0, 0 == nop
        0x00000013,
        0x00000013,
        0x00000013,
        0x00000013,
        0x00000013,
        0x00000013,
        0x0000006f
    };
    constexpr uint32_t start_pc = 8;
    pipeline::Pipeline cpu{instructions, start_pc};
    cpu.setProgramCounter(start_pc);

    try {
        while (true) {
            cpu.tick();
        }
    } catch (std::logic_error& e) {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
