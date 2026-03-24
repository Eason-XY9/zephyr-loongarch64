/*
 * Copyright (c) 2024
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Linker command/script file
 *
 * Generic Linker script for the loongarch platform
 */

#ifndef _LINKER
#define _LINKER
#endif
#ifndef __GCC_LINKER_CMD__
#define __GCC_LINKER_CMD__
#endif
#ifndef CONFIG_LOONGARCH64
#define CONFIG_LOONGARCH64 1
#endif

#if defined(__has_include)
#  if __has_include(<generated/autoconf.h>)
#    include <generated/autoconf.h>
#  else
#    include <autoconf.h>
#  endif
#else
#  include <autoconf.h>
#endif
#include <linker/sections.h>
#include <linker/linker-defs.h>
#include <linker/linker-tool.h>

/* Provide required device/thread symbols even when no generated sections exist */
PROVIDE(__device_PRE_KERNEL_1_start = 0);
PROVIDE(__device_PRE_KERNEL_2_start = 0);
PROVIDE(__device_POST_KERNEL_start = 0);
PROVIDE(__device_APPLICATION_start = 0);
PROVIDE(__device_init_end = 0);
PROVIDE(_static_thread_data_list_start = 0);
PROVIDE(_static_thread_data_list_end = 0);

/* Fallback defaults if config header is missing symbols */
#ifndef CONFIG_SRAM_BASE_ADDRESS
#ifdef CONFIG_LOONGARCH_RAM_BASE_ADDR
#define CONFIG_SRAM_BASE_ADDRESS CONFIG_LOONGARCH_RAM_BASE_ADDR
#else
#define CONFIG_SRAM_BASE_ADDRESS 0
#endif
#endif

#ifndef CONFIG_SRAM_SIZE
#ifdef CONFIG_LOONGARCH_RAM_SIZE_MB
#define CONFIG_SRAM_SIZE (CONFIG_LOONGARCH_RAM_SIZE_MB * 1024)
#else
#define CONFIG_SRAM_SIZE 0
#endif
#endif

#ifndef CONFIG_FLASH_BASE_ADDRESS
#ifdef CONFIG_LOONGARCH_ROM_BASE_ADDR
#define CONFIG_FLASH_BASE_ADDRESS CONFIG_LOONGARCH_ROM_BASE_ADDR
#else
#define CONFIG_FLASH_BASE_ADDRESS 0
#endif
#endif

#ifndef CONFIG_FLASH_SIZE
#ifdef CONFIG_LOONGARCH_ROM_SIZE
#define CONFIG_FLASH_SIZE (CONFIG_LOONGARCH_ROM_SIZE / 1024)
#else
#define CONFIG_FLASH_SIZE 0
#endif
#endif

#ifndef CONFIG_KERNEL_ENTRY
#define CONFIG_KERNEL_ENTRY __reset
#endif

#ifdef CONFIG_XIP
#define ROMABLE_REGION              ROM
#else
#define ROMABLE_REGION              RAM
#endif
#define RAMABLE_REGION              RAM

#define _EXCEPTION_SECTION_NAME     exceptions
#define _RESET_SECTION_NAME         reset

#if defined(CONFIG_ROM_END_OFFSET)
#define ROM_END_OFFSET CONFIG_ROM_END_OFFSET
#else
#define ROM_END_OFFSET 0
#endif

#if defined(CONFIG_FLASH_LOAD_OFFSET)
#define FLASH_LOAD_OFFSET CONFIG_FLASH_LOAD_OFFSET
#else
#define FLASH_LOAD_OFFSET 0
#endif

#ifdef CONFIG_XIP
#define ROM_BASE (CONFIG_FLASH_BASE_ADDRESS + FLASH_LOAD_OFFSET)
#ifndef ROM_SIZE
#define ROM_SIZE (CONFIG_FLASH_SIZE * 1024 - FLASH_LOAD_OFFSET - ROM_END_OFFSET)
#endif
#else /* CONFIG_XIP */
#define ROM_BASE CONFIG_SRAM_BASE_ADDRESS
#define ROM_SIZE ((CONFIG_SRAM_SIZE) * 1024 - ROM_END_OFFSET)
#endif /* CONFIG_XIP */

#define RAM_BASE CONFIG_SRAM_BASE_ADDRESS
#define RAM_SIZE ((CONFIG_SRAM_SIZE) * 1024)

#ifdef CONFIG_LOONGARCH_PMP
	#define MPU_MIN_SIZE CONFIG_PMP_GRANULARITY
	#define MPU_MIN_SIZE_ALIGN . = ALIGN(MPU_MIN_SIZE);
	#if defined(CONFIG_MPU_REQUIRES_POWER_OF_TWO_ALIGNMENT)
		#define MPU_ALIGN(region_size) \
			. = ALIGN(MPU_MIN_SIZE); \
			. = ALIGN( 1 << LOG2CEIL(region_size))
	#else
		#define MPU_ALIGN(region_size) \
			. = ALIGN(MPU_MIN_SIZE)
	#endif
#else
	#define MPU_MIN_SIZE_ALIGN
	#define MPU_ALIGN(region_size) . = ALIGN(4)
#endif


MEMORY
{
#ifdef CONFIG_XIP
    ROM (rx)  : ORIGIN = ROM_BASE, LENGTH = ROM_SIZE
#endif
    RAM (rwx) : ORIGIN = RAM_BASE, LENGTH = RAM_SIZE

#if defined(CONFIG_LINKER_DEVNULL_MEMORY)
    DEVNULL_ROM (rx) : ORIGIN = DEVNULL_ADDR, LENGTH = DEVNULL_SIZE
#endif

    /* Used by and documented in include/linker/intlist.ld */
    IDT_LIST  (wx)      : ORIGIN = 0xFFFFF000, LENGTH = 4K
}

ENTRY(CONFIG_KERNEL_ENTRY)

SECTIONS
    {

    /*
     * The .plt and .iplt are here according to
     * 'loongarch64-zephyr-elf-ld --verbose', before text section.
     */
    SECTION_PROLOGUE(.plt,,)
	{
		*(.plt)
	}

    SECTION_PROLOGUE(.iplt,,)
	{
		*(.iplt)
	}

    GROUP_START(ROMABLE_REGION)
    __rom_region_start = ROM_BASE;

    SECTION_PROLOGUE(rom_start,,)
    {
		. = ALIGN(16);
    } GROUP_LINK_IN(ROMABLE_REGION)

    SECTION_PROLOGUE(_RESET_SECTION_NAME,,)
    {
		KEEP(*(.reset.*))
    } GROUP_LINK_IN(ROMABLE_REGION)

    SECTION_PROLOGUE(_EXCEPTION_SECTION_NAME,,)
    {
		 KEEP(*(".exception.entry.*"))
		 *(".exception.other.*")
    } GROUP_LINK_IN(ROMABLE_REGION)

    SECTION_PROLOGUE(_TEXT_SECTION_NAME,,)
	{
		. = ALIGN(4);
		KEEP(*(.openocd_debug))
		KEEP(*(".openocd_debug.*"))

		__text_region_start = .;

		*(.text)
		*(".text.*")
		*(.gnu.linkonce.t.*)
	} GROUP_LINK_IN(ROMABLE_REGION)

    __text_region_end = .;

	__rodata_region_start = .;
#include <linker/common-rom.ld>

    SECTION_PROLOGUE(_RODATA_SECTION_NAME,,)
	{
		 . = ALIGN(4);
		 *(.srodata)
		 *(".srodata.*")
		 *(.rodata)
		 *(".rodata.*")
		 *(.gnu.linkonce.r.*)
		 *(.sdata2 .sdata2.* .gnu.linkonce.s2.*)

	. = ALIGN(4);
	} GROUP_LINK_IN(ROMABLE_REGION)

	__rodata_region_end = .;

	/* For non-XIP system, __rom_region_end symbol should be set to
	 * the end of common ROMABLE_REGIONs (text and rodata) instead of
	 * the linker script end, so it wouldn't mistakenly contain
	 * RAMABLE_REGION in it.
	 */
#ifndef CONFIG_XIP
#ifdef CONFIG_LOONGARCH_PMP
	SECTION_PROLOGUE(rom_mpu_padding,,)
	{
		MPU_ALIGN(__rodata_region_end - __rom_region_start);
#ifdef CONFIG_QEMU_TARGET
		/*
		 * QEMU doesn't vet each instruction fetch individually.
		 * Instead, it grabs a whole page and perform dynamic
		 * translation on it in a batch. It therefore validates
		 * PMP permissions using page-sized and -aligned chunks.
		 */
		. = ALIGN(0x1000);
#endif
	} GROUP_LINK_IN(ROMABLE_REGION)
#endif /* CONFIG_LOONGARCH_PMP */

	__rom_region_end = .;
	__rom_region_size = __rom_region_end - __rom_region_start;
#endif /* CONFIG_XIP */
    GROUP_END(ROMABLE_REGION)

    GROUP_START(RAMABLE_REGION)

	. = RAM_BASE;
	_image_ram_start = .;

    SECTION_DATA_PROLOGUE(_DATA_SECTION_NAME,,)
	{
		 . = ALIGN(4);
		/* _image_ram_start = .; */
		 __kernel_ram_start = .;
		 __data_region_start = .;
		 __data_start = .;

		 *(.data)
		 *(".data.*")

#ifdef CONFIG_LOONGARCH_GP
		/*
		 * LoongArch architecture has 12-bit signed immediate offsets in the
		 * instructions. If we can put the most commonly accessed globals
		 * in a special 4K span of memory addressed by the GP register, then
		 * we can access those values in a single instruction, saving both
		 * codespace and runtime.
		 *
		 * Since these immediate offsets are signed, place gp 0x800 past the
		 * beginning of .sdata so that we can use both positive and negative
		 * offsets.
		 */
		 . = ALIGN(8);
		 PROVIDE (__global_pointer$ = . + 0x800);
#endif

		 *(.sdata .sdata.* .gnu.linkonce.s.*)

		 __data_end = .;

	}  GROUP_DATA_LINK_IN(RAMABLE_REGION, ROMABLE_REGION)
	__data_size = __data_end - __data_start;
	__data_load_start = LOADADDR(_DATA_SECTION_NAME);

	__data_region_load_start = LOADADDR(_DATA_SECTION_NAME);

    __data_region_end = .;

    SECTION_DATA_PROLOGUE(_BSS_SECTION_NAME,(NOLOAD),)
	{
		MPU_MIN_SIZE_ALIGN
		/*
		 * For performance, BSS section is assumed to be 4 byte aligned and
		 * a multiple of 4 bytes
		 */
		 . = ALIGN(4);
		 __bss_start = .;
		 *(.sbss)
		 *(".sbss.*")
		 *(.bss)
		 *(".bss.*")
		 COMMON_SYMBOLS

		 /*
		  * As memory is cleared in words only, it is simpler to ensure the BSS
		  * section ends on a 4 byte boundary. This wastes a maximum of 3 bytes.
		  */
		  __bss_end = ALIGN(4);
	}  GROUP_DATA_LINK_IN(RAMABLE_REGION, RAMABLE_REGION)

	__kernel_ram_end = .;
	__kernel_ram_size = __kernel_ram_end - __kernel_ram_start;

    GROUP_END(RAMABLE_REGION)

    /DISCARD/ : { *(.note.GNU-stack) }

    SECTION_PROLOGUE(.loongarch.attributes, 0,)
	{
	KEEP(*(.loongarch.attributes))
	KEEP(*(.gnu.attributes))
	}

/* Output section descriptions are needed for these sections to suppress
 * warnings when "--orphan-handling=warn" is set for lld.
 */
#if defined(CONFIG_LLVM_USE_LLD)
    SECTION_PROLOGUE(.symtab, 0,) { *(.symtab) }
    SECTION_PROLOGUE(.strtab, 0,) { *(.strtab) }
    SECTION_PROLOGUE(.shstrtab, 0,) { *(.shstrtab) }
#endif

/* Because ROMABLE_REGION != RAMABLE_REGION in XIP-system, it is valid
 * to set __rom_region_end symbol at the end of linker script and
 * doesn't mistakenly contain the RAMABLE_REGION in it.
 */
#ifdef CONFIG_XIP
/* Must be last in romable region */
SECTION_PROLOGUE(.last_section,,)
{
  /* .last_section contains a fixed word to ensure location counter and actual
   * rom region data usage match when CONFIG_LINKER_LAST_SECTION_ID=y. */
  KEEP(*(.last_section))
  /* __rom_region_size is used when configuring the PMP entry of the ROM region.
   * Addresses (pmpaddr) in PMP registers need to be aligned to 4. Align
   * __rom_region_size to 4 to meet that requirement. */
  MPU_MIN_SIZE_ALIGN
} GROUP_LINK_IN(ROMABLE_REGION)

/* To provide the image size as a const expression,
 * calculate this value here. */
__rom_region_end = LOADADDR(.last_section) + SIZEOF(.last_section);
__rom_region_size = __rom_region_end - __rom_region_start;
#endif

}
