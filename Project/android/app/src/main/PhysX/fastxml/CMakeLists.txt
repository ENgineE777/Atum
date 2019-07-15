cmake_minimum_required(VERSION 3.4.1)

set( DIR_physx ../../../../../../../../PhysX/physx )
set( DIR_pxshared ../../../../../../../../PhysX/pxshared )
set( DIR_physx_fastxml ../../../../../../../../PhysX/physx/source/fastxml/ )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11 -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/../pxshared/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/foundation/include")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --include-directory=${DIR_physx}/source/fastxml/include")

add_library(fastxml STATIC
			${DIR_physx_fastxml}/src/PsFastXml.cpp
            )

target_link_libraries(fastxml
            android
            log
            m)
