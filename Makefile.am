ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS=-Wall -Werror -g
AM_CFLAGS+=-D DEBUG
AM_CFLAGS+=-D WARNING
AM_CFLAGS+=-D BIG_ENDIAN_SIMULATOR
# Uncomment if performance when running with -DDEBUG is an issue
# Warning, if uncommented, issuing calls to debug functions during options
# parsing might result in debug flag incorrectly set to 0 for some files
#AM_CFLAGS+=-D CACHE_DEBUG_FLAG

LDADD=-lpthread


SUBDIRS=
SUBDIRS+=. Examples_loader

bin_PROGRAMS= mainPrint mainReloc
bin_PROGRAMS+=ARM_runner_example
mainPrint_SOURCES= gestionErr.h gestionErr.c my_fonctions.h my_fonctions.c elf_header.h elf_header.c elf_sections.h elf_sections.c elf_hexdump.h elf_hexdump.c elf_symtab.h elf_symtab.c elf_reltab.h elf_reltab.c elf_reimplantation.h elf_reimplantation.c mainPrint.c
mainReloc_SOURCES=  gestionErr.h gestionErr.c my_fonctions.h my_fonctions.c elf_header.h elf_header.c elf_sections.h elf_sections.c elf_hexdump.h elf_hexdump.c elf_symtab.h elf_symtab.c elf_reltab.h elf_reltab.c elf_reimplantation.h elf_reimplantation.c mainReloc.c

COMMON=debug.h debug.c util.h util.c

EXTRA_DIST=filtre.pl
SIMULATOR_INTERFACE=csapp.h csapp.c scanner.h scanner.l gdb_protocol.h gdb_protocol.c arm_simulator_interface.h arm_simulator_interface.c

ARM_runner_example_SOURCES=$(COMMON) $(SIMULATOR_INTERFACE) ARM_runner_example.c

EXTRA_DIST+=sample_run.sh