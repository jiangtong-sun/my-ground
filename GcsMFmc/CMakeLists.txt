cmake_minimum_required(VERSION 3.16)

project(GCSMFmc VERSION 1.0.1.12 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

enable_language(RC)

add_definitions(-DQXlsx_SHAREDLIB)
add_definitions(-DCOMMON_SHAREDLIB)
add_definitions(-DBASEPROTO_SHAREDLIB)
add_definitions(-DVEHICLE_SHAREDLIB)
add_definitions(-DZMQ_BUILD_DRAFT_API)
add_definitions(-DQT_QML_DEBUG_NO_WARNING)

#输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin/${CMAKE_BUILD_TYPE})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

#导入第三方库
include_directories(${CMAKE_SOURCE_DIR}/Common)
include_directories(${CMAKE_SOURCE_DIR}/BaseProto)
include_directories(${CMAKE_SOURCE_DIR}/BaseMedia)
include_directories(${CMAKE_SOURCE_DIR}/GcsQuick)
include_directories(${CMAKE_SOURCE_DIR}/Vehicle)
include_directories(${CMAKE_SOURCE_DIR}/Session)
include_directories(${CMAKE_SOURCE_DIR}/GLocation)
include_directories(${CMAKE_SOURCE_DIR}/Mission)
include_directories(${CMAKE_SOURCE_DIR}/Drawing)

include_directories(${CMAKE_SOURCE_DIR}/QXlsx/header)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/include)
link_directories(${CMAKE_SOURCE_DIR}/3rdparty/win32/lib)
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
link_directories(${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/GcsQuick)

#导入Qt相关依赖包
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Quick Svg Network Widgets Positioning Location Sql PrintSupport QuickControls2)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick Svg Network Widgets Positioning Location Sql PrintSupport QuickControls2 REQUIRED)
find_package(Qt5TextToSpeech)

#遍历所有Cpp文件
file(GLOB_RECURSE CPP_FILES *.h *.cpp)
foreach (filepath ${CPP_FILES})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" filename ${filepath})
    list(APPEND sources_files ${filename})
endforeach (filepath)

if (WIN32)
    list(APPEND sources_files "MiniDmp.h")
endif ()

list(APPEND sources_files gcsfmchudfmp.cpp)
list(APPEND sources_files gcsfmchudfmp.qrc)

set(QML_IMPORT_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/qmls
    CACHE STRING "" FORCE)

# 可执行程序
list(APPEND QML_EXT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/qmls")
set(QML_IMPORT_PATH "${QML_EXT_DIR}" CACHE STRING "qmls" FORCE)
add_executable(${PROJECT_NAME} ${sources_files})

# 拷贝运行文件
if (WIN32)
    file(GLOB_RECURSE 3RDPARTY_DLL_DIR ${CMAKE_SOURCE_DIR}/3rdparty/*.dll)
    file(COPY ${3RDPARTY_DLL_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif ()

if (WIN32)
    file(GLOB_RECURSE 3RDPARTY_DLL_DIR ${CMAKE_SOURCE_DIR}/3rdparty/res/*.*)
    file(COPY ${3RDPARTY_DLL_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res)
endif ()

file(GLOB_RECURSE 3RDPARTY_DLL_DIR ${CMAKE_SOURCE_DIR}/3rdparty/xfres/*.*)
file(COPY ${3RDPARTY_DLL_DIR} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/xfres)

# 可执行程序属性设置
set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    OUTPUT_NAME "gcsapp"
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(ExtentLib
        "spdlogd.lib"
        "libzmq-v142-gd-4_3_5.lib")
else ()
    set(ExtentLib
        "spdlog.lib"
        "libzmq-v142-4_3_5.lib"
    )
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Quick
        Qt${QT_VERSION_MAJOR}::Svg
        Qt${QT_VERSION_MAJOR}::Network
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Positioning
        Qt${QT_VERSION_MAJOR}::PrintSupport
        Qt${QT_VERSION_MAJOR}::QuickControls2
        Qt${QT_VERSION_MAJOR}::TextToSpeech
	QXlsx::QXlsx
	${ExtentLib}
        GLocation
        "uv.lib"
        "Ws2_32.lib"
        "Common.lib"
        "Vehicle.lib"
        "Session.lib"
        "Drawing.lib"
        "BaseProto.lib"
        "Mission.lib"
        "BaseMedia.lib"
        "CSerialPort.lib"
        "GcsQuick.lib"
        "cuda.lib"
        "cudart.lib"
        "avcodec.lib"
        "avdevice.lib"
        "avformat.lib"
        "swresample.lib"
        "swscale.lib"
        "avutil.lib"
        "JoyStick.lib"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    APP_VERSION="${PROJECT_VERSION}.g"
)
