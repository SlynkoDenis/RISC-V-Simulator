#ifndef RISC_V_SIMULATOR_DATA_MEM_UNIT_H
#define RISC_V_SIMULATOR_DATA_MEM_UNIT_H

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include "common.h"


namespace modules {

    class DataMemUnit {
    public:
        DataMemUnit() = default;

        explicit DataMemUnit(const std::vector<word_>& data) : address(0) {
            std::copy(data.cbegin(), data.cend(), memory.begin());
        }

        virtual ~DataMemUnit() noexcept = default;

        virtual void tick() {
            if (write_enable) {
                memory.at(address) = write_data;
                // TODO: modify read_data after write?
            } else {
                read_data = memory.at(address);
            }
        }

        [[nodiscard]] word_ getData() const {
            return memory.at(address);
        }

        void setNewSignals(bool we, word_ addr, word_ wd) {
            write_enable = we;
            address = addr;
            write_data = wd;
        }

        virtual void debug() const {
            std::cout << "DataMemUnit: read_data=" << read_data << "; write_enable=";
            std::cout << write_enable << "; address=" << address << std::endl;
        }

        bool write_enable = false;
        word_ address = 0;
        word_ write_data = 0;

    private:
        static constexpr size_t capacity = 4096;
        std::array<word_, capacity> memory = {0};
        word_ read_data = 0;
    };
}

#endif //RISC_V_SIMULATOR_DATA_MEM_UNIT_H
