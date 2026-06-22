cmake_minimum_required(VERSION 3.16)

project(CSerialPort VERSION 1.01 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

list(APPEND SourceFiles
        ibuffer.hpp
        ilog.hpp
        ithread.hpp
        itimer.hpp
        iutils.hpp
        CSerialPort.h
        SerialPortBase.h
        SerialPortInfo.h
        SerialPortInfoBase.h
        SerialPortInfoWinBase.h
        SerialPortListener.h
        SerialPort_global.h
        SerialPort_version.h
    )

list(APPEND SourceFiles
    CSerialPort.cpp
    SerialPortBase.cpp
    SerialPortInfo.cpp
    SerialPortInfoBase.cpp)

if (WIN32)
    list(APPEND SourceFiles SerialPortInfoWinBase.cpp SerialPortWinBase.cpp)
elseif (UNIX)
    list(APPEND SourceFiles SerialPortInfoUnixBase.cpp SerialPortUnixBase.cpp)
endif ()


if (NOT MSVC)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

add_library(${PROJECT_NAME} "STATIC" ${SourceFiles})

if (WIN32)
    target_link_libraries( ${PROJECT_NAME} setupapi)
elseif (UNIX)
    target_link_libraries( ${PROJECT_NAME} pthread)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION 0.${PROJECT_VERSION}
)
