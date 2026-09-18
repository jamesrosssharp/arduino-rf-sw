MCU=atmega328pb
CFLAGS ?= -Os -DF_CPU=128000UL -mmcu=$(MCU)
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

RNG_SEED_LD=$(SRC_DIR)/rng_seed.ld

$(BUILD_DIR)/$(TARGET_BIN): $(OBJS) $(RNG_SEED_LD)
	avr-gcc $(LDFLAGS) -Wl,-T,$(RNG_SEED_LD) -o $@ $(OBJS) -ffunction-sections -Wl,--gc-sections
	#avr-strip -s $@
	avr-size --mcu=$(MCU) -A --totals $@

$(BUILD_DIR)/%.o: %.c Makefile
	@mkdir -p $(addprefix $(BUILD_DIR)/, $(dir $^))
	avr-gcc $(CFLAGS) -c -o $@ $<

prog: $(BUILD_DIR)/$(TARGET)
	avrdude -F -p m328pb -c atmelice_isp -U flash:w:$(BUILD_DIR)/$(TARGET)

prog128: $(BUILD_DIR)/$(TARGET)
	avrdude -F -p m328pb -c atmelice_isp -B 16kHz -U flash:w:$(BUILD_DIR)/$(TARGET) -U lfuse:w:0xe3:m -U hfuse:w:0xd9:m -U efuse:w:0xf8:m



avrterm:
	avrdude -F -V -c avrisp -p ATMEGA328p -P ${ARDUINO_USB} -b 19200 -t -u

clean: 
	rm -rf build

