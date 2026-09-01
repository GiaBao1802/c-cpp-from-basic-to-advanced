# Toolchain, Makefile, CMake và linker script

## Pipeline thật

```text
source → preprocess (.i) → assembly (.s) → object (.o) → ELF → HEX/BIN
```

Học bằng GCC/Clang:

```bash
gcc -E module.c -o module.i
gcc -S module.c -o module.s
gcc -c module.c -o module.o
gcc module.o main.o -Wl,-Map=app.map -o app.elf
nm -C app.elf
objdump -h -d app.elf
readelf -S -s app.elf
```

## Makefile

```make
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Werror -MMD -MP
SRC := $(wildcard src/*.c)
OBJ := $(SRC:src/%.c=build/%.o)

app: $(OBJ)
	$(CC) $(OBJ) -Wl,-Map=build/app.map -o $@

build/%.o: src/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(OBJ:.o=.d)
```

Target, prerequisite và recipe tạo dependency graph. `.PHONY`, automatic variables `$@/$</$^`, generated dependency và parallel build là phần cần hiểu. Make không tự biết header dependency nếu không tạo `.d`.

## CMake

CMake tạo build system, không phải compiler. Dùng target-based commands: `target_sources`, `target_include_directories`, `target_compile_definitions`, `target_link_libraries`; tránh global flags làm dependency ẩn.

## Linker script

```ld
MEMORY {
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
}
SECTIONS {
  .text : { KEEP(*(.vectors)) *(.text*) *(.rodata*) } > FLASH
  .data : { __data_start = .; *(.data*) __data_end = .; } > RAM AT> FLASH
  .bss (NOLOAD) : { __bss_start = .; *(.bss*) *(COMMON) __bss_end = .; } > RAM
}
```

VMA là địa chỉ runtime; LMA là nơi initial image lưu trong Flash. Startup copy `.data` LMA→VMA và clear `.bss`. `KEEP` ngăn garbage collection xóa vector/section chỉ được reference gián tiếp.

## Senior build concerns

Reproducible build, pinned toolchain, warning policy, LTO, debug/release parity, generated code dependency, link-time dead stripping, symbol visibility, static/shared library, cross-compilation toolchain file, build cache và SBOM/license scanning.

Trong AUTOSAR, map file giúp kiểm tra per-core section, flash/RAM budget, task stack, calibration, bootloader/application boundary và MemMap placement.
