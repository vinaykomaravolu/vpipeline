cmake_minimum_required(VERSION 3.0.0)
project(Pipeline VERSION 0.1.0)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(BINARY ${CMAKE_PROJECT_NAME})

include(CPack)

include_directories(include)

file(GLOB SOURCE src/*.cpp)
add_executable(${BINARY} ${SOURCE})
add_library(${BINARY}_lib STATIC ${SOURCE})

target_include_directories(${BINARY}
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/include
)

add_subdirectory(lib/googletest)
add_subdirectory(test)