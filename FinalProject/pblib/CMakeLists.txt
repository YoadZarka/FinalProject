PROJECT(pblib)

cmake_minimum_required(VERSION 2.8)

# SET (CMAKE_CXX_COMPILER "clang++")
 SET(CMAKE_CXX_COMPILER "g++")

set(CMAKE_CXX_FLAGS "-O3 -std=c++11")
# set(CMAKE_CXX_FLAGS "-O3 -std=c++11 -static")
# set(CMAKE_CXX_FLAGS "-O3 -g3 -std=c++11")
# set(CMAKE_CXX_FLAGS "-O1 -g3 -std=c++11")
# set(CMAKE_CXX_FLAGS "-O3 -g3 -std=c++0x")

# set (CMAKE_BUILD_TYPE "Debug")
 set (CMAKE_BUILD_TYPE "Release")

ADD_CUSTOM_TARGET(cpfiles ALL
   COMMAND mkdir -p lib/encoder &&  cp *.h lib/ && cp encoder/*.h lib/encoder/ && cp libpblib.* lib/
)


file(GLOB pblib_src_files
    "*.cpp", "encoder/*.cpp"
)

list(REMOVE_ITEM  pblib_src_files "${pblib_SOURCE_DIR}/fuzzer.cpp" "${pblib_SOURCE_DIR}/pbencoder.cpp")


add_library(pblib ${pblib_src_files})
# add_library(pblib SHARED ${pblib_src_files})


add_custom_target(tar COMMAND
    ${CMAKE_COMMAND} -E tar "czf" "pblib.tgz"  "*.h" "*.cpp" "encoder/*.h" "encoder/*.cpp"
)

add_custom_target(tar_lib COMMAND
    ${CMAKE_COMMAND} -E tar "czf" "pblib_static.tgz"  "libpblib.a" "lib/" "manual/pblib.pdf" "LICENSE" "pbencoder"  "manual/pblib.pdf"  "VERSION" 
)

add_dependencies(cpfiles pblib)
add_dependencies(tar_lib pblib cpfiles)
