#ifndef RISC_V_SIMULATOR_HAZARD_UNIT_H
#define RISC_V_SIMULATOR_HAZARD_UNIT_H

#include "../modules/common.h"


namespace pipeline {

    enum class BypassOptionsEncoding : modules::byte_ {
        REG,
        MEM,
        WB
    };

    class HazardUnit {
    public:
        virtual ~HazardUnit() noexcept = default;

        [[nodiscard]] modules::byte_ getRs1() const {
            return static_cast<modules::byte_>(hu_rs1);
        }

        [[nodiscard]] modules::byte_ getRs2() const {
            return static_cast<modules::byte_>(hu_rs2);
        }

        virtual void tick() {

        }

        virtual void debug() const {

        }

    private:
        BypassOptionsEncoding hu_rs1 = BypassOptionsEncoding::REG;
        BypassOptionsEncoding hu_rs2 = BypassOptionsEncoding::REG;
    };
}

#endif //RISC_V_SIMULATOR_HAZARD_UNIT_H
