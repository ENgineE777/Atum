cmake_minimum_required(VERSION 3.4.1)

set( DIR_physx ../../../../../../../../PhysX/physx )
set( DIR_pxshared ../../../../../../../../PhysX/pxshared )
set( DIR_physx_lowlevelaabb ../../../../../../../../PhysX/physx/source/lowlevelaabb/ )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11 -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/GpuBroadPhase/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/GpuBroadPhase/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/../pxshared/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/foundation/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/api/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/utils")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/pipeline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevelaabb/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevelaabb/src")



add_library(lowlevelaabb STATIC
			${DIR_physx_lowlevelaabb}/src/BpAABBManager.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhase.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhaseABP.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhaseMBP.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhaseSap.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhaseSapAux.cpp
			${DIR_physx_lowlevelaabb}/src/BpBroadPhaseShared.cpp
			${DIR_physx_lowlevelaabb}/src/BpMBPTasks.cpp
			${DIR_physx_lowlevelaabb}/src/BpSAPTasks.cpp
            )

target_link_libraries(lowlevelaabb
            android
            log
            m)
