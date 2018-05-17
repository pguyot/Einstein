cmake_minimum_required (VERSION 3.5)
include(CheckLibraryExists)
include(TestBigEndian)
include(FindPkgConfig)
project (k)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR})

set(_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(_FLAGS_RELEASE "-O3 -DRELEASE")

set(CMAKE_CXX_FLAGS_RELEASE "${_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DEBUG "${_FLAGS_DEBUG}")

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wno-unknown-pragmas -Wwrite-strings -Wno-multichar -Wfloat-equal -Wshadow -Wpointer-arith -Wconversion")
endif()

find_library(M_LIB m)
find_library(PTHREAD_LIB pthread)

TEST_BIG_ENDIAN(_IS_BIG_ENDIAN)

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
    set(SOURCES ${SOURCES} Misc/UPStrings.cp)
endif()

set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE CXX)

add_library(k STATIC ${SOURCES})

# because all of the K library .cp files have include file paths starting with "K/"
target_include_directories(k PUBLIC ../)
target_link_libraries(k ${M_LIB} ${PTHREAD_LIB})

string(TOUPPER ${CMAKE_HOST_SYSTEM_NAME} _LOCAL_LIB_DIR_SUFFIX)

install(TARGETS k DESTINATION lib/${CMAKE_HOST_SYSTEM_NAME}-${CMAKE_BUILD_TYPE})
