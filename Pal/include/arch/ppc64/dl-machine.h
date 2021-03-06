/* SPDX-License-Identifier: LGPL-3.0-or-later */
/* Copyright (C) 2014 Stony Brook University */

/*
 * dl-machine-ppc64.h
 *
 * This files contain architecture-specific implementation of ELF dynamic
 * relocation function.
 * The source code is imported and modified from the GNU C Library.
 */

#ifndef DL_MACHINE_H
#define DL_MACHINE_H

#define ELF_MACHINE_NAME "powerpc64"

#include <sysdeps/generic/ldsodefs.h>

#include "pal_internal.h"
#include "pal_rtld.h"

/* The ppc64 never uses Elf64_Rel relocations.  */
#define ELF_MACHINE_NO_REL 1

/* Perform the relocation specified by RELOC and SYM (which is fully resolved).
   MAP is the object containing the reloc.  */

//#define DEBUG_RELOC

static void elf_machine_rela(struct link_map* l, Elf64_Rela* reloc, Elf64_Sym* sym,
                             void* const reloc_addr_arg) {
    Elf64_Addr* const reloc_addr   = reloc_addr_arg;
    const unsigned long int r_type = ELF64_R_TYPE(reloc->r_info);

    const char* __attribute_unused strtab = (const void*)D_PTR(l->l_info[DT_STRTAB]);

#ifdef DEBUG_RELOC
#define debug_reloc(r_type)                                                              \
    do {                                                                                 \
        if (strtab && sym && sym->st_name)                                               \
            log_debug("%p " #r_type ": %s 0x%lx\n", reloc_addr, strtab + sym->st_name, value); \
        else if (value)                                                                  \
            log_debug("%p " #r_type ": 0x%lx\n", reloc_addr, value);                           \
        else                                                                             \
            log_debug("%p " #r_type "\n", reloc_addr);                                      \
    } while (0)
#else
#define debug_reloc(...) \
    do {                 \
    } while (0)
#endif

    //printf("r_type: %ld\n", r_type);

    if (r_type == R_PPC64_RELATIVE) {
        log_debug("!!! Skipping over a R_PPC_RELATIVE!\n");
        /* This is defined in rtld.c, but nowhere in the static libc.a;
           make the reference weak so static programs can still link.
           This declaration cannot be done when compiling rtld.c
           (i.e. #ifdef RTLD_BOOTSTRAP) because rtld.c contains the
           common defn for _dl_rtld_map, which is incompatible with a
           weak decl in the same file.  */

        //*reloc_addr = l->l_addr + reloc->r_addend;
        return;
    }

    if (r_type == R_PPC64_NONE)
        return;

    Elf64_Addr value = l->l_addr + sym->st_value;
#ifndef RTLD_BOOTSTRAP
    struct link_map* sym_map = 0;

    if (sym->st_shndx == SHN_UNDEF) {
        value = RESOLVE_RTLD(strtab + sym->st_name);

        if (!value) {
            sym_map = RESOLVE_MAP(&strtab, &sym);
            if (!sym_map)
                return;

            assert(sym);
            value = sym_map->l_addr + sym->st_value;
        }

#if CACHE_LOADED_BINARIES == 1
        if (!sym_map || sym_map->l_type == OBJECT_RTLD) {
            assert(l->nrelocs < NRELOCS);
            l->relocs[l->nrelocs++] = reloc_addr;
        }
#endif
    }
#endif

    if (ELFW(ST_TYPE)(sym->st_info) == STT_GNU_IFUNC && sym->st_shndx != SHN_UNDEF)
        value = ((Elf64_Addr(*)(void))value)();

    /* In the libc loader, they guaranteed that only R_ARCH_RELATIVE,
       R_ARCH_GLOB_DAT, R_ARCH_JUMP_SLOT appear in ld.so. We observed
       the same thing in libpal.so, so we are gonna to make the same
       assumption */
    switch (r_type) {
        case R_PPC64_GLOB_DAT:
            debug_reloc(R_PPC64_GLOB_DAT);
            *reloc_addr = value + reloc->r_addend;
            break;

        case R_PPC64_JMP_SLOT:
            debug_reloc(R_PPC64_JMP_SLOT);
            *reloc_addr = value + reloc->r_addend;
            break;

        case R_PPC64_ADDR64:
            debug_reloc(R_PPC64_ADDR64);
            *reloc_addr = value + reloc->r_addend;
            break;

        default:
            log_error("%s @ %d: Unhandled r_type %ld\nSTOP!\n", __func__, __LINE__, r_type);
            while(1);
            return;
    }

#ifndef RTLD_BOOTSTRAP
    /* We have relocated the symbol, we don't want the
       interpreter to relocate it again. */
    reloc->r_info ^= ELF64_R_TYPE(reloc->r_info);
#endif
}

static void elf_machine_rela_relative(struct link_map* l, const Elf64_Rela* reloc,
                                      void* const reloc_addr_arg) {
    Elf64_Addr* const reloc_addr = reloc_addr_arg;
    assert(ELF64_R_TYPE(reloc->r_info) == R_PPC64_RELATIVE);
    *reloc_addr = l->l_addr + reloc->r_addend;
}

#endif /* !DL_MACHINE_H */
