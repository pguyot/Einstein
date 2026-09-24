cmake_minimum_required(VERSION 3.14)
project(EinsteinDrivers)

include(ExternalProject)
include(FetchContent)

# -- Check for ARM toolchain
find_program(ARM_GCC arm-none-eabi-gcc)
if (ARM_GCC)
    get_filename_component(ARM_GCC_PATH ${ARM_GCC} DIRECTORY)
    message(STATUS "Found ARM GCC at: ${ARM_GCC_PATH}")
    message(STATUS "Newton drivers will be built.")

    # -- Fetch NCT_Projects (Headers and platform file)
    FetchContent_Declare(
        nct_projects
        GIT_REPOSITORY https://github.com/ekoeppen/NCT_Projects.git
        GIT_TAG        main
    )
    FetchContent_MakeAvailable(nct_projects)
    set(NCT_PROJECTS_DIR ${nct_projects_SOURCE_DIR})

    # -- Setup Host Tools (DCL tools and tntk)
    add_subdirectory(host)

    # -- Build REx
    ExternalProject_Add(drivers_target
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/target
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/target-build
        # We depend on the tools targets defined in the host sub-project
        DEPENDS dcl_host tntk_host
        CMAKE_ARGS
            -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/newton-cross.cmake
            -DREX_TOOL=${DCL_BIN_DIR}/Rex
            -DELFTOPKG_TOOL=${DCL_BIN_DIR}/ELFtoPKG
            -DELFTONTK_TOOL=${DCL_BIN_DIR}/ELFtoNTK
            -DTNTK_TOOL=${TNTK_BIN_DIR}/tntk
            -DNCT_PROJECTS=${NCT_PROJECTS_DIR}
            -DEINSTEIN_ROOT=${CMAKE_CURRENT_SOURCE_DIR}/..
        INSTALL_COMMAND ""
        BUILD_ALWAYS 1
    )

    # Expose the final artifact to the parent project
    add_custom_target(EinsteinDrivers ALL)
    add_dependencies(EinsteinDrivers drivers_target)
    
    # Copy the finished REx to the build dir so the root can find it
    add_custom_command(TARGET EinsteinDrivers POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Einstein.rex ${CMAKE_CURRENT_SOURCE_DIR}/../_Data_/Einstein.rex)
else()
    message(STATUS "ARM toolchain NOT found. Skipping driver and host tool build.")
endif()
