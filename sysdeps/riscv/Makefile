ifeq ($(subdir),misc)
sysdep_headers += sys/asm.h
endif

ifeq ($(subdir),elf)
gen-as-const-headers += dl-link.sym
endif

# RISC-V's assembler also needs to know about PIC as it changes the definition
# of some assembler macros.
ASFLAGS-.os += $(pic-ccflag)

ifeq (no,$(riscv-r-align))
ASFLAGS-.os += -Wa,-mno-relax
ASFLAGS-.o += -Wa,-mno-relax
sysdep-CFLAGS += -mno-relax
endif

# Enable RISC-V CFI
ifeq (yes,$(riscv-enable-cfi))
sysdep-dl-routines += dl-cfi
CFLAGS-.o += -fcf-protection=full
CFLAGS-.os += -fcf-protection=full
CFLAGS-.op += -fcf-protection=full
CFLAGS-.oS += -fcf-protection=full
asm-CPPFLAGS += -fcf-protection=full -include sysdep.h
endif
