#include <vector>
#include "pipeline/pipeline.h"


int main() {
    std::vector<pipeline::word_> instructions = {
        0x014000ef,
        0x00850513,
        0x00850513,
        0x00850513,
        0x00850513,
        0x00850513,
        0x0ff00613,
        0x000306b7,
        0x00c68223,
        0x00000013,
        0x00000013,
        0x00000013,
        0x00000013,
        0x00000013,
        0x00000013,
        0x00000013
    };
    pipeline::Pipeline cpu{instructions};

    while(true) {
        cpu.tick();
    }

    return 0;
}
