cmake_minimum_required (VERSION 3.8)
project(unitree_legged_sdk)

set(CMAKE_C_FLAGS "$ENV{CFLAGS} -O2 -Wall -pthread")
set(CMAKE_CXX_FLAGS "$ENV{CFLAGS} -O2 -Wall -pthread -std=c++11")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

file(GLOB_RECURSE mqtt_c ${CMAKE_CURRENT_SOURCE_DIR}/mqtt/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mqtt)

set(ALL_COMPILE_SRC
    ${mqtt_c}
)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib usr/local/lib)
link_directories(usr/local/lib)

set(EXTRA_LIBS -pthread libunitree_legged_sdk_amd64.so lcm)

# 将源代码添加到此项目的可执行文件。
add_executable (mqttsend "mqttsend.cpp" ${ALL_COMPILE_SRC})
add_executable (mqttrecv "mqttrecv.cpp" ${ALL_COMPILE_SRC})
add_executable(KeyboardControl examples/KeyboardControl.cpp ${ALL_COMPILE_SRC})

target_link_libraries(mqttsend  -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs -lssl -lcrypto)
target_link_libraries(mqttrecv  -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs -lssl -lcrypto)
target_link_libraries(KeyboardControl ${EXTRA_LIBS} )
target_link_libraries(KeyboardControl  -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs -lssl -lcrypto)

#include_directories(include ${CMAKE_CURRENT_SOURCE_DIR}/mqtt)
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mqtt)

#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib usr/local/lib)
#link_directories(usr/local/lib)

#add_compile_options(-std=c++11)

#set(EXTRA_LIBS -pthread libunitree_legged_sdk_amd64.so lcm)

set(CMAKE_CXX_FLAGS "-O3")

#add_executable(KeyboardControl examples/KeyboardControl.cpp)
#target_link_libraries(KeyboardControl ${EXTRA_LIBS} -lpaho-mqtt3a -lpaho-mqtt3as -lpaho-mqtt3c -lpaho-mqtt3cs -lssl -lcrypto)



