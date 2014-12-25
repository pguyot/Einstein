#!/bin/sh

# Derived from http://therealbnut.wordpress.com/2012/01/01/setting-xcode-4-0-environment-variables-from-a-script/
LLVM_XCCONFIG_FILE="${PROJECT_DIR}/EinsteinLLVM.xcconfig"
LLVM_XCCONFIG_FILE_TEMP="${PROJECT_DIR}/EinsteinLLVM.xcconfig.new"
LLVM_COMPONENTS="jit mcjit native interpreter"

if [ $"{ACTION}" != "clean" ]; then
    # Prefer llvm-3.5, accept 3.6 (HEAD) or use default version.
    LLVM_CONFIG_BIN=` \
        which llvm-config-mp-3.5  \
        || which llvm-config-3.5  \
        || which /opt/local/bin/llvm-config-mp-3.5  \
        || which /usr/local/bin/llvm-config-3.5  \
        || which llvm-config-mp-3.6  \
        || which llvm-config-3.6  \
        || which /opt/local/bin/llvm-config-mp-3.6  \
        || which /usr/local/bin/llvm-config-3.6  \
        || which /usr/local/opt/llvm/bin/llvm-config  \
        || which llvm-config  \
        `
    if [ "${LLVM_CONFIG_BIN}" = "" ]; then
        echo "Could not find llvm-config. Please install LLVM 3.5 with MacPorts or Homebrew."
        echo "Prefer a version without assertions."
        exit 1;
    fi
    LLVM_ASSERTION_MODE=`${LLVM_CONFIG_BIN} --assertion-mode`
    if [ "${LLVM_ASSERTION_MODE}" = "ON" ]; then
        echo "LLVM was compiled with assertions on. Please expect slowness or reinstall llvm without assertions"
    fi    
    LLVM_CFLAGS=`${LLVM_CONFIG_BIN} --cflags | sed -E 's|-O[a-z0-9]+||g'`
    LLVM_CXXFLAGS=`${LLVM_CONFIG_BIN} --cxxflags | sed -E 's|-O[a-z0-9]+||g'`
    LLVM_LDFLAGS=`${LLVM_CONFIG_BIN} --ldflags`" "`${LLVM_CONFIG_BIN} --libs $LLVM_COMPONENTS`" "`${LLVM_CONFIG_BIN} --system-libs`

    echo "// Configuration file for LLVM settings, generated as a build phase." > $LLVM_XCCONFIG_FILE_TEMP
    echo "// $LLVM_COMPONENTS" >> $LLVM_XCCONFIG_FILE_TEMP
    echo "LLVM_CFLAGS = $LLVM_CFLAGS" >> $LLVM_XCCONFIG_FILE_TEMP
    echo "LLVM_CXXFLAGS = $LLVM_CXXFLAGS" >> $LLVM_XCCONFIG_FILE_TEMP
    echo "LLVM_LDFLAGS = $LLVM_LDFLAGS" >> $LLVM_XCCONFIG_FILE_TEMP

    (diff -q $LLVM_XCCONFIG_FILE_TEMP $LLVM_XCCONFIG_FILE >/dev/null 2>&1)

    # Tell user that they need to rebuild the project.
    if [ -e $LLVM_XCCONFIG_FILE ]; then
        diff -q $LLVM_XCCONFIG_FILE_TEMP $LLVM_XCCONFIG_FILE >/dev/null 2>&1
        if [ $? -ne 0 ]; then
            mv $LLVM_XCCONFIG_FILE_TEMP $LLVM_XCCONFIG_FILE
            echo "EinsteinLLVM.xcconfig written as configuration changed."
            echo "Please rebuild."
            exit 2
        fi
    else 
        mv $LLVM_XCCONFIG_FILE_TEMP $LLVM_XCCONFIG_FILE
        echo "EinsteinLLVM.xcconfig file written."
        echo "Please rebuild."
        exit 2
    fi
    rm $LLVM_XCCONFIG_FILE_TEMP
fi
