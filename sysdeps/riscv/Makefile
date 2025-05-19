ifeq ($(subdir),misc)
sysdep_headers += sys/asm.h
endif
sysdep-dl-routines += dl-cfi dl-get-cpu-features

ifeq ($(subdir),elf)
gen-as-const-headers += dl-link.sym features-offsets.sym
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
ifeq (1,$(riscv-use-cfi))
# Mark linker output CFI landing pad compatible, it warns on non-CFI inputs.
#sysdep-LDFLAGS += -Wl,-z,force-zicfilp
# Make warnings fatal outside the test system.
LDFLAGS-lib.so += -Wl,--fatal-warnings
#LDFLAGS-rtld += -Wl,-z,force-zicfilp,--fatal-warnings
else ifeq (2,$(riscv-use-cfi))
# Mark linker output CFI landing pad compatible, it warns on non-CFI inputs.
#sysdep-LDFLAGS += -Wl,-z,force-zicfilp-func-sig
# Make warnings fatal outside the test system.
LDFLAGS-lib.so += -Wl,--fatal-warnings
#LDFLAGS-rtld += -Wl,-z,force-zicfilp-func-sig,--fatal-warnings
endif
