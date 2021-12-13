#ifndef RISC_V_SIMULATOR_HAZARD_UNIT_H
#define RISC_V_SIMULATOR_HAZARD_UNIT_H

#include "../modules/common.h"


namespace pipeline {

    class HazardUnit {
    public:
        virtual ~HazardUnit() noexcept = default;

        [[nodiscard]] modules::byte_ getRs1() const {
            return hu_rs1;
        }

        [[nodiscard]] modules::byte_ getRs2() const {
            return hu_rs2;
        }

        virtual void tick() {

        }

        virtual void debug() const {

        }

    private:
        modules::byte_ hu_rs1 = 0;
        modules::byte_ hu_rs2 = 0;
    };
}

#endif //RISC_V_SIMULATOR_HAZARD_UNIT_H
