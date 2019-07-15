cmake_minimum_required(VERSION 3.4.1)

set( DIR_physx ../../../../../../../../PhysX/physx )
set( DIR_pxshared ../../../../../../../../PhysX/pxshared )
set( DIR_physx_scenequery ../../../../../../../../PhysX/physx/source/scenequery/ )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/../pxshared/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/foundation/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/common/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/common/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/contact")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/common")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/convex")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/distance")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/sweep")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/gjk")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/intersection")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/mesh")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/hf")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/pcm")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/geomutils/src/ccd")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/lowlevel/api/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/scenequery/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/simulationcontroller/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/physx/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/physx/src/buffering")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=$${DIR_physx}/source/pvd/include")


add_library(scenequery STATIC
			${DIR_physx_scenequery}/src/SqAABBPruner.cpp
			${DIR_physx_scenequery}/src/SqAABBTree.cpp
			${DIR_physx_scenequery}/src/SqAABBTreeUpdateMap.cpp
			${DIR_physx_scenequery}/src/SqBounds.cpp
			${DIR_physx_scenequery}/src/SqBucketPruner.cpp
			${DIR_physx_scenequery}/src/SqCompoundPruner.cpp
			${DIR_physx_scenequery}/src/SqCompoundPruningPool.cpp
			${DIR_physx_scenequery}/src/SqExtendedBucketPruner.cpp
			${DIR_physx_scenequery}/src/SqIncrementalAABBPruner.cpp
			${DIR_physx_scenequery}/src/SqIncrementalAABBPrunerCore.cpp
			${DIR_physx_scenequery}/src/SqIncrementalAABBTree.cpp
			${DIR_physx_scenequery}/src/SqMetaData.cpp
			${DIR_physx_scenequery}/src/SqPruningPool.cpp
			${DIR_physx_scenequery}/src/SqPruningStructure.cpp
			${DIR_physx_scenequery}/src/SqSceneQueryManager.cpp
            )

target_link_libraries(scenequery
            android
            log
            m)
