#ifndef RISC_V_SIMULATOR_GATES_H
#define RISC_V_SIMULATOR_GATES_H

#include <exception>
#include "common.h"


namespace modules {

    template <typename T>
    struct Add {
        inline T operator()(T lhs, T rhs) const {
            return lhs + rhs;
        }
    };

    template <typename T>
    struct And {
        inline T operator()(T lhs, T rhs) const {
            return lhs & rhs;
        }
    };

    template <typename T>
    struct Or {
        inline T operator()(T lhs, T rhs) const {
            return lhs | rhs;
        }
    };

    struct SignExtend {
        word_ operator()(byte2_ input) const {
            return static_cast<word_>(input);
        }
    };

    // TODO: rewrite multiplexers into one struct using ...
    template <typename T>
    struct Multiplexer2 {
        inline word_ operator()(bool control, T src1, T src2) const {
            return control ? src2 : src1;
        }
    };

    template <typename T>
    struct Multiplexer3 {
        inline word_ operator()(byte_ control, T src1, T src2, T src3) const {
            if (control == 0) {
                return src1;
            }
            if (control == 1) {
                return src2;
            }
            if (control == 2) {
                return src3;
            }
            throw std::logic_error("invalid control signal");
        }
    };

    template <typename T>
    struct Multiplexer4 {
        inline word_ operator()(byte_ control, T src1, T src2, T src3, T src4) const {
            if (control == 0) {
                return src1;
            }
            if (control == 1) {
                return src2;
            }
            if (control == 2) {
                return src3;
            }
            if (control == 3) {
                return src4;
            }
            throw std::logic_error("invalid control signal");
        }
    };
}

#endif //RISC_V_SIMULATOR_GATES_H
