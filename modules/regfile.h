#ifndef RISC_V_SIMULATOR_REGFILE_H
#define RISC_V_SIMULATOR_REGFILE_H

#include <array>
#include <exception>


namespace modules {

    class RegFile {
    public:
        virtual ~RegFile() noexcept = default;

        virtual void tick() {
            if (write_enable3) {
                if (address3 == 0) {
                    throw std::logic_error("attempt to write into reg x0");
                }
                regs.at(address3) = write_data3;
            } else {
                read_data1 = regs.at(address1);
                read_data2 = regs.at(address2);
            }
        }

        [[nodiscard]] word_ getReadData1() const {
            return read_data1;
        }

        [[nodiscard]] word_ getReadData2() const {
            return read_data2;
        }

        void setNewSignals(byte_ addr1, byte_ addr2, byte_ addr3, bool we3, word_ wd3) {
            address1 = addr1;
            address2 = addr2;
            address3 = addr3;
            write_enable3 = we3;
            write_data3 = wd3;
        }

        virtual void debug() const {
            std::cout << "RegFile: read_data1=" << read_data1 << "; read_data2=" << read_data2;
            std::cout << "; write_enable3=" << write_enable3 << "; write_data3=" << write_data3;
            std::cout << "; address1=" << address1 << "; address2=" << address2 << "; address3=" << address3 << std::endl;
        }

        byte_ address1 = 0;
        byte_ address2 = 0;
        byte_ address3 = 0;
        bool write_enable3 = false;
        word_ write_data3 = 0;

    private:
        static constexpr size_t number_of_regs = 32;
        std::array<word_, number_of_regs> regs;
        word_ read_data1 = 0;
        word_ read_data2 = 0;
    };
}

#endif //RISC_V_SIMULATOR_REGFILE_H
