cmake_minimum_required(VERSION 3.10.2)
project(myitem)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

include_directories(.)
include_directories(./SourceCodes)

set(LIB_SRC
    SourceCodes/fd_manager.cc
    SourceCodes/fiber.cc
    SourceCodes/hook.cc
    SourceCodes/iomanager.cc
    SourceCodes/scheduler.cc
    SourceCodes/thread.cc
    SourceCodes/timer.cc
    )

set(
    LIB_LIB
    myitem
    dl
    pthread
)

#生成动态库myitem
add_library(myitem SHARED ${LIB_SRC})

add_executable(mytest ./test/mytest.cc)
#add_dependencies表示告诉编译器要生成mytest可执行程序，需要先生成sylar库
add_dependencies(mytest sylar)
target_link_libraries(mytest ${LIB_LIB})

add_executable(hook_test ./test/hook_test.cc)
#add_dependencies表示告诉编译器要生成mytest可执行程序，需要先生成sylar库
add_dependencies(hook_test sylar)
target_link_libraries(hook_test ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)