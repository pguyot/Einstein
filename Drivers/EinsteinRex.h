
#ifndef EINSTEIN_REX_H
#define EINSTEIN_REX_H
extern unsigned char Einstein_rex[];
extern unsigned int Einstein_rex_len;
#endif

// ld -r -b binary -o binary.o binary.file
//  xxd -i – pmg

/*
 touch stub.c
 gcc -o stub.o -c stub.c
 ld -r -o foo.o -sectcreate binary foo_bin foo.bin stub.o

 #include <mach-o/getsect.h>
 const struct section_64 *sect = getsectbyname("binary", "foo_bin");
 char *buffer = calloc(1, sect->size+1);
 memcpy(buffer, sect->addr, sect->size); // whatever
 or getsectdata:

 #include <mach-o/getsect.h>
 size_t size;
 char *data = getsectdata("binary", "foo_bin", &size);
 char *buffer = calloc(1, size+1);
 memcpy(buffer, data, size); // whatever
 */

#if 0

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {printf("Usage:\n\thexembed <filename>\n"); return 1;}

    const char *fname = argv[1];
    FILE *fp = fopen(fname, "rb");
    if (!fp) {
        fprintf(stderr, "Error opening file: %s.\n", fname);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    const int fsize = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    unsigned char *b = malloc(fsize);

    fread(b, fsize, 1, fp);
    fclose(fp);

    printf("/* Embedded file: %s */\n", fname);
    printf("const int fsize = %d;\n", fsize);
    printf("const unsigned char *file = {\n");

    int i;
    for (i = 0; i < fsize; ++i) {
        printf("0x%02x%s",
               b[i],
               i == fsize-1 ? "" : ((i+1) % 16 == 0 ? ",\n" : ","));
    }
    printf("\n};\n");

    free(b);
    return 0;
}

#endif
