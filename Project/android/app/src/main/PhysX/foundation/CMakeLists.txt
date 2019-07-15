cmake_minimum_required(VERSION 3.4.1)

set( DIR_physx ../../../../../../../../PhysX/physx )
set( DIR_pxshared ../../../../../../../../PhysX/pxshared )
set( DIR_physx_foundation ../../../../../../../../PhysX/physx/source/foundation/ )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11 -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/../pxshared/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/foundation/include")

add_library(foundation STATIC
			${DIR_physx_foundation}/src/PsAllocator.cpp
			${DIR_physx_foundation}/src/PsAssert.cpp
			${DIR_physx_foundation}/src/PsFoundation.cpp
			${DIR_physx_foundation}/src/PsMathUtils.cpp
			${DIR_physx_foundation}/src/PsString.cpp
			${DIR_physx_foundation}/src/PsTempAllocator.cpp
			${DIR_physx_foundation}/src/PsUtilities.cpp
            )

target_link_libraries(foundation
            android
            log
            m)
