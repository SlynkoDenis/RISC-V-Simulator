#ifndef RISC_V_SIMULATOR_CMP_H
#define RISC_V_SIMULATOR_CMP_H

#include "common.h"


namespace modules {

    enum class CmpControl : byte_ {
        EQ,
        NE,
        LT,
        GE,
        LTU,
        GEU
    };

    inline std::ostream& operator<<(std::ostream& os, const CmpControl& cmp_control) {
        os << static_cast<byte_>(cmp_control);
        return os;
    }

    template <typename T>
    struct Cmp {
        inline bool operator()(CmpControl control, T lhs, T rhs) const {
            switch (control) {
                case CmpControl::EQ:
                    return lhs == rhs;
                case CmpControl::NE:
                    return lhs != rhs;
                case CmpControl::LT:
                case CmpControl::LTU:
                    return lhs < rhs;
                case CmpControl::GE:
                case CmpControl::GEU:
                    return lhs >= rhs;
            }
        }
    };
}

#endif //RISC_V_SIMULATOR_CMP_H
