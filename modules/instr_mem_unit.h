#ifndef RISC_V_SIMULATOR_INSTR_MEM_UNIT_H
#define RISC_V_SIMULATOR_INSTR_MEM_UNIT_H

#include <algorithm>
#include <array>
#include <vector>
#include "common.h"


namespace modules {

    class InstrMemUnit {
    public:
        explicit InstrMemUnit(const std::vector<word_>& instructions, word_ start_address) : address(0) {
            std::copy(instructions.cbegin() + start_address, instructions.cend(), memory.begin());
            end_of_section = start_address + instructions.size() * sizeof(word_);
        }

        virtual ~InstrMemUnit() noexcept = default;

        [[nodiscard]] word_ getData() {

            return *reinterpret_cast<word_ *>(reinterpret_cast<byte_ *>(memory.data()) + address);
        }

        [[nodiscard]] word_ getEndOfInstructionSection() const {
            return end_of_section;
        }

        virtual void debug() {
            std::cout << "InstrMemUnit: read_data=" << getData() << "; address=" << address << std::endl;
        }

        word_ address = 0;

    private:
        static constexpr size_t capacity = 1024;
        std::array<word_, capacity> memory = {0};
        word_ end_of_section = 0;
    };
}

#endif //RISC_V_SIMULATOR_INSTR_MEM_UNIT_H
