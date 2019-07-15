cmake_minimum_required(VERSION 3.4.1)

set( DIR_physx ../../../../../../../../PhysX/physx )
set( DIR_pxshared ../../../../../../../../PhysX/pxshared )
set( DIR_physx_common ../../../../../../../../PhysX/physx/source/common/ )
set( DIR_physx_geomutils ../../../../../../../../PhysX/physx/source/geomutils/ )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11 -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/src/windows")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/src")
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
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/physxgpu/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/../pxshared/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/foundation/include")


add_library(physxcommon STATIC
			${DIR_physx_common}/src/CmCollection.cpp
			${DIR_physx_common}/src/CmMathUtils.cpp
			${DIR_physx_common}/src/CmPtrTable.cpp
			${DIR_physx_common}/src/CmRadixSort.cpp
			${DIR_physx_common}/src/CmRadixSortBuffered.cpp
			${DIR_physx_common}/src/CmRenderOutput.cpp
			${DIR_physx_common}/src/CmVisualization.cpp
			
			${DIR_physx_geomutils}/src/GuAABBTreeBuild.cpp
			${DIR_physx_geomutils}/src/GuBounds.cpp
			${DIR_physx_geomutils}/src/GuBox.cpp
			${DIR_physx_geomutils}/src/GuBVHStructure.cpp
			${DIR_physx_geomutils}/src/GuCapsule.cpp
			${DIR_physx_geomutils}/src/GuCCTSweepTests.cpp
			${DIR_physx_geomutils}/src/GuGeometryQuery.cpp
			${DIR_physx_geomutils}/src/GuGeometryUnion.cpp
			${DIR_physx_geomutils}/src/GuInternal.cpp
			${DIR_physx_geomutils}/src/GuMeshFactory.cpp
			${DIR_physx_geomutils}/src/GuMetaData.cpp
			${DIR_physx_geomutils}/src/GuMTD.cpp
			${DIR_physx_geomutils}/src/GuOverlapTests.cpp
			${DIR_physx_geomutils}/src/GuRaycastTests.cpp
			${DIR_physx_geomutils}/src/GuSerialize.cpp
			${DIR_physx_geomutils}/src/GuSweepMTD.cpp
			${DIR_physx_geomutils}/src/GuSweepSharedTests.cpp
			${DIR_physx_geomutils}/src/GuSweepTests.cpp

			${DIR_physx_geomutils}/src/ccd/GuCCDSweepConvexMesh.cpp
			${DIR_physx_geomutils}/src/ccd/GuCCDSweepPrimitives.cpp
            )

target_link_libraries(physxcommon
            android
            log
            m)
