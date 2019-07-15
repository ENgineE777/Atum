cmake_minimum_required(VERSION 3.4.1)

set( DIR_physx ../../../../../../../../PhysX/physx )
set( DIR_pxshared ../../../../../../../../PhysX/pxshared )
set( DIR_physx_lowleveldynamics ../../../../../../../../PhysX/physx/source/lowleveldynamics/ )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11 -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/../pxshared/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/foundation/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/contact")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/api/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/pipeline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/utils")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/software/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowleveldynamics/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowleveldynamics/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxgpu/include")


add_library(lowleveldynamics STATIC
			${DIR_physx_lowleveldynamics}/src/DyArticulation.cpp
			${DIR_physx_lowleveldynamics}/src/DyArticulationContactPrep.cpp
			${DIR_physx_lowleveldynamics}/src/DyArticulationContactPrepPF.cpp
			${DIR_physx_lowleveldynamics}/src/DyArticulationHelper.cpp
			${DIR_physx_lowleveldynamics}/src/DyArticulationScalar.cpp
			${DIR_physx_lowleveldynamics}/src/DyArticulationSIMD.cpp
			${DIR_physx_lowleveldynamics}/src/DyConstraintPartition.cpp
			${DIR_physx_lowleveldynamics}/src/DyConstraintSetup.cpp
			${DIR_physx_lowleveldynamics}/src/DyConstraintSetupBlock.cpp
			${DIR_physx_lowleveldynamics}/src/DyContactPrep.cpp
			${DIR_physx_lowleveldynamics}/src/DyContactPrep4.cpp
			${DIR_physx_lowleveldynamics}/src/DyContactPrep4PF.cpp
			${DIR_physx_lowleveldynamics}/src/DyContactPrepPF.cpp
			${DIR_physx_lowleveldynamics}/src/DyDynamics.cpp
			${DIR_physx_lowleveldynamics}/src/DyFeatherstoneArticulation.cpp
			${DIR_physx_lowleveldynamics}/src/DyFeatherstoneForwardDynamic.cpp
			${DIR_physx_lowleveldynamics}/src/DyFeatherstoneInverseDynamic.cpp
			${DIR_physx_lowleveldynamics}/src/DyFrictionCorrelation.cpp
			${DIR_physx_lowleveldynamics}/src/DyRigidBodyToSolverBody.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverConstraintsBlock.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverConstraints.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverConstraintsBlock.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverControl.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverControlPF.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverPFConstraints.cpp
			${DIR_physx_lowleveldynamics}/src/DySolverPFConstraintsBlock.cpp
			${DIR_physx_lowleveldynamics}/src/DyTGSContactPrep.cpp
			${DIR_physx_lowleveldynamics}/src/DyTGSContactPrepBlock.cpp
			${DIR_physx_lowleveldynamics}/src/DyTGSDynamics.cpp
			${DIR_physx_lowleveldynamics}/src/DyThreadContext.cpp
			${DIR_physx_lowleveldynamics}/src/DyThresholdTable.cpp
            )

target_link_libraries(lowleveldynamics
            android
            log
            m)
