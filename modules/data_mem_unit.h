#ifndef RISC_V_SIMULATOR_DATA_MEM_UNIT_H
#define RISC_V_SIMULATOR_DATA_MEM_UNIT_H

#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include "address_exceptions.h"
#include "common.h"


namespace modules {

    class DataMemUnit {
    public:
        DataMemUnit() = default;

        explicit DataMemUnit(const std::vector<word_>& data) : address(0) {
            std::copy(data.cbegin(), data.cend(), memory.begin());
        }

        virtual ~DataMemUnit() noexcept = default;

        // TODO: add alignment check
        virtual void tick() {
            if (write_enable) {
                if (address > capacity * sizeof(word_)) {
                    throw InvalidAddressException("data write overflow - address=" +\
                                                  std::to_string(address));
                }
                *reinterpret_cast<word_ *>(reinterpret_cast<byte_ *>(memory.data()) + address) = write_data;
                // read_data modification is useless, as we don't read after write
            } else {
                read_data = getData();
            }
        }

        [[nodiscard]] word_ getData() {
            if (address > capacity * sizeof(word_)) {
                throw InvalidAddressException("data read overflow - address=" + std::to_string(address));
            }
            return *reinterpret_cast<word_ *>(reinterpret_cast<byte_ *>(memory.data()) + address);
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
