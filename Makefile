# msp430fr-firmware-gcc-skeleton
#
# Please read the README.md file
#
# License: The code is released under the MIT license
#
# Copyright (c) 2020, 0xebef

# build type: release or debug
ifndef config
  config=release
endif

# paths
GCC_PATH                                   := /opt/msp430-gcc
COMPILER_PATH                              := $(GCC_PATH)/bin

# microcontroller model
MCU                                        := msp430fr6989

# device, same as microcontroller model but in uppercase
DEVICE                                     := MSP430FR6989

# global definitions
DEFINES                                    := -D__$(DEVICE)__ -DAUTHOR=0xebef

# toolchain
CC                                         := $(COMPILER_PATH)/msp430-elf-gcc
LD                                         := $(COMPILER_PATH)/msp430-elf-ld
AR                                         := $(COMPILER_PATH)/msp430-elf-ar
AS                                         := $(COMPILER_PATH)/msp430-elf-gcc
GASP                                       := $(COMPILER_PATH)/msp430-elf-gasp
GDB                                        := $(COMPILER_PATH)/msp430-elf-gdb
NM                                         := $(COMPILER_PATH)/msp430-elf-nm
OBJCOPY                                    := $(COMPILER_PATH)/msp430-elf-objcopy
OBJDUMP                                    := $(COMPILER_PATH)/msp430-elf-objdump
RANLIB                                     := $(COMPILER_PATH)/msp430-elf-ranlib
STRIP                                      := $(COMPILER_PATH)/msp430-elf-strip
SIZE                                       := $(COMPILER_PATH)/msp430-elf-size
READELF                                    := $(COMPILER_PATH)/msp430-elf-readelf
MAKETXT                                    := srec_cat
CP                                         := cp -p
RM                                         := rm -f
MV                                         := mv
TEST                                       := test
MSPDEBUG                                   := LD_LIBRARY_PATH=/opt/energia/hardware/tools/DSLite/DebugServer/drivers mspdebug tilib -d /dev/ttyACM0

# include directories
GCC_INC_DIR                                := $(GCC_PATH)/msp430-elf/include
HW_INC_DIR                                 := $(GCC_PATH)/include
INC_DIR                                    := inc
DRV_INC_DIR                                := lib/driverlib

# source directories
SRC_DIR                                    := src
DRV_SRC_DIR                                := lib/driverlib/MSP430FR5xx_6xx

# source files
SRC                                        := $(wildcard $(SRC_DIR)/*.c)
DRV_SRC                                    := $(wildcard $(DRV_SRC_DIR)/*.c)

# target
ifeq ($(config),release)
  BUILD_DIR=build/release
else
  BUILD_DIR=build/debug
endif
OBJ_DIR := $(BUILD_DIR)/obj
BIN_DIR := $(BUILD_DIR)/bin
TARGET  := $(BIN_DIR)/app
ELF     := $(TARGET).elf

# compiler flags
ifeq ($(config),release)
  OPTIMFLAGS  = -Os
else
  OPTIMFLAGS  = -Og -g
endif
DRV_FLAGS      := -Wno-missing-field-initializers -Wno-int-to-pointer-cast -Wno-parentheses
#MEMMODEL_FLAGS := -mlarge -muse-lower-region-prefix -mdata-region=lower -mcode-region=upper
#MEMMODEL_FLAGS := -mlarge -mdata-region=either -mcode-region=either
#MEMMODEL_FLAGS := -mlarge -mdata-region=upper -mcode-region=upper
MEMMODEL_FLAGS := -mlarge -mdata-region=upper -mcode-region=either
COMMON_FLAGS   := -MD -mmcu=$(MCU) $(DEFINES) $(OPTIMFLAGS) -mhwmult=f5series -mrelax $(MEMMODEL_FLAGS) \
                  -ffunction-sections -fdata-sections -fno-strict-aliasing \
                  -I $(GCC_INC_DIR) \
                  -I $(HW_INC_DIR) -L $(HW_INC_DIR) \
                  -I $(INC_DIR) \
                  -I $(DRV_INC_DIR)
CFLAGS         := $(COMMON_FLAGS) -c -Wall -Wextra -Werror -Wshadow -Wformat -Wformat-security -Wno-unused-function -Wno-unused-parameter -Wno-unused-but-set-variable -Wundef -std=gnu99
ASFLAGS        := $(COMMON_FLAGS) -x assembler-with-cpp -Wa,-gstabs
LDFLAGS        := $(COMMON_FLAGS) -T $(HW_INC_DIR)/$(MCU).ld -Wl,--gc-sections -Wl,--cref -Wl,-Map=$(TARGET).map
LIBS           := -l gcc -l c -l m

# attempt to create the output directories
ifneq ($(BUILD_DIR),)
  $(shell [ -d $(BUILD_DIR) ]              || mkdir -p $(BUILD_DIR))
  $(shell [ -d $(OBJ_DIR) ]                || mkdir -p $(OBJ_DIR))
  $(shell [ -d $(OBJ_DIR)/$(SRC_DIR) ]     || mkdir -p $(OBJ_DIR)/$(SRC_DIR))
  $(shell [ -d $(OBJ_DIR)/$(DRV_SRC_DIR) ] || mkdir -p $(OBJ_DIR)/$(DRV_SRC_DIR))
  $(shell [ -d $(BIN_DIR) ]                || mkdir -p $(BIN_DIR))
endif

# Object files
OBJS := $(patsubst %.c,$(OBJ_DIR)/$(SRC_DIR)/%.o,$(notdir $(SRC))) \
$(patsubst %.c,$(OBJ_DIR)/$(DRV_SRC_DIR)/%.o,$(notdir $(DRV_SRC))) \
        $(patsubst %.c,$(OBJ_DIR)/$(DRV_SRC_DIR)/%.o,$(notdir $(DRV_SRC)))

# Dependancies
DEPS := $(OBJS:.o=.d)

# Rules
.PHONY: all
all: $(ELF)

$(ELF) : $(OBJS)
	$(TEST) -f $@ && $(RM) $@ || true
	$(CC) $(LDFLAGS) $^ $(LIBS) -o $@
	$(TEST) -f $@ && $(SIZE) $@ && $(OBJDUMP) -h $@ || true
	$(TEST) -f $@.lst && $(RM) $@.lst || true
ifeq ($(config),debug)
	$(TEST) -f $@ && $(SIZE) $@ && $(OBJDUMP) -D -S $@ > $@.lst || true
endif

$(OBJ_DIR)/$(SRC_DIR)/%.o : $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

$(OBJ_DIR)/$(DRV_SRC_DIR)/%.o : $(DRV_SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(DRV_FLAGS) $< -o $@

.PHONY: clean
clean:
	$(RM) -r $(OBJ_DIR)
	$(RM) -r $(BIN_DIR)

.PHONY: flash
flash:
	$(MSPDEBUG) "erase" "load $(ELF)" "exit"

.PHONY: restart
restart:
	$(MSPDEBUG) "exit"

.PHONY: test
test:
	echo "test"

-include $(DEPS)
