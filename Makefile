MCU=atmega328p
CFLAGS ?= -Os -DF_CPU=16000000UL -mmcu=$(MCU)
LDFLAGS ?= -mmcu=$(MCU)
ARDUINO_USB ?= /dev/ttyACM0

TARGET=rng.hex
TARGET_BIN=$(patsubst %.hex,%.elf,$(TARGET))

BUILD_DIR=build
SRC_DIR=src

all: $(BUILD_DIR)/$(TARGET)

SRC=$(shell find -L $(SRC_DIR) -name '*.c')
OBJS=$(addprefix $(BUILD_DIR)/, $(patsubst %.c, %.o, $(SRC)))

$(BUILD_DIR)/$(TARGET): $(BUILD_DIR)/$(TARGET_BIN)
	avr-objcopy -O ihex -R .eeprom $< $@

$(BUILD_DIR)/$(TARGET_BIN): $(OBJS)
	avr-gcc $(LDFLAGS) -o $@ $(OBJS) -ffunction-sections -Wl,--gc-sections
	#avr-strip -s $@
	avr-size --mcu=$(MCU) -A --totals $@

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(addprefix $(BUILD_DIR)/, $(dir $^))
	avr-gcc $(CFLAGS) -c -o $@ $<

prog: $(BUILD_DIR)/$(TARGET)
	avrdude -F -V -c arduino -p ATMEGA328p -P ${ARDUINO_USB} -b 115200 -U flash:w:$(BUILD_DIR)/$(TARGET)

avrterm:
	avrdude -F -V -c avrisp -p ATMEGA328p -P ${ARDUINO_USB} -b 19200 -t -u

clean: 
	rm -rf build

