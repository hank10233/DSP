###############################################################################
# Makefile for the project 005_master_test
###############################################################################

## General Flags
PROJECT = 005_master_test
MCU = atmega128
TARGET = 005_master_test.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -DF_CPU=11059200UL -Os -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=005_master_test.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = 005_master_test.o ASA_TWI.o ASA_DIO.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = "C:\Users\hank10233\Documents\AVRStudio\sp4\LLL\ASA_Lib.o" 

## Build
all: $(TARGET) 005_master_test.hex 005_master_test.eep 005_master_test.lss size

## Compile
005_master_test.o: ../005_master_test.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ASA_TWI.o: ../../LLL/ASA_TWI.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

ASA_DIO.o: ../../LLL/ASA_DIO.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) 005_master_test.elf dep/* 005_master_test.hex 005_master_test.eep 005_master_test.lss 005_master_test.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

