cmake_minimum_required(VERSION 3.16)

project(BaseMedia VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

add_definitions(-DBASEMEDIA_SHAREDLIB)
add_definitions(-DBASEMEDIA_EXPORTS)
#add_definitions(-DENABLE_CUDA_DEVICE)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Quick Core)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick Core REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/include/cuda)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/include)
include_directories(${CMAKE_SOURCE_DIR}/Logging)
include_directories(${CMAKE_SOURCE_DIR}/Common)

link_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/lib)
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB CPP_FILES *.c *.cpp *.cxx *.h)

foreach (filepath ${CPP_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND src_files ${filename})
endforeach (filepath)

add_library(${PROJECT_NAME} SHARED ${src_files})

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(ExtentLib
        "spdlogd.lib"
        "opencv_core490d.lib"
        "opencv_ml490d.lib"
        "opencv_imgproc490d.lib"
        "opencv_imgcodecs490d.lib"
        "opencv_highgui490d.lib"
        "libzmq-v142-gd-4_3_5.lib"
    )
else ()
    set(ExtentLib
        "spdlog.lib"
        "opencv_imgcodecs490.lib"
        "opencv_core490.lib"
        "opencv_ml490.lib"
        "opencv_imgproc490.lib"
        "libzmq-v142-4_3_5.lib"
    )
endif ()


target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Quick
    ${ExtentLib}
    "cuda.lib"
    "cudart.lib"
    "common.lib"
    "avcodec.lib"
    "avdevice.lib"
    "avformat.lib"
    "swresample.lib"
    "swscale.lib"
    "avutil.lib"
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION 0.${PROJECT_VERSION}
)
