#!/bin/sh

case "$1" in
    Debug|debug)
        BT=Debug
        ;;
    *)
        BT=Release
        ;;
esac

echo $BT build of Einstein...

# This is probably horrible.
SYSTEM=`cmake --system-information|grep CMAKE_HOST_SYSTEM_NAME| tr -d '"' |cut -d ' ' -f 2`

if [ $SYSTEM != "Linux" ]; then
    echo "Hey, we can only build Linux for now, your system seems to be $SYSTEM"
    exit 1;
fi

cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-32-linux.cmake -DCMAKE_BUILD_TYPE=$BT ../.. && make
