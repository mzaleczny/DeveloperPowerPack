cmake_minimum_required(VERSION 3.29)

project(MariaDB_Samples VERSION 1.0)

################################################
## Prefer Clang if present
################################################
find_program(CLANG_C_COMPILER NAMES clang)
find_program(CLANG_CXX_COMPILER NAMES clang++)

if (CLANG_C_COMPILER AND CLANG_CXX_COMPILER)
    message(STATUS "Clang found ${CLANG_C_COMPILER}, ${CLANG_CXX_COMPILER}")
    set(CMAKE_C_COMPILER "${CLANG_C_COMPILER}" CACHE FILEPATH "" FORCE)
    set(CMAKE_CXX_COMPILER "${CLANG_CXX_COMPILER}" CACHE FILEPATH "" FORCE)
else()
    message(STATUS "Clang not found, using default compiler")
endif()
################################################
################################################


list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/../../libraries/cmake")
find_package(Tilc REQUIRED)

add_library(${PROJECT_NAME}_compiler_flags INTERFACE)
target_compile_features(${PROJECT_NAME}_compiler_flags INTERFACE cxx_std_23)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SetSubsystem "")
else()
    # if current system is windows and compiler is not clang then set subsystem
    if (NOT (CLANG_C_COMPILER AND CLANG_CXX_COMPILER))
        set(SetSubsystem "/SUBSYSTEM:CONSOLE")
    endif()
endif()

#set(SetSubsystem "/SUBSYSTEM:WINDOWS")

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CommonBinDir "Debug")
    set(BinDir "x64-Debug")
else()
    set(CommonBinDir "Release")
    set(BinDir "x64-Release")
endif()



function(AddAndConfigureProgram TARGET_NAME PROGRAM_FILES)
    add_executable(${TARGET_NAME} ${PROGRAM_FILES})

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries/.cache/mariadb-connector-c/include>"
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries/out/${CommonBinDir}/mariadb/install/include/mariadb>"
        )
    else()
        target_include_directories(${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR}
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>"
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries/.cache/mariadb-connector-c/include>"
            "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/../../libraries/out/${CommonBinDir}/mariadb/install/include/mariadb>"
        )
    endif()

    target_link_libraries(${TARGET_NAME} PUBLIC ${PROJECT_NAME}_compiler_flags Tilc::Tilc ws2_32 ${PROJECT_SOURCE_DIR}/../../libraries/out/${CommonBinDir}/mariadb/libmariadb/libmariadb.lib)
    target_link_options(${TARGET_NAME} PRIVATE "${SetSubsystem}")
    set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    # Turn off optimizations
    #set_source_files_properties(${TARGET_NAME} PROPERTIES COMPILE_FLAGS "/Od")
    set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endfunction()




AddAndConfigureProgram(MariaDB_01_Connection "src/MariaDB_01_Connection.cpp")
AddAndConfigureProgram(MariaDB_02_Select_And_Statement "src/MariaDB_02_Select_And_Statement.cpp")
AddAndConfigureProgram(MariaDB_03_CreateTable_InsertQuery_And_DropTable "src/MariaDB_03_CreateTable_InsertQuery_And_DropTable.cpp")
AddAndConfigureProgram(MariaDB_04_PreparedStatement "src/MariaDB_04_PreparedStatement.cpp")
AddAndConfigureProgram(MariaDB_05_Update_And_Delete "src/MariaDB_05_Update_And_Delete.cpp")
AddAndConfigureProgram(MariaDB_20_Select_And_Statement_From_Tilc "src/MariaDB_20_Select_And_Statement_From_Tilc.cpp")
AddAndConfigureProgram(MariaDB_21_CreateTable_InsertQuery_And_DropTable_From_Tilc "src/MariaDB_21_CreateTable_InsertQuery_And_DropTable_From_Tilc.cpp")
AddAndConfigureProgram(MariaDB_22_PreparedStatement_From_Tilc "src/MariaDB_22_PreparedStatement_From_Tilc.cpp")


# Dlls can be copied once for one target as all targets sits in the same directory
TilcNonGraphicsCopyRuntimeDlls(MariaDB_01_Connection)
MariaDBCopyRuntimeDlls(MariaDB_01_Connection)

TilcNonGraphicsCopyRuntimeDlls(MariaDB_02_Select_And_Statement)
MariaDBCopyRuntimeDlls(MariaDB_02_Select_And_Statement)

TilcNonGraphicsCopyRuntimeDlls(MariaDB_03_CreateTable_InsertQuery_And_DropTable)
MariaDBCopyRuntimeDlls(MariaDB_03_CreateTable_InsertQuery_And_DropTable)

TilcNonGraphicsCopyRuntimeDlls(MariaDB_04_PreparedStatement)
MariaDBCopyRuntimeDlls(MariaDB_04_PreparedStatement)

TilcNonGraphicsCopyRuntimeDlls(MariaDB_05_Update_And_Delete)
MariaDBCopyRuntimeDlls(MariaDB_05_Update_And_Delete)

TilcNonGraphicsCopyRuntimeDlls(MariaDB_20_Select_And_Statement_From_Tilc)
MariaDBCopyRuntimeDlls(MariaDB_20_Select_And_Statement_From_Tilc)

TilcNonGraphicsCopyRuntimeDlls(MariaDB_21_CreateTable_InsertQuery_And_DropTable_From_Tilc)
MariaDBCopyRuntimeDlls(MariaDB_21_CreateTable_InsertQuery_And_DropTable_From_Tilc)

TilcNonGraphicsCopyRuntimeDlls(MariaDB_22_PreparedStatement_From_Tilc)
MariaDBCopyRuntimeDlls(MariaDB_22_PreparedStatement_From_Tilc)
