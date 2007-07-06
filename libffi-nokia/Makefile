lib/libffi.a : src/ffi.c src/types.c src/sysv.S src/prep_cif.c
	/opt/crosstool/gcc-3.3.6-glibc-2.3.2/arm-unknown-linux-gnu/bin/arm-unknown-linux-gnu-gcc -Iinclude/ src/types.c -c -o src/types.o
	/opt/crosstool/gcc-3.3.6-glibc-2.3.2/arm-unknown-linux-gnu/bin/arm-unknown-linux-gnu-gcc -Iinclude/ src/ffi.c -c -o src/ffi.o
	/opt/crosstool/gcc-3.3.6-glibc-2.3.2/arm-unknown-linux-gnu/bin/arm-unknown-linux-gnu-gcc -Iinclude/ src/sysv.S -c -o src/sysv.o
	/opt/crosstool/gcc-3.3.6-glibc-2.3.2/arm-unknown-linux-gnu/bin/arm-unknown-linux-gnu-gcc -Iinclude/ src/prep_cif.c -c -o src/prep_cif.o
	/opt/crosstool/gcc-3.3.6-glibc-2.3.2/arm-unknown-linux-gnu/bin/arm-unknown-linux-gnu-ar r lib/libffi.a src/types.o src/ffi.o src/sysv.o src/prep_cif.o