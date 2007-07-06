lib/libffi.a : src/ffi.c src/types.c src/sysv.S src/prep_cif.c
	/scratchbox/compilers/cs2005q3.2-glibc-arm/arm-none-linux-gnueabi/bin/gcc -Iinclude/ src/types.c -c -o src/types.o
	/scratchbox/compilers/cs2005q3.2-glibc-arm/arm-none-linux-gnueabi/bin/gcc -Iinclude/ src/ffi.c -c -o src/ffi.o
	/scratchbox/compilers/cs2005q3.2-glibc-arm/arm-none-linux-gnueabi/bin/gcc -Iinclude/ src/sysv.S -c -o src/sysv.o
	/scratchbox/compilers/cs2005q3.2-glibc-arm/arm-none-linux-gnueabi/bin/gcc -Iinclude/ src/prep_cif.c -c -o src/prep_cif.o
	/scratchbox/compilers/cs2005q3.2-glibc-arm/arm-none-linux-gnueabi/bin/ar r lib/libffi.a src/types.o src/ffi.o src/sysv.o src/prep_cif.o
