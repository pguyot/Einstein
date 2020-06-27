PHDRS {
    text PT_LOAD ;
    data PT_LOAD ;
    dynamic PT_DYNAMIC ;
}

SECTIONS
{
  . = 0;
/* .interp section should be first, containing Protocol glue */
  .interp : { *(.interp) } : text
/* Put other interesting sections first to reduce total file size */
  .text : { *(.text*) } : text
  .rodata : { *(.rodata*) } : text
  .data.rel.ro : { *(.data.rel.ro*) } : text
  .plt : { *(.plt*) } : text
  .got : { *(.got*) } : text
/* Other sections */
  .dynamic : { *(.dynamic*) } : data : dynamic
  .hash : { *(.hash) } : data
  .rel.dyn : { *(.rel.dyn) } : data
  .dynstr : { *(.dynstr) } : data
  .dynsym : { *(.dynsym) } : data
  .gnu.version : { *(.gnu.version) } : data
  .gnu.version_d : { *(.gnu.version_d) } : data
  .symtab : { *(.symtab) } : data
  .strtab : { *(.strtab) } : data
  .shstrtab : { *(.shstrtab) } : data
}
