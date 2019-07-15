cmake_minimum_required(VERSION 3.4.1)

set( DIR_physx ../../../../../../../../PhysX/physx )
set( DIR_pxshared ../../../../../../../../PhysX/pxshared )
set( DIR_physx_lowlevel_api ../../../../../../../../PhysX/physx/source/lowlevel/api/ )
set( DIR_physx_lowlevel_api_common ../../../../../../../../PhysX/physx/source/lowlevel/common/ )
set( DIR_physx_lowlevel_api_software ../../../../../../../../PhysX/physx/source/lowlevel/software/ )

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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/api/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/collision")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/pipeline")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/common/include/utils")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowlevel/software/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/lowleveldynamics/include")



add_library(lowlevel STATIC
			${DIR_physx_lowlevel_api}/src/px_globals.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcContactCache.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcContactMethodImpl.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcMaterialMesh.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcMaterialMethodImpl.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcMaterialShape.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcNpBatch.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcNpCacheStreamPair.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcNpContactPrepShared.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcNpMemBlockPool.cpp
			${DIR_physx_lowlevel_api_common}/src/pipeline/PxcNpThreadContext.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsCCD.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsContactManager.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsContext.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsDefaultMemoryManager.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsIslandSim.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsMaterialCombiner.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsNphaseImplementationContext.cpp
			${DIR_physx_lowlevel_api_software}/src/PxsSimpleIslandManager.cpp
            )

target_link_libraries(lowlevel
            android
            log
            m)
