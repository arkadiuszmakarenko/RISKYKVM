# Unified Makefile for RISKYKVM Projects
# Supports three build variants:
#   - Bootloader: Custom bootloader (FLASH @ 0x00000000, 32K, src in Bootloader/)
#   - RISKYKVM: Standalone application (FLASH @ 0x00000000, 64K, src in src/)
#   - RISKYKVMBootloader: Application for bootloader (FLASH @ 0x00008000, 32K, src in src/)
# Main applications share source code from src/ directory

#############################
# Project configuration
#############################

# Project variant: Bootloader, RISKYKVM, or RISKYKVMBootloader
# Override on CLI: make PROJECT=RISKYKVMBootloader
PROJECT ?= RISKYKVM

# Build type: Release (size-optimized) or Debug (debuggable)
# Override on CLI: make BUILD_TYPE=Debug
BUILD_TYPE ?= Release

# Validate PROJECT selection
ifeq ($(filter $(PROJECT),Bootloader RISKYKVM RISKYKVMBootloader),)
    $(error Invalid PROJECT: $(PROJECT). Must be Bootloader, RISKYKVM, or RISKYKVMBootloader)
endif

# Build directory (separate per project and build type)
BUILD_DIR = build/$(PROJECT)/$(BUILD_TYPE)

#############################
# Source and include configuration
#############################

# Source directory selection and linker script
ifeq ($(PROJECT),Bootloader)
    # Bootloader uses shared sources from src/ plus its own User/ files
    SHARED_C_DIRS = Core Debug Peripheral/src Udisk_Lib
    LOCAL_C_DIRS = User
    
	SHARED_C_SOURCES = $(shell find $(addprefix src/,$(SHARED_C_DIRS)) -name "*.c" 2>/dev/null)
	LOCAL_C_SOURCES = $(shell find $(addprefix Bootloader/,$(LOCAL_C_DIRS)) -name "*.c" 2>/dev/null)
    C_SOURCES = $(SHARED_C_SOURCES) $(LOCAL_C_SOURCES)
    
    # Bootloader has its own Startup
    ASM_SOURCES = Bootloader/Startup/startup_ch32v20x_D6.S
    
    # Include directories for Bootloader
    INC_DIRS = -Isrc/Debug \
               -Isrc/Core \
			   -IBootloader/User \
               -Isrc/Peripheral/inc \
               -IBootloader/User/Host_IAP \
               -IBootloader/User/USB_Host \
               -Isrc/Udisk_Lib
    
    # Linker script
    LINKER_SCRIPT = Bootloader/Ld/Link.ld

	# Bootloader links CHRV3 U-disk static library
	LIB_DIRS = -LBootloader \
			   -Lsrc/Udisk_Lib
	LIBS = -lRV3UFI
    
    # Startup directory for assembler flags
    STARTUP_DIR = Bootloader/Startup
else
    # RISKYKVM and RISKYKVMBootloader both use src/ directory
    C_DIRS = Core Debug Peripheral/src User
	C_SOURCES = $(shell find $(addprefix src/,$(C_DIRS)) -name "*.c" 2>/dev/null)
    
    # Shared Startup
    ASM_SOURCES = src/Startup/startup_ch32v20x_D6.S
    
    # Include directories
    INC_DIRS = -Isrc/Debug \
               -Isrc/Core \
               -Isrc/User \
               -Isrc/Peripheral/inc \
               -Isrc/User/USB_Host
    
    # Linker script selection
    ifeq ($(PROJECT),RISKYKVMBootloader)
        LINKER_SCRIPT = RISKYKVMBootloader/Ld/Link.ld
    else
        LINKER_SCRIPT = RISKYKVM/Ld/Link.ld
    endif
    
    # Startup directory for assembler flags
    STARTUP_DIR = src/Startup

	# No extra libraries for app variants
	LIB_DIRS =
	LIBS =
endif

#############################
# Toolchain Configuration
#############################

# Can be overridden via environment or command line for containerized builds
MRS_TOOLCHAIN_ROOT ?= ../MRS_Toolchain_Linux_x64_V210
TOOLCHAIN_PREFIX ?= riscv-wch-elf

# Toolchain paths - can specify full path to toolchain bin directory
# For containers, set TOOLCHAIN_PATH to the bin directory, e.g.:
#   make TOOLCHAIN_PATH=/opt/riscv/bin/riscv-wch-elf
ifneq ($(TOOLCHAIN_PATH),)
    # User provided full toolchain path
    CC := $(TOOLCHAIN_PATH)-gcc
    OBJCOPY := $(TOOLCHAIN_PATH)-objcopy
    OBJDUMP := $(TOOLCHAIN_PATH)-objdump
    SIZE := $(TOOLCHAIN_PATH)-size
    GDB := $(TOOLCHAIN_PATH)-gdb
    READELF := $(TOOLCHAIN_PATH)-readelf
else
    # Use MRS toolchain root
	TOOLCHAIN_BIN_DIR := $(MRS_TOOLCHAIN_ROOT)/RISC-V Embedded GCC12/bin
    TOOLCHAIN_PATH := $(TOOLCHAIN_BIN_DIR)/$(TOOLCHAIN_PREFIX)
    CC := $(TOOLCHAIN_PATH)-gcc
    OBJCOPY := $(TOOLCHAIN_PATH)-objcopy
    OBJDUMP := $(TOOLCHAIN_PATH)-objdump
    SIZE := $(TOOLCHAIN_PATH)-size
    GDB := $(TOOLCHAIN_PATH)-gdb
    READELF := $(TOOLCHAIN_PATH)-readelf
endif

#############################
# Compiler flags
#############################

# Base compiler flags
CFLAGS = -march=rv32imacxw \
         -mabi=ilp32 \
         -msmall-data-limit=8 \
         -msave-restore \
         -fmax-errors=20 \
         -fmessage-length=0 \
         -fsigned-char \
         -ffunction-sections \
         -fdata-sections \
         -fno-common \
         -Wunused \
         -Wuninitialized \
         -std=gnu99 \
         -MMD \
         -MP

# Assembler flags
ASFLAGS = -march=rv32imacxw \
          -mabi=ilp32 \
          -msmall-data-limit=8 \
          -msave-restore \
          -fmax-errors=20 \
          -fmessage-length=0 \
          -fsigned-char \
          -ffunction-sections \
          -fdata-sections \
          -fno-common \
          -Wunused \
          -Wuninitialized \
          -x assembler-with-cpp \
          -I$(STARTUP_DIR) \
          -MMD \
          -MP

# Base linker flags
LDFLAGS = -march=rv32imacxw \
          -mabi=ilp32 \
          -msmall-data-limit=8 \
          -msave-restore \
          -fmax-errors=20 \
          -fmessage-length=0 \
          -fsigned-char \
          -ffunction-sections \
          -fdata-sections \
          -fno-common \
          -Wunused \
          -Wuninitialized \
          -T$(LINKER_SCRIPT) \
          -nostartfiles \
          -Xlinker --gc-sections \
          -Wl,-Map,$(BUILD_DIR)/$(PROJECT).map \
		  -Wl,-u,_sbrk \
          --specs=nano.specs \
          --specs=nosys.specs

#############################
# Optimization and debug per build type
#############################

ifeq ($(BUILD_TYPE),Debug)
	# Maximum debugging experience - all debug flags enabled
	CFLAGS += -Og -g3 -ggdb3 -gdwarf-4 \
	          -fno-omit-frame-pointer \
	          -fvar-tracking \
	          -fvar-tracking-assignments \
	          -fno-eliminate-unused-debug-types \
	          -fno-inline \
	          -fno-inline-small-functions \
	          -fno-inline-functions-called-once \
	          -fdebug-prefix-map=$(shell pwd)=. \
	          -grecord-gcc-switches
	ASFLAGS += -g3 -ggdb3 -gdwarf-4
	LDFLAGS += -g3 -ggdb3
else
	# Size-optimized release - match MRS toolchain settings
	CFLAGS += -Os -flto --param=highcode-gen-section-name=1
	ASFLAGS += -Os
	LDFLAGS += -Os -flto
endif

#############################
# Object and dependency files
#############################

# For Bootloader: separate object paths for shared and local sources
ifeq ($(PROJECT),Bootloader)
    SHARED_C_OBJECTS = $(patsubst src/%.c,$(BUILD_DIR)/src/%.o,$(SHARED_C_SOURCES))
    LOCAL_C_OBJECTS = $(patsubst Bootloader/%.c,$(BUILD_DIR)/Bootloader/%.o,$(LOCAL_C_SOURCES))
    C_OBJECTS = $(SHARED_C_OBJECTS) $(LOCAL_C_OBJECTS)
    ASM_OBJECTS = $(patsubst Bootloader/%.S,$(BUILD_DIR)/Bootloader/%.o,$(ASM_SOURCES))
    
    SHARED_C_DEPS = $(patsubst src/%.c,$(BUILD_DIR)/src/%.d,$(SHARED_C_SOURCES))
    LOCAL_C_DEPS = $(patsubst Bootloader/%.c,$(BUILD_DIR)/Bootloader/%.d,$(LOCAL_C_SOURCES))
    C_DEPS = $(SHARED_C_DEPS) $(LOCAL_C_DEPS)
    ASM_DEPS = $(patsubst Bootloader/%.S,$(BUILD_DIR)/Bootloader/%.d,$(ASM_SOURCES))
else
    # For RISKYKVM and RISKYKVMBootloader: all objects go to BUILD_DIR
    C_OBJECTS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
    ASM_OBJECTS = $(patsubst src/%.S,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
    
    C_DEPS = $(patsubst src/%.c,$(BUILD_DIR)/%.d,$(C_SOURCES))
    ASM_DEPS = $(patsubst src/%.S,$(BUILD_DIR)/%.d,$(ASM_SOURCES))
endif

ALL_OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)
ALL_DEPS = $(C_DEPS) $(ASM_DEPS)

#############################
# Build targets
#############################

# Default target - build current project in Release mode
all: all-release

# Build single project (used internally)
build-single: check-toolchain $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).hex $(BUILD_DIR)/$(PROJECT).bin $(BUILD_DIR)/$(PROJECT).lst size
	@$(MAKE) post-build

# Check if toolchain is available
check-toolchain:
	@echo "==============================="
	@echo "Building: $(PROJECT)"
	@echo "Build Type: $(BUILD_TYPE)"
	@echo "Linker Script: $(LINKER_SCRIPT)"
	@echo "==============================="
	@echo "Checking toolchain..."
	@if [ ! -f "$(CC)" ]; then \
		echo "Error: Compiler not found at $(CC)"; \
		echo "Please check your toolchain configuration or run 'make toolchain-info'"; \
		echo "You can also set TOOLCHAIN_PATH=/path/to/bin/prefix for containers"; \
		exit 1; \
	fi
	@echo "✓ Using toolchain: $(CC)"

# Create build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

#############################
# Compilation rules
#############################

# Rule for shared C sources (Bootloader only)
ifeq ($(PROJECT),Bootloader)
$(BUILD_DIR)/src/%.o: src/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@"$(CC)" $(CFLAGS) $(INC_DIRS) -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
endif

# Rule for Bootloader-local C sources (Bootloader only)
ifeq ($(PROJECT),Bootloader)
$(BUILD_DIR)/Bootloader/%.o: Bootloader/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@"$(CC)" $(CFLAGS) $(INC_DIRS) -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
endif

# Rule for src C sources (RISKYKVM and RISKYKVMBootloader)
ifneq ($(PROJECT),Bootloader)
$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "Compiling $<"
	@"$(CC)" $(CFLAGS) $(INC_DIRS) -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
endif

# Rule for Bootloader assembly sources
ifeq ($(PROJECT),Bootloader)
$(BUILD_DIR)/Bootloader/%.o: Bootloader/%.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "Assembling $<"
	@"$(CC)" $(ASFLAGS) -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
endif

# Rule for src assembly sources
ifneq ($(PROJECT),Bootloader)
$(BUILD_DIR)/%.o: src/%.S | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	@echo "Assembling $<"
	@"$(CC)" $(ASFLAGS) -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
endif

#############################
# Linking and binary generation
#############################

# Rule for linking the ELF file
$(BUILD_DIR)/$(PROJECT).elf: $(ALL_OBJECTS) | $(BUILD_DIR)
	@echo "Linking $@"
	@"$(CC)" $(LDFLAGS) $(LIB_DIRS) -o "$@" $(ALL_OBJECTS) $(LIBS)

# Rule for creating hex file
$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Creating $@"
	@"$(OBJCOPY)" -O ihex "$<" "$@"

# Rule for creating binary file
$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Creating $@"
	@"$(OBJCOPY)" -O binary "$<" "$@"

# Rule for creating listing file
$(BUILD_DIR)/$(PROJECT).lst: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Creating $@"
	@"$(OBJDUMP)" --all-headers --demangle --disassemble -M xw "$<" > "$@"

#############################
# Post-build steps
#############################

post-build:
ifeq ($(PROJECT),RISKYKVMBootloader)
	@echo "Creating RISKYKVM.UPD for bootloader update..."
	@cp "$(BUILD_DIR)/$(PROJECT).bin" "$(BUILD_DIR)/RISKYKVM.UPD"
	@cp "$(BUILD_DIR)/$(PROJECT).bin" "build/RISKYKVM.UPD"
	@echo "✓ Created: $(BUILD_DIR)/RISKYKVM.UPD"
	@echo "✓ Created: build/RISKYKVM.UPD"
endif

#############################
# Combined binary generation
#############################

# Combine bootloader and application into a single release binary
# This target will build the required binaries if they don't exist
# Accepts COMBINE_BUILD_TYPE parameter (default: Release)
COMBINE_BUILD_TYPE ?= Release

combine-release:
	@echo "==============================="
	@echo "Creating combined release binary ($(COMBINE_BUILD_TYPE))"
	@echo "==============================="
	@if [ ! -f "build/Bootloader/$(COMBINE_BUILD_TYPE)/Bootloader.bin" ]; then \
		echo "Building Bootloader ($(COMBINE_BUILD_TYPE))..."; \
		$(MAKE) build-single PROJECT=Bootloader BUILD_TYPE=$(COMBINE_BUILD_TYPE); \
	fi
	@if [ ! -f "build/RISKYKVMBootloader/$(COMBINE_BUILD_TYPE)/RISKYKVMBootloader.bin" ]; then \
		echo "Building RISKYKVMBootloader ($(COMBINE_BUILD_TYPE))..."; \
		$(MAKE) build-single PROJECT=RISKYKVMBootloader BUILD_TYPE=$(COMBINE_BUILD_TYPE); \
	fi
	@echo ""
	@echo "Padding bootloader to 32KB (0x8000 bytes)..."
	@dd if=build/Bootloader/$(COMBINE_BUILD_TYPE)/Bootloader.bin of=build/Bootloader_padded.bin bs=1 count=32768 conv=sync 2>/dev/null
	@truncate -s 32768 build/Bootloader_padded.bin
	@echo "Padding RISKYKVMBootloader to 32KB (0x8000 bytes)..."
	@dd if=build/RISKYKVMBootloader/$(COMBINE_BUILD_TYPE)/RISKYKVMBootloader.bin of=build/App_padded.bin bs=1 count=32768 conv=sync 2>/dev/null
	@truncate -s 32768 build/App_padded.bin
	@echo "Combining binaries..."
	@cat build/Bootloader_padded.bin build/App_padded.bin > build/RISKYKVMRelease.bin
	@rm -f build/Bootloader_padded.bin build/App_padded.bin
	@echo "✓ Created: build/RISKYKVMRelease.bin"
	@ls -lh build/RISKYKVMRelease.bin
	@echo ""
	@echo "Flash layout:"
	@echo "  0x00000000 - 0x00007FFF : Bootloader (32KB)"
	@echo "  0x00008000 - 0x0000FFFF : RISKYKVMBootloader (32KB)"
	@echo "  Total: 64KB"

#############################
# Information targets
#############################

# Show size information
size: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Size information:"
	@"$(SIZE)" --format=berkeley "$<"

# Show detailed memory usage
memory-usage: $(BUILD_DIR)/$(PROJECT).elf
	@echo "Detailed memory usage:"
	@"$(SIZE)" --format=sysv "$<"
	@echo ""
	@echo "Section sizes:"
	@"$(READELF)" -S "$<" | grep -E '(\.text|\.data|\.bss|\.rodata)' || true

#############################
# Device programming targets
#############################

# Flash combined release binary (bootloader + application)
flash: build/RISKYKVMRelease.bin
	@echo "Flashing RISKYKVMRelease.bin (64KB: Bootloader + Application) to device..."
	@if [ ! -f "$<" ]; then \
		echo "Combined binary not found. Building..."; \
		$(MAKE) all-release; \
	fi
	@if [ -f "$(MRS_TOOLCHAIN_ROOT)/OpenOCD/bin/openocd" ]; then \
		echo "Using MRS OpenOCD with sudo (required for USB access)..."; \
		sudo $(MRS_TOOLCHAIN_ROOT)/OpenOCD/bin/openocd \
			-f $(MRS_TOOLCHAIN_ROOT)/OpenOCD/bin/wch-riscv.cfg \
			-c "program $(shell pwd)/$< verify reset exit 0x00000000"; \
	else \
		echo "MRS OpenOCD not found. Please configure your programming tool."; \
		echo "You can flash manually using: $<"; \
	fi

# Flash bootloader only
flash-bootloader: build/Bootloader/Release/Bootloader.bin
	@echo "Flashing Bootloader only (32KB) to device at 0x00000000..."
	@if [ ! -f "$<" ]; then \
		echo "Bootloader binary not found. Building..."; \
		$(MAKE) build-single PROJECT=Bootloader BUILD_TYPE=Release; \
	fi
	@if [ -f "$(MRS_TOOLCHAIN_ROOT)/OpenOCD/bin/openocd" ]; then \
		echo "Using MRS OpenOCD with sudo (required for USB access)..."; \
		sudo $(MRS_TOOLCHAIN_ROOT)/OpenOCD/bin/openocd \
			-f $(MRS_TOOLCHAIN_ROOT)/OpenOCD/bin/wch-riscv.cfg \
			-c "program $(shell pwd)/$< verify reset exit 0x00000000"; \
	else \
		echo "MRS OpenOCD not found. Please configure your programming tool."; \
		echo "You can flash manually using: $<"; \
	fi

# Erase entire flash chip
erase:
	@echo "Erasing entire flash chip..."
	@sudo $(shell pwd)/$(MRS_TOOLCHAIN_ROOT)/OpenOCD/OpenOCD/bin/openocd \
		-f $(shell pwd)/$(MRS_TOOLCHAIN_ROOT)/OpenOCD/OpenOCD/bin/wch-riscv.cfg \
		-c "init" \
		-c "halt" \
		-c "flash erase_sector 0 0 last" \
		-c "reset" \
		-c "shutdown"

# Debug target
debug: $(BUILD_DIR)/$(PROJECT).elf
	@"$(GDB)" "$<"

#############################
# Build all targets
#############################

# Build all three variants in Release mode
all-release:
	@echo "Building all project variants in Release mode..."
	@$(MAKE) build-single PROJECT=Bootloader BUILD_TYPE=Release
	@$(MAKE) build-single PROJECT=RISKYKVM BUILD_TYPE=Release
	@$(MAKE) build-single PROJECT=RISKYKVMBootloader BUILD_TYPE=Release
	@$(MAKE) combine-release COMBINE_BUILD_TYPE=Release

# Build all three variants in Debug mode
all-debug:
	@echo "Building all project variants in Debug mode..."
	@$(MAKE) build-single PROJECT=Bootloader BUILD_TYPE=Debug
	@$(MAKE) build-single PROJECT=RISKYKVM BUILD_TYPE=Debug
	@$(MAKE) build-single PROJECT=RISKYKVMBootloader BUILD_TYPE=Debug
	@$(MAKE) combine-release COMBINE_BUILD_TYPE=Debug

#############################
# Cleanup targets
#############################

# Clean all build artifacts
clean:
	@echo "Cleaning all build artifacts..."
	@rm -rf build
	@echo "✓ Cleaned everything"

# Alias for clean (backward compatibility)
clean-all: clean

#############################
# Toolchain information
#############################

toolchain-info:
	@echo "Toolchain Configuration:"
	@echo "========================"
	@echo "MRS_TOOLCHAIN_ROOT: $(MRS_TOOLCHAIN_ROOT)"
	@echo "TOOLCHAIN_PREFIX:  $(TOOLCHAIN_PREFIX)"
	@echo "TOOLCHAIN_BIN_DIR: $(TOOLCHAIN_BIN_DIR)"
	@echo ""
	@echo "Active Tools:"
	@echo "CC:      $(CC)"
	@echo "OBJCOPY: $(OBJCOPY)"
	@echo "SIZE:    $(SIZE)"
	@echo "GDB:     $(GDB)"
	@echo ""
	@echo "Checking toolchain availability..."
	@if [ -f "$(CC)" ]; then \
		echo "✓ Compiler found: $(CC)"; \
		"$(CC)" --version | head -1; \
	else \
		echo "✗ Compiler not found: $(CC)"; \
		echo "  Please check your toolchain configuration"; \
	fi

#############################
# Help target
#############################

help:
	@echo "RISKYKVM Unified Makefile"
	@echo "=============================="
	@echo ""
	@echo "Available targets:"
	@echo "  all              - Build all three variants in Release mode (default)"
	@echo "  all-release      - Build all three variants in Release mode"
	@echo "  all-debug        - Build all three variants in Debug mode"
	@echo "  build-single     - Build single selected project (use PROJECT= and BUILD_TYPE=)"
	@echo "  combine-release  - Combine Bootloader + RISKYKVMBootloader binaries"
	@echo "  flash            - Flash combined release binary to device"
	@echo "  flash-bootloader - Flash bootloader only to device"
	@echo "  erase            - Erase entire flash chip"
	@echo "  clean            - Remove all build artifacts"
	@echo "  size             - Show size information for the ELF file"
	@echo "  memory-usage     - Show detailed memory usage breakdown"
	@echo "  debug            - Start GDB debug session"
	@echo "  toolchain-info   - Show current toolchain configuration"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Project Selection:"
	@echo "  PROJECT=Bootloader              - Custom bootloader (FLASH @ 0x00000000, 32K)"
	@echo "  PROJECT=RISKYKVM                - Standalone application (FLASH @ 0x00000000, 64K)"
	@echo "  PROJECT=RISKYKVMBootloader      - Bootloader application (FLASH @ 0x00008000, 32K)"
	@echo ""
	@echo "Build Type Selection:"
	@echo "  BUILD_TYPE=Release  - Size-optimized build (default)"
	@echo "  BUILD_TYPE=Debug    - Debug build with full symbols"
	@echo ""
	@echo "Toolchain Configuration:"
	@echo "  MRS_TOOLCHAIN_ROOT=/path/to/toolchain  - Override toolchain root directory"
	@echo "  TOOLCHAIN_PATH=/path/to/bin/prefix     - Override full toolchain path (for containers)"
	@echo "  TOOLCHAIN_PREFIX=prefix                - Override toolchain prefix (default: riscv-wch-elf)"
	@echo ""
	@echo "Examples:"
	@echo "  make                                             # Build all three variants in Release"
	@echo "  make PROJECT=Bootloader BUILD_TYPE=Debug         # Build bootloader Debug"
	@echo "  make all-debug                                   # Build all three variants in Debug"
	@echo "  make -j8                                         # Build with parallel compilation"
	@echo "  make flash                                       # Flash combined binary to device"
	@echo "  make clean                                       # Clean all builds"
	@echo ""
	@echo "Container/Custom Toolchain Examples:"
	@echo "  make TOOLCHAIN_PATH=/opt/riscv/bin/riscv-wch-elf"
	@echo "  make MRS_TOOLCHAIN_ROOT=/toolchain"
	@echo ""
	@echo "Build artifacts location: build/<PROJECT>/<BUILD_TYPE>/"

#############################
# Phony targets
#############################

.PHONY: all all-release all-debug build-single combine-release clean clean-all size memory-usage \
        flash flash-bootloader erase debug check-toolchain toolchain-info help post-build

# Include dependency files if they exist
-include $(ALL_DEPS)
