cmake_minimum_required(VERSION 3.20)

project(GCSApp VERSION 1.0)



if (MSVC)
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
endif ()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DAFXF_PROJECT_CONF)

add_subdirectory(QXlsx)
add_subdirectory(Common)
add_subdirectory(CSerialPort)
add_subdirectory(GLocation)
add_subdirectory(BaseProto)
add_subdirectory(BaseMedia)
add_subdirectory(Vehicle)
add_subdirectory(Session)
add_subdirectory(Mission)
add_subdirectory(Drawing)
add_subdirectory(GcsMFmc)

