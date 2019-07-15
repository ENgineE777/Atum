cmake_minimum_required(VERSION 3.4.1)

set( DIR_physx ../../../../../../../../PhysX/physx )
set( DIR_pxshared ../../../../../../../../PhysX/pxshared )
set( DIR_physx_characterkinematic ../../../../../../../../PhysX/physx/source/physxcharacterkinematic/ )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11 -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/common/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/contact")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/common")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/convex")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/distance")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/gjk")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/intersection")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/mesh")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/hf")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/geomutils/src/pcm")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/../pxshared/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/foundation/include")



add_library(characterkinematic STATIC
			${DIR_physx_characterkinematic}/src/CctBoxController.cpp
			${DIR_physx_characterkinematic}/src/CctCapsuleController.cpp
			${DIR_physx_characterkinematic}/src/CctCharacterController.cpp
			${DIR_physx_characterkinematic}/src/CctCharacterControllerCallbacks.cpp
			${DIR_physx_characterkinematic}/src/CctCharacterControllerManager.cpp
			${DIR_physx_characterkinematic}/src/CctController.cpp
			${DIR_physx_characterkinematic}/src/CctObstacleContext.cpp
			${DIR_physx_characterkinematic}/src/CctSweptBox.cpp
			${DIR_physx_characterkinematic}/src/CctSweptCapsule.cpp
			${DIR_physx_characterkinematic}/src/CctSweptVolume.cpp
            )

target_link_libraries(characterkinematic
            android
            log
            m)
