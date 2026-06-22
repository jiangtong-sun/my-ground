cmake_minimum_required(VERSION 3.16)

project(Session VERSION 1.01 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

add_definitions(-DQXlsx_SHAREDLIB)
add_definitions(-DCOMMON_SHAREDLIB)
add_definitions(-DBASEPROTO_SHAREDLIB)
add_definitions(-DBASEMEDIA_SHAREDLIB)
add_definitions(-DVEHICLE_SHAREDLIB)
add_definitions(-DZMQ_BUILD_DRAFT_API)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Qml Network Location Positioning)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Qml Network Location Positioning REQUIRED)
find_package(Qt5TextToSpeech)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/include)
include_directories(${CMAKE_SOURCE_DIR}/CSerialPort)
include_directories(${CMAKE_SOURCE_DIR}/GcsQuick)
include_directories(${CMAKE_SOURCE_DIR}/GLocation)
include_directories(${CMAKE_SOURCE_DIR}/BaseProto)
include_directories(${CMAKE_SOURCE_DIR}/Common)
include_directories(${CMAKE_SOURCE_DIR}/Vehicle)

link_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/lib)
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB SOURCE_FILES *.cpp *.cxx)
foreach (filepath ${SOURCE_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND sourcefiles ${filename})
endforeach (filepath)

file(GLOB HEADER_FILES *.h)
foreach (filepath ${HEADER_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND headerfiles ${filename})
endforeach (filepath)

list(APPEND headerfiles "QHosKey/qhotkey.h" "QHosKey/qhotkey_p.h")
list(APPEND sourcefiles "QHosKey/qhotkey.cpp" "QHosKey/qhotkey_win.cpp")
add_library(${PROJECT_NAME} "STATIC" ${sourcefiles} ${headerfiles})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Qml
    Qt${QT_VERSION_MAJOR}::Positioning
    Qt${QT_VERSION_MAJOR}::TextToSpeech
    QXlsx::QXlsx
    "Common.lib"
    "Vehicle.lib"
    "BaseProto.lib"
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION 0.${PROJECT_VERSION}
)
