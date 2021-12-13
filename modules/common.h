#ifndef RISC_V_SIMULATOR_COMMON_H
#define RISC_V_SIMULATOR_COMMON_H

#include <cstdint>

#define DEBUG_LOG(obj) std::cout << __FILE__ << ":" << __LINE__ << ": "; obj.debug()

namespace modules {

    using byte_ = uint8_t;
    using byte2_ = uint16_t;
    using word_ = uint32_t;
}

#endif //RISC_V_SIMULATOR_COMMON_H
