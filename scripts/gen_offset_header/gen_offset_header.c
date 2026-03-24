/*
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Minimal gen_offset_header with ELF32/ELF64 support, C89 compatible.
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <stdint.h>
#include <elf.h>

/* ensure macros exist even on old elf.h */
#ifndef ELF64_ST_BIND
#define ELF64_ST_BIND(i)   ((i)>>4)
#endif
#ifndef ELF_ST_BIND
#define ELF_ST_BIND        ELF32_ST_BIND
#endif

#define STRUCT_OFF_SUFFIX "_OFFSET"
#define STRUCT_SIZ_SUFFIX "_SIZEOF"

#ifndef OPEN_FLAGS
#if defined(_WIN32) || defined(__CYGWIN32__) || defined(__WIN32__)
#define OPEN_FLAGS (O_RDONLY|O_BINARY)
#else
#define OPEN_FLAGS (O_RDONLY)
#endif
#endif

static int elf_is_64;
static char swabRequired;
static Elf32_Ehdr ehdr32;
static Elf64_Ehdr ehdr64;
static Elf32_Shdr *shdr32;
static Elf64_Shdr *shdr64;

#define SWAB_Elf32_Half(x)   ((x >> 8) | (x << 8))
#define SWAB_Elf32_Word(x)   (((x >> 24) & 0xff) | ((x << 8) & 0xff0000) | ((x >> 8) & 0xff00) | ((x << 24) & 0xff000000))
#define SWAB_Elf32_Addr  SWAB_Elf32_Word
#define SWAB_Elf32_Off   SWAB_Elf32_Word
#define SWAB_Elf32_Sword SWAB_Elf32_Word

static void swabElfHdr32(Elf32_Ehdr *p)
{
    if (!swabRequired) {
        return;
    }
    p->e_type      = SWAB_Elf32_Half(p->e_type);
    p->e_machine   = SWAB_Elf32_Half(p->e_machine);
    p->e_version   = SWAB_Elf32_Word(p->e_version);
    p->e_entry     = SWAB_Elf32_Addr(p->e_entry);
    p->e_phoff     = SWAB_Elf32_Off(p->e_phoff);
    p->e_shoff     = SWAB_Elf32_Off(p->e_shoff);
    p->e_flags     = SWAB_Elf32_Word(p->e_flags);
    p->e_ehsize    = SWAB_Elf32_Half(p->e_ehsize);
    p->e_phentsize = SWAB_Elf32_Half(p->e_phentsize);
    p->e_phnum     = SWAB_Elf32_Half(p->e_phnum);
    p->e_shentsize = SWAB_Elf32_Half(p->e_shentsize);
    p->e_shnum     = SWAB_Elf32_Half(p->e_shnum);
    p->e_shstrndx  = SWAB_Elf32_Half(p->e_shstrndx);
}

static void swabElfSectionHdr32(Elf32_Shdr *p)
{
    if (!swabRequired) {
        return;
    }
    p->sh_name      = SWAB_Elf32_Word(p->sh_name);
    p->sh_type      = SWAB_Elf32_Word(p->sh_type);
    p->sh_flags     = SWAB_Elf32_Word(p->sh_flags);
    p->sh_addr      = SWAB_Elf32_Addr(p->sh_addr);
    p->sh_offset    = SWAB_Elf32_Off(p->sh_offset);
    p->sh_size      = SWAB_Elf32_Word(p->sh_size);
    p->sh_link      = SWAB_Elf32_Word(p->sh_link);
    p->sh_info      = SWAB_Elf32_Word(p->sh_info);
    p->sh_addralign = SWAB_Elf32_Word(p->sh_addralign);
}

static void swabElfSym32(Elf32_Sym *p)
{
    if (!swabRequired) {
        return;
    }
    p->st_name  = SWAB_Elf32_Word(p->st_name);
    p->st_value = SWAB_Elf32_Addr(p->st_value);
    p->st_size  = SWAB_Elf32_Word(p->st_size);
    p->st_shndx = SWAB_Elf32_Half(p->st_shndx);
}

static int ehdrLoad(int fd)
{
    unsigned char ident[EI_NIDENT];
    unsigned ix;

    if (lseek(fd, 0, SEEK_SET) == -1) {
        fprintf(stderr, "Unable to seek\n");
        return -1;
    }
    if (read(fd, ident, EI_NIDENT) != EI_NIDENT) {
        fprintf(stderr, "Failed to read ELF ident\n");
        return -1;
    }
    if (strncmp((char *)ident, ELFMAG, 4) != 0) {
        fprintf(stderr, "Input object module not ELF format\n");
        return -1;
    }

    elf_is_64 = (ident[EI_CLASS] == ELFCLASS64);

    if (lseek(fd, 0, SEEK_SET) == -1) {
        fprintf(stderr, "Unable to seek\n");
        return -1;
    }

    if (elf_is_64) {
        if (read(fd, &ehdr64, sizeof(ehdr64)) != (ssize_t)sizeof(ehdr64)) {
            fprintf(stderr, "Failed to read ELF64 header\n");
            return -1;
        }
        return 0;
    }

    if (read(fd, &ehdr32, sizeof(ehdr32)) != (ssize_t)sizeof(ehdr32)) {
        fprintf(stderr, "Failed to read ELF header\n");
        return -1;
    }

    ix = 0x12345678;
    if (((*(char*)&ix == 0x78) && (ehdr32.e_ident[EI_DATA] == ELFDATA2MSB)) ||
        ((*(char*)&ix == 0x12) && (ehdr32.e_ident[EI_DATA] == ELFDATA2LSB))) {
        swabRequired = 1;
    }
    swabElfHdr32(&ehdr32);
    return 0;
}

static int shdrsLoad(int fd)
{
    unsigned ix;

    if (elf_is_64) {
        shdr64 = malloc(ehdr64.e_shnum * sizeof(Elf64_Shdr));
        if (!shdr64) {
            fprintf(stderr, "No memory for section headers!\n");
            return -1;
        }
        if (lseek(fd, ehdr64.e_shoff, SEEK_SET) == -1) {
            fprintf(stderr, "Unable to seek\n");
            return -1;
        }
        for (ix = 0; ix < ehdr64.e_shnum; ix++) {
            if (read(fd, &shdr64[ix], sizeof(Elf64_Shdr)) != (ssize_t)sizeof(Elf64_Shdr)) {
                fprintf(stderr, "Unable to read entire section header (#%u)\n", ix);
                return -1;
            }
        }
        return 0;
    }

    shdr32 = malloc(ehdr32.e_shnum * sizeof(Elf32_Shdr));
    if (!shdr32) {
        fprintf(stderr, "No memory for section headers!\n");
        return -1;
    }
    if (lseek(fd, ehdr32.e_shoff, SEEK_SET) == -1) {
        fprintf(stderr, "Unable to seek\n");
        return -1;
    }
    for (ix = 0; ix < ehdr32.e_shnum; ix++) {
        if (read(fd, &shdr32[ix], sizeof(Elf32_Shdr)) != (ssize_t)sizeof(Elf32_Shdr)) {
            fprintf(stderr, "Unable to read entire section header (#%u)\n", ix);
            return -1;
        }
        swabElfSectionHdr32(&shdr32[ix]);
    }
    return 0;
}

static int symTblFind(uint64_t *off, uint64_t *size)
{
    unsigned ix;
    if (elf_is_64) {
        for (ix = 0; ix < ehdr64.e_shnum; ix++) {
            if (shdr64[ix].sh_type == SHT_SYMTAB) {
                *off = shdr64[ix].sh_offset;
                *size = shdr64[ix].sh_size;
                return 0;
            }
        }
    } else {
        for (ix = 0; ix < ehdr32.e_shnum; ix++) {
            if (shdr32[ix].sh_type == SHT_SYMTAB) {
                *off = shdr32[ix].sh_offset;
                *size = shdr32[ix].sh_size;
                return 0;
            }
        }
    }
    fprintf(stderr, "Object module missing symbol table!\n");
    return -1;
}

static int strTblFind(unsigned *idx)
{
    unsigned ix;
    unsigned found = 0xffffffff;

    if (elf_is_64) {
        for (ix = 0; ix < ehdr64.e_shnum; ix++) {
            if (shdr64[ix].sh_type == SHT_STRTAB) {
                if ((found == 0xffffffff) || (ix != ehdr64.e_shstrndx)) {
                    found = ix;
                }
            }
        }
    } else {
        for (ix = 0; ix < ehdr32.e_shnum; ix++) {
            if (shdr32[ix].sh_type == SHT_STRTAB) {
                if ((found == 0xffffffff) || (ix != ehdr32.e_shstrndx)) {
                    found = ix;
                }
            }
        }
    }

    if (found == 0xffffffff) {
        fprintf(stderr, "Object module missing string table!\n");
        return -1;
    }

    *idx = found;
    return 0;
}

static int strTblLoad(int fd, unsigned idx, char **pp)
{
    uint64_t size, offset;

    if (elf_is_64) {
        size = shdr64[idx].sh_size;
        offset = shdr64[idx].sh_offset;
    } else {
        size = shdr32[idx].sh_size;
        offset = shdr32[idx].sh_offset;
    }

    *pp = malloc(size);
    if (!*pp) {
        fprintf(stderr, "No memory for string table!\n");
        return -1;
    }
    if (lseek(fd, offset, SEEK_SET) == -1) {
        free(*pp);
        *pp = NULL;
        fprintf(stderr, "Unable to seek\n");
        return -1;
    }
    if (read(fd, *pp, size) != (ssize_t)size) {
        free(*pp);
        *pp = NULL;
        fprintf(stderr, "Unable to read entire string table!\n");
        return -1;
    }
    return 0;
}

static void headerPreambleDump(FILE *fp, char *filename)
{
    unsigned hash = 5381;
    char fileNameHash[20];
    size_t ix;

    fprintf(fp,
"/* %s - structure member offsets definition header */\n\n"
"/*\n"
" * Copyright (c) 2010-2014 Wind River Systems, Inc.\n"
" *\n"
" * Licensed under the Apache License, Version 2.0 (the \"License\");\n"
" * you may not use this file except in compliance with the License.\n"
" * You may obtain a copy of the License at\n"
" *\n"
" *     http://www.apache.org/licenses/LICENSE-2.0\n"
" *\n"
" * Unless required by applicable law or agreed to in writing, software\n"
" * distributed under the License is distributed on an \"AS IS\" BASIS,\n"
" * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
" * See the License for the specific language governing permissions and\n"
" * limitations under the License.\n"
" * SPDX-License-Identifier: Apache-2.0\n"
" */\n\n"
"/* THIS FILE IS AUTO GENERATED.  PLEASE DO NOT EDIT */\n\n"
"/*\n"
" * This header file provides macros for the offsets of various structure\n"
" * members.  These offset macros are primarily intended to be used in\n"
" * assembly code.\n"
" */\n\n", filename);

    for (ix = 0; filename[ix] != '\0'; ++ix) {
        hash = (hash * 33) + (unsigned int) filename[ix];
    }
    sprintf(fileNameHash, "_HGUARD_%08x", hash);

    fprintf(fp,
"/*\n"
" * Auto-generated header guard.\n"
" */\n"
"#ifndef %s\n"
"#define %s\n\n"
"#ifdef __cplusplus\n"
"extern \"C\" {\n"
"#endif\n\n"
"/* defines */\n\n", fileNameHash, fileNameHash);
}

static void headerAbsoluteSymbolsDump32(int fd, FILE *fp, Elf32_Off symOff,
                                        Elf32_Word symSize, char *strtbl)
{
    Elf32_Sym sym;
    unsigned ix;
    unsigned num = symSize / sizeof(Elf32_Sym);

    if (lseek(fd, symOff, SEEK_SET) == -1) {
        fprintf(stderr, "Unable to seek\n");
        return;
    }

    for (ix = 0; ix < num; ++ix) {
        if (read(fd, &sym, sizeof(sym)) != (ssize_t)sizeof(sym)) {
            break;
        }
        swabElfSym32(&sym);
        if (sym.st_shndx == SHN_ABS && ELF_ST_BIND(sym.st_info) == STB_GLOBAL) {
            const char *name = &strtbl[sym.st_name];
            if (strstr(name, STRUCT_OFF_SUFFIX) || strstr(name, STRUCT_SIZ_SUFFIX)) {
                fprintf(fp, "#define\t%s\t0x%X\n", name, sym.st_value);
            }
        }
    }
}

static void headerAbsoluteSymbolsDump64(int fd, FILE *fp, Elf64_Off symOff,
                                        Elf64_Xword symSize, char *strtbl)
{
    Elf64_Sym sym;
    unsigned ix;
    unsigned num = symSize / sizeof(Elf64_Sym);

    if (lseek(fd, symOff, SEEK_SET) == -1) {
        fprintf(stderr, "Unable to seek\n");
        return;
    }

    for (ix = 0; ix < num; ++ix) {
        if (read(fd, &sym, sizeof(sym)) != (ssize_t)sizeof(sym)) {
            break;
        }
        if (sym.st_shndx == SHN_ABS && ELF64_ST_BIND(sym.st_info) == STB_GLOBAL) {
            const char *name = &strtbl[sym.st_name];
            if (strstr(name, STRUCT_OFF_SUFFIX) || strstr(name, STRUCT_SIZ_SUFFIX)) {
                fprintf(fp, "#define\t%s\t0x%llX\n", name, (unsigned long long)sym.st_value);
            }
        }
    }
}

static void headerPostscriptDump(FILE *fp)
{
    fputs("\n#ifdef __cplusplus\n}\n#endif\n\n#endif /* _HGUARD_ */\n", fp);
}

int main(int argc, char *argv[])
{
    uint64_t symOff = 0, symSize = 0;
    char *strtbl = NULL;
    char *inFileName = NULL;
    char *outFileName = NULL;
    int option;
    int inFd = -1;
    FILE *outFile = NULL;
    unsigned strIdx = 0;

    if (argc != 5) {
        fprintf(stderr, "usage: %s -i <objectModule> -o <outputHeaderName>\n", argv[0]);
        goto errorReturn;
    }

    while ((option = getopt(argc, argv, "i:o:")) != -1) {
        switch (option) {
        case 'i':
            inFileName = optarg;
            break;
        case 'o':
            outFileName = optarg;
            break;
        default:
            fprintf(stderr, "usage: %s -i <objectModule> -o <outputHeaderName>\n", argv[0]);
            goto errorReturn;
        }
    }

    inFd = open(inFileName, OPEN_FLAGS);
    if (inFd == -1) {
        fprintf(stderr, "Cannot open input object module");
        goto errorReturn;
    }

    outFile = fopen(outFileName, "w");
    if (!outFile) {
        fprintf(stderr, "Cannot open output header file");
        goto errorReturn;
    }

    if ((ehdrLoad(inFd) != 0) ||
        (shdrsLoad(inFd) != 0) ||
        (symTblFind(&symOff, &symSize) != 0) ||
        (strTblFind(&strIdx) != 0) ||
        (strTblLoad(inFd, strIdx, &strtbl) != 0)) {
        goto errorReturn;
    }

    headerPreambleDump(outFile, outFileName);
    if (elf_is_64) {
        headerAbsoluteSymbolsDump64(inFd, outFile, symOff, symSize, strtbl);
    } else {
        headerAbsoluteSymbolsDump32(inFd, outFile, (Elf32_Off)symOff, (Elf32_Word)symSize, strtbl);
    }
    headerPostscriptDump(outFile);

    close(inFd);
    fclose(outFile);
    free(strtbl);
    free(shdr32);
    free(shdr64);
    return 0;

errorReturn:
    if (inFd != -1) {
        close(inFd);
    }
    if (outFile) {
        fclose(outFile);
    }
    free(strtbl);
    free(shdr32);
    free(shdr64);
    return 1;
}
