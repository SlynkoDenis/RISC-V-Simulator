#ifndef RISC_V_SIMULATOR_INSTR_MEM_UNIT_H
#define RISC_V_SIMULATOR_INSTR_MEM_UNIT_H

#include <algorithm>
#include <array>
#include <vector>
#include "common.h"


namespace modules {

    class InstrMemUnit {
    public:
        explicit InstrMemUnit(const std::vector<word_>& instructions) : address(0) {
            std::copy(instructions.cbegin(), instructions.cend(), memory.begin());
        }

        virtual ~InstrMemUnit() noexcept = default;

        [[nodiscard]] word_ getData() const {
            return read_data;
        }

        virtual void tick() {
            read_data = memory.at(address);
        }

        virtual void debug() const {
            std::cout << "InstrMemUnit: read_data=" << read_data << "; address=" << address << std::endl;
        }

        word_ address = 0;

    private:
        static constexpr size_t capacity = 1024;
        std::array<word_, capacity> memory = {0};
        word_ read_data = 0;
    };
}

#endif //RISC_V_SIMULATOR_INSTR_MEM_UNIT_H
