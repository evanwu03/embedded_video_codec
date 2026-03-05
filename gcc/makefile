SIMPLELINK_MSP432_SDK_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/imports.mak


NAME = sprite_renderer
TARGET = $(NAME).out

BUILD_DIR   := build

SRC_DIRS    := ../src ../gcc .. ../LcdDriver ../hal/msp432

INC_DIRS    := ../include ../LcdDriver ../hal/include

# ============================================================
# Toolchain
# ============================================================
CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

LNK = $(CC)


# ============================================================
# Flags
# ============================================================
CFLAGS = -I.. \
	-I/include \
	-I../src/ \
	"-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/" \
	"-I${SIMPLELINK_MSP432_SDK_INSTALL_DIR}/source/ti/devices/msp432p4xx/inc" \
	"-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/third_party/CMSIS/Include" \
	-I/LcdDriver   \
	-D__MSP432P401R__ \
	-DDeviceFamily_MSP432P401x \
	-mcpu=cortex-m4 \
	-march=armv7e-m \
	-mthumb \
	-std=c99 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-ffunction-sections \
	-fdata-sections \
	-g \
	-gstrict-dwarf \
	-Wall \
	"-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
	"-I$(GCC_ARMCOMPILER)/arm-none-eabi/include" \
	-O1 \

LFLAGS = -Wl,-T,../gcc/msp432p401r.lds \
	"-Wl,-Map,$(NAME).map" \
	"-L$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source" \
	-l:ti/display/lib/display.am4fg \
	-l:ti/grlib/lib/gcc/m4f/grlib.a \
	-l:third_party/spiffs/lib/gcc/m4f/spiffs.a \
	-l:ti/drivers/lib/drivers_msp432p401x.am4fg \
	-l:third_party/fatfs/lib/gcc/m4f/fatfs.a \
	-l:ti/devices/msp432p4xx/driverlib/gcc/msp432p4xx_driverlib.a \
	-march=armv7e-m \
	-mthumb \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-static \
	-Wl,--gc-sections \
	"-L$(GCC_ARMCOMPILER)/arm-none-eabi/lib/thumb/v7e-m/fpv4-sp/hard" \
	-lgcc \
	-lc \
	-lm \
	-lnosys \
	--specs=nano.specs


# ============================================================
# Source and Object Collection
# ============================================================
SRC := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJ := $(patsubst ../%.c,$(BUILD_DIR)/%.o,$(SRC))
OBJ := $(patsubst ./%.c,$(BUILD_DIR)/gcc/%.o,$(OBJ))

all: $(BUILD_DIR)/$(TARGET)


$(BUILD_DIR)/$(TARGET): $(OBJ)
	@echo "Linking: $@\n"
	$(LNK) $(CFLAGS) $(OBJ) -o $@ $(LFLAGS)

$(BUILD_DIR)/%.o: ../%.c
	@mkdir -p $(dir $@)
	@echo "Building: $<\n"
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/gcc/%.o: ./%.c
	@mkdir -p $(dir $@)
	@echo "Building (GCC): $<\n"
	$(CC) $(CFLAGS) -c $< -o $@



# ------------------------------------------------------------------
# Utility targets
# ------------------------------------------------------------------
clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)

flash:
	openocd -f board/ti_msp432_launchpad.cfg -c "program $(BUILD_DIR)/$(TARGET) verify reset exit"
