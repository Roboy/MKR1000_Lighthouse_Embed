# Copyright (c) 2015 Atmel Corporation/Thibaut VIARD.  All right reserved.
# Copyright (c) 2015 Arduino LLC.  All right reserved.
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

# -----------------------------------------------------------------------------
# Paths
# -----------------------------------------------------------------------------

NAME=Jiggy
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	MODULE_PATH?=$(HOME)/mkr1000
	RM=rm
	SEP=/
	MKDIR= mkdir -p
endif

UPLOAD_BOSSA=$(MODULE_PATH)/tools/bossac
ARM_GCC_PATH?=$(MODULE_PATH)/tools/gcc-arm-none-eabi/bin/arm-none-eabi-
ARM_GCC_PATH?=/home/sebtut/.arduino15/packages/arduino/tools/arm-none-eabi-gcc/4.8.3-2014q1/bin/arm-none-eabi-
BUILD_PATH=$(MODULE_PATH)/build
UPLOAD_PORT_BASENAME=ttyACM0
UPLOAD_BOSSA=$(MODULE_PATH)/tools/bossac

ifdef DEBUG
OPTIMIZATION = -g -O0 -DDEBUG
OBJ_DIR=debug
else
	OPTIMIZATION = -Os
	OBJ_DIR=release
endif

# -----------------------------------------------------------------------------
# Toolchain
# -----------------------------------------------------------------------------
CC=$(ARM_GCC_PATH)gcc
LKELF=$(ARM_GCC_PATH)g++
OBJCOPY=$(ARM_GCC_PATH)objcopy
NM=$(ARM_GCC_PATH)nm
SIZE=$(ARM_GCC_PATH)size

# -----------------------------------------------------------------------------
# Compiler options / CFLAGS
# -----------------------------------------------------------------------------

#_old CFLAGS_EXTRA= -DF_CPU=48000000L -DARDUINO_SAMD_MKR1000 -DARDUINO_ARCH_SAMD -D__SAMD21G18A__ -DBOARD_ID_$(BOARD_ID)

LNK_SCRIPT = $(MODULE_PATH)/MKRLib/variants/mkr1000/linker_scripts/gcc/flash_with_bootloader.ld
CFLAGS_EXTRA= -DF_CPU=48000000L -DARDUINO=10612 -DARDUINO_SAMD_MKR1000 -DARDUINO_ARCH_SAMD  -D__SAMD21G18A__ -DUSB_VID=0x2341 -DUSB_PID=0x824e -DUSBCON '-DUSB_MANUFACTURER="Arduino LLC"' '-DUSB_PRODUCT="Genuino MKR1000"'
CFLAGS=-mthumb -mcpu=cortex-m0plus -Wall -Wextra -std=gnu++11 -c -fno-threadsafe-statics -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections -MMD -nostdlib -nostartfiles --param max-inline-insns-single=500 

ifdef DEBUG
  #CFLAGS+=-g3 -O1 -DDEBUG=1
  CFLAGS+=-g -Os -DDEBUG=1
else
  CFLAGS+=-Os -DDEBUG=0
endif

ELF=$(NAME).elf
BIN=$(NAME).bin
HEX=$(NAME).hex

INCLUDES=-I"$(MODULE_PATH)/CMSIS/CMSIS/Include/" -I"$(MODULE_PATH)/CMSIS/Device/ATMEL/" -I"$(MODULE_PATH)/MKRLib/libraries/SPI/" -I"$(MODULE_PATH)/MKRLib/variants/mkr1000" -I"$(MODULE_PATH)/include/"

# -----------------------------------------------------------------------------
# Linker options
#
#_old LDFLAGS= -mthumb -mcpu=cortex-m0plus -Wall -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all
#_old LDFLAGS+=-Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols --specs=nano.specs --specs=nosys.specs

LDFLAGS= --specs=nano.specs --specs=nosys.specs -mcpu=cortex-m0plus -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align

# -----------------------------------------------------------------------------
# Source files and objects
SOURCES= \
	main.cpp \
	variant.cpp

OBJECTS=$(addprefix $(BUILD_PATH)/, $(SOURCES:.cpp=.o))
DEPS=$(addprefix $(BUILD_PATH)/, $(SOURCES:.cpp=.o))

all: print_info $(SOURCES) $(BIN) $(HEX) $(AS_BUILD)

$(ELF): Makefile $(BUILD_PATH) $(OBJECTS)
	@echo ----------------------------------------------------------
	@echo Creating ELF binary
	"$(CC)" -Os -Wl,--gc-sections -save-temps "-T$(LNK_SCRIPT)" "-Wl,-Map,$(BUILD_PATH)/$(NAME).map" $(LDFLAGS) -o "$(BUILD_PATH)/$(ELF)" $(OBJECTS) -Wl,--start-group -lm "$(MODULE_PATH)/lib/libcore.a" -Wl,--end-group
	@echo ----------------------------------------------------------
	"$(NM)" "$(BUILD_PATH)/$(ELF)" >"$(BUILD_PATH)/$(NAME)_symbols.txt"
	@echo ----------------------------------------------------------
	"$(SIZE)" --format=sysv -t -x $(BUILD_PATH)/$(ELF)

$(BIN): $(ELF)
	@echo ----------------------------------------------------------
	@echo Creating Flash binary
	"$(OBJCOPY)" -O binary $(BUILD_PATH)/$< $(BUILD_PATH)/$@

$(HEX): $(ELF)
	@echo ----------------------------------------------------------
	@echo Creating Flash binary
	"$(OBJCOPY)" -O ihex $(BUILD_PATH)/$< $(BUILD_PATH)/$@

$(BUILD_PATH)/%.o: %.cpp
	@echo ----------------------------------------------------------
	@echo Compiling $< to $@
	"$(LKELF)" $(CFLAGS) $(CFLAGS_EXTRA) $(INCLUDES) $< -o $@
	@echo 
	
$(BUILD_PATH):
	@echo ----------------------------------------------------------
	@echo Creating build folder
	-mkdir $(BUILD_PATH)

print_info:
	@echo ----------------------------------------------------------
	@echo Compiling Application using
	@echo BASE PATH = $(MODULE_PATH)
	@echo GCC  PATH = $(ARM_GCC_PATH)

clean: $(AS_CLEAN)
	@echo ----------------------------------------------------------
	@echo Cleaning project
	-rm -r $(BUILD_PATH)

.phony: print_info $(BUILD_PATH)

install: 
	-@echo "Touch programming port ..."
	-@stty -F "/dev/$(UPLOAD_PORT_BASENAME)" raw ispeed 1200 ospeed 1200 cs8 -cstopb ignpar eol 255 eof 255
	-@printf "\x00" > "/dev/$(UPLOAD_PORT_BASENAME)"
	-@echo "Waiting before uploading ..."
	-@sleep 1
	-@echo "Uploading ..."
	$(UPLOAD_BOSSA) -i -d --port="$(UPLOAD_PORT_BASENAME)" -U true -e -w -v -b "$(BUILD_PATH)/$(NAME).bin" -R
	#   $(UPLOAD_BOSSA) $(UPLOAD_VERBOSE_FLAGS) --port="$(UPLOAD_PORT_BASENAME)" -U true -e -w -b "$(BUILD_PATH)/$(NAME).bin" -R
	#   @echo "Done."
