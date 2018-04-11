cmake_minimum_required (VERSION 3.5)
include(CheckLibraryExists)
include(TestBigEndian)
project (k)

# guard against in-source builds and bad build-type strings
#include(cmake/safeguards.cmake)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})


if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wwrite-strings -Wno-multichar -Wfloat-equal -Wshadow -Wpointer-arith -Wconversion")
    set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

find_library(M_LIB m)
#find_library(STDCPP_LIB "stdc++")
find_library(PTHREAD_LIB pthread)


# set(LIBRARY_OUTPUT_DIRECTORY _Build_/CMake)

#include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/Exceptions ${CMAKE_SOURCE_DIR}/Misc ${CMAKE_SOURCE_DIR}/Misc ${CMAKE_SOURCE_DIR}/Streams ${CMAKE_SOURCE_DIR}/Threads ${CMAKE_SOURCE_DIR}/Unicode)
#include_directories(Exceptions Misc Streams Threads Unicode)

# set(CMAKE_OSX_ARCHITECTURES "i386")

TEST_BIG_ENDIAN(_IS_BIG_ENDIAN)

#add_definitions(-DTARGET_RT_BIG_ENDIAN=${_IS_BIG_ENDIAN} -DTARGET_RT_LITTLE_ENDIAN=${_IS_LITTLE_ENDIAN})
add_definitions(-DTARGET_RT_BIG_ENDIAN=$<BOOL:${_IS_BIG_ENDIAN}> -DTARGET_RT_LITTLE_ENDIAN=$<NOT:${_IS_BIG_ENDIAN}>)

set(SOURCES Exceptions/TException.cp
Exceptions/Errors/TError.cp
Exceptions/Errors/TMemError.cp
Exceptions/IO/TIOException.cp
Exceptions/IO/TEOFException.cp
Misc/TCircleBuffer.cp
Misc/TMappedFile.cp
Streams/TFileStream.cp
Streams/TStream.cp
Threads/TCondVar.cp
Threads/TMutex.cp
Threads/TThread.cp
Unicode/UUTF16Conv.cp
Unicode/UUTF16CStr.cp)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    add_definitions (-DTARGET_OS_LINUX -DHAS_C99_LONGLONG -Wno-multichar)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
    add_definitions (-DTARGET_OS_OPENSTEP)
    set(SOURCES ${SOURCES} Misc/UpStrings.cp)
endif()


set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

#add_library (k SHARED ${SOURCES})
add_library(k STATIC ${SOURCES})

target_include_directories(k PUBLIC ../)
set_target_properties(k PROPERTIES LINK_FLAGS "-m32")

target_compile_options(k PUBLIC "-m32")

# check_library_exists("pthread" pthread_create "" HAVE_PTHREAD)
#     if (HAVE_PTHREAD)
#             target_compile_options(k PUBLIC "-lpthread")
#             # list(APPEND libs pthread)
#     else()
#             message(FATAL_ERROR "Missing library: pthread")
#     endif()

# target_compile_options(k PUBLIC -lstdc++ -lm)
# list(APPEND libs stdc++)
# list(APPEND libs m)

target_link_libraries(k ${M_LIB} ${PTHREAD_LIB})

string(TOUPPER ${CMAKE_HOST_SYSTEM_NAME} _LOCAL_LIB_DIR_SUFFIX)

install(TARGETS k DESTINATION lib.${_LOCAL_LIB_DIR_SUFFIX})
