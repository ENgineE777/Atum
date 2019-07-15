cmake_minimum_required(VERSION 3.4.1)

set( DIR_physx ../../../../../../../../PhysX/physx )
set( DIR_pxshared ../../../../../../../../PhysX/pxshared )
set( DIR_physx_simulationcontroller ../../../../../../../../PhysX/physx/source/simulationcontroller/ )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11 -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/../pxshared/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/foundation/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxgpu/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/contact")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/common")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/convex")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/distance")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/sweep")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/gjk")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/intersection")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/mesh")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/hf")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/pcm")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/ccd")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/simulationcontroller/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/simulationcontroller/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/api/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/collision")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/pipeline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/utils")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/software/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowleveldynamics/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevelaabb/include")


add_library(simulationcontroller STATIC
			${DIR_physx_simulationcontroller}/src/ScActorCore.cpp
			${DIR_physx_simulationcontroller}/src/ScActorSim.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationCore.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationJointCore.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationJointSim.cpp
			${DIR_physx_simulationcontroller}/src/ScArticulationSim.cpp
			${DIR_physx_simulationcontroller}/src/ScBodyCore.cpp
			${DIR_physx_simulationcontroller}/src/ScBodySim.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintCore.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintGroupNode.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintInteraction.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintProjectionManager.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintProjectionTree.cpp
			${DIR_physx_simulationcontroller}/src/ScConstraintSim.cpp
			${DIR_physx_simulationcontroller}/src/ScElementInteractionMarker.cpp
			${DIR_physx_simulationcontroller}/src/ScElementSim.cpp
			${DIR_physx_simulationcontroller}/src/ScInteraction.cpp
			${DIR_physx_simulationcontroller}/src/ScIterators.cpp
			${DIR_physx_simulationcontroller}/src/ScMetaData.cpp
			${DIR_physx_simulationcontroller}/src/ScNPhaseCore.cpp
			${DIR_physx_simulationcontroller}/src/ScPhysics.cpp
			${DIR_physx_simulationcontroller}/src/ScRigidCore.cpp
			${DIR_physx_simulationcontroller}/src/ScRigidSim.cpp
			${DIR_physx_simulationcontroller}/src/ScScene.cpp
            )

target_link_libraries(simulationcontroller
            android
            log
            m)
