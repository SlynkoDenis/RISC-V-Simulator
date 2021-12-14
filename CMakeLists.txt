cmake_minimum_required(VERSION 3.17)
project(RISC_V_Simulator)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} LOWER_BUILD_TYPE)
if(${LOWER_BUILD_TYPE} MATCHES "debug")
    add_definitions(-DDEBUG)
endif()


set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(cpu main.cpp modules/address_exceptions.h)

target_include_directories(cpu PUBLIC module)

add_subdirectory(pipeline)

target_link_libraries(cpu PUBLIC
        pipeline
        )

#add_library(modules STATIC)
#
#add_library(pipeline STATIC)
#
#target_include_directories(modules INTERFACE modules/)
#
#target_include_directories(pipeline INTERFACE pipeline/)

#add_executable(RISC_V_Simulator
#        main.cpp
#        modules/register.h
#        modules/regfile.h
#        modules/common.h
#        pipeline/pipeline.cpp pipeline/pipeline.h
#        modules/instr_mem_unit.h
#        modules/data_mem_unit.h
#        modules/alu.h
#        modules/gates.h
#        pipeline/control_unit.h
#        pipeline/hazard_unit.h
#        pipeline/instruction.h
#        pipeline/we_gen.h
#        pipeline/imm.h
#        modules/cmp.h
#        )
