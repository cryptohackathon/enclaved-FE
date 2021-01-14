cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
project(fe_sgx_demo VERSION 0.1.0 LANGUAGES C CXX)

Set(APP_NAME fe_sgx_demo_client)

message(STATUS "Using ${CMAKE_CXX_COMPILER}")

set_property(TARGET ${target} PROPERTY C_STANDARD 11)
set_property(TARGET ${target} PROPERTY CXX_STANDARD 14)

find_package(CURL REQUIRED)

message(STATUS "Curl library: ${CURL_LIBRARIES}")

set(DEFAULT_LIBRARIES ${CURL_LIBRARIES} pthread cifer gmp)
include_directories(
    ${CURL_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/include
    )

link_directories( ${CURL_LIBRARIES} )


add_executable(${APP_NAME} main.cpp EncryptorClient.cpp)
target_link_libraries(${APP_NAME} ${DEFAULT_LIBRARIES}
)
