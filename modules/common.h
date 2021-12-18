#ifndef RISC_V_SIMULATOR_COMMON_H
#define RISC_V_SIMULATOR_COMMON_H

#include <cstdint>

#ifdef DEBUG
#define DEBUG_LOG(obj) std::cout << std::dec << __LINE__ << std::hex << ": "; obj.debug()
#else
#define DEBUG_LOG(obj)
#endif

namespace modules {

    using byte_ = uint8_t;
    using byte2_ = uint16_t;
    using word_ = uint32_t;
}

#endif //RISC_V_SIMULATOR_COMMON_H
