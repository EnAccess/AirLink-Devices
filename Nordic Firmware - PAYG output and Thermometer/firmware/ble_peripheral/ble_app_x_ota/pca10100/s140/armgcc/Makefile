PROJECT_NAME     := ble_app_buttonless_dfu_pca10100_s140
TARGETS          := nrf52833_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := ../../../../../..
PROJ_DIR := ../../..
AIRLINK_DIR := ../../../..

$(OUTPUT_DIRECTORY)/nrf52833_xxaa.out: \
  LINKER_SCRIPT  := ble_app_buttonless_dfu_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52833.S \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52833.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu_bonded.c \
  $(SDK_ROOT)/components/ble/ble_services/ble_dfu/ble_dfu_unbonded.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
  $(SDK_ROOT)/components/ble/peer_manager/auth_status_tracker.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager_handler.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/components/drivers_ext/ili9341/ili9341.c \
  $(SDK_ROOT)/components/drivers_ext/st7735/st7735.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/components/libraries/bootloader/dfu/nrf_dfu_svci.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp_btn_ble.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/crc16/crc16.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/components/libraries/fifo/app_fifo.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
  $(SDK_ROOT)/components/libraries/gfx/nrf_gfx.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer2.c \
  $(SDK_ROOT)/components/libraries/timer/drv_rtc.c \
  $(SDK_ROOT)/components/libraries/uart/app_uart_fifo.c \
  $(SDK_ROOT)/components/libraries/uart/retarget.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/external/nexus/examples/BLE/src/clock.c \
  $(SDK_ROOT)/external/nexus/examples/BLE/src/feedback.c \
  $(SDK_ROOT)/external/nexus/examples/BLE/src/identity.c \
  $(SDK_ROOT)/external/nexus/examples/BLE/src/keycode.c \
  $(SDK_ROOT)/external/nexus/examples/BLE/src/payg_state.c \
  $(SDK_ROOT)/external/nexus/examples/BLE/src/processing.c \
  $(SDK_ROOT)/external/nexus/examples/BLE/src/screen.c \
  $(SDK_ROOT)/external/nexus/src/nexus_common_internal.c \
  $(SDK_ROOT)/external/nexus/src/nexus_keycode_core.c \
  $(SDK_ROOT)/external/nexus/src/nexus_keycode_mas.c \
  $(SDK_ROOT)/external/nexus/src/nexus_keycode_pro.c \
  $(SDK_ROOT)/external/nexus/src/nexus_keycode_pro_extended.c \
  $(SDK_ROOT)/external/nexus/src/nexus_nv.c \
  $(SDK_ROOT)/external/nexus/src/nexus_security.c \
  $(SDK_ROOT)/external/nexus/src/nexus_util.c \
  $(SDK_ROOT)/external/nexus/utils/crc_ccitt.c \
  $(SDK_ROOT)/external/nexus/utils/siphash_24.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_KEIL.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/external/thedotfactory_fonts/calibri.c \
  $(SDK_ROOT)/external/thedotfactory_fonts/orkney8pts.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_spi.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rtc.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spi.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_spim.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \

# Source files common to all targets from SSL
SRC_FILES += \
  $(AIRLINK_DIR)/Sources/ble_device_config_service.c \
  $(AIRLINK_DIR)/Sources/ble_flash_fstorage_definitions.c \
  $(AIRLINK_DIR)/Sources/ble_gpio_config.c \
  $(AIRLINK_DIR)/Sources/ble_pue_use_update_service.c \
  $(AIRLINK_DIR)/Sources/ble_pwr_mgmt_definitions.c \
  $(AIRLINK_DIR)/Sources/ble_resource_definitions.c \
  $(AIRLINK_DIR)/Sources/ble_timers_definitions.c \
  $(AIRLINK_DIR)/Sources/cbor_definitions.c \
  $(PROJ_DIR)/main.c \
  
# Include folders common to all targets
INC_FOLDERS += \
  $(SDK_ROOT)/modules/nrfx/mdk \
  $(SDK_ROOT)/components/toolchain/cmsis/include \
  ../config \
# Include folders common to all targets from keil
INC_FOLDERS += \
  $(SDK_ROOT)/components \
  $(SDK_ROOT)/components\ble\ble_advertising \
  $(SDK_ROOT)/components\ble\ble_services\ble_dfu \
  $(SDK_ROOT)/components\ble\common \
  $(SDK_ROOT)/components\ble\nrf_ble_gatt \
  $(SDK_ROOT)/components\ble\nrf_ble_qwr \
  $(SDK_ROOT)/components\ble\peer_manager \
  $(SDK_ROOT)/components\boards \
  $(SDK_ROOT)/components\libraries\atomic \
  $(SDK_ROOT)/components\libraries\atomic_fifo \
  $(SDK_ROOT)/components\libraries\atomic_flags \
  $(SDK_ROOT)/components\libraries\balloc \
  $(SDK_ROOT)/components\libraries\bootloader \
  $(SDK_ROOT)/components\libraries\bootloader\ble_dfu \
  $(SDK_ROOT)/components\libraries\bootloader\dfu \
  $(SDK_ROOT)/components\libraries\bsp \
  $(SDK_ROOT)/components\libraries\button \
  $(SDK_ROOT)/components\libraries\crc16 \
  $(SDK_ROOT)/components\libraries\delay \
  $(SDK_ROOT)/components\libraries\experimental_section_vars \
  $(SDK_ROOT)/components\libraries\fds \
  $(SDK_ROOT)/components\libraries\fstorage \
  $(SDK_ROOT)/components\libraries\gfx \
  $(SDK_ROOT)/components\libraries\log \
  $(SDK_ROOT)/components\libraries\log\src \
  $(SDK_ROOT)/components\libraries\memobj \
  $(SDK_ROOT)/components\libraries\mutex \
  $(SDK_ROOT)/components\libraries\pwr_mgmt \
  $(SDK_ROOT)/components\libraries\ringbuf \
  $(SDK_ROOT)/components\libraries\scheduler \
  $(SDK_ROOT)/components\libraries\sortlist \
  $(SDK_ROOT)/components\libraries\strerror \
  $(SDK_ROOT)/components\libraries\svc \
  $(SDK_ROOT)/components\libraries\timer \
  $(SDK_ROOT)/components\libraries\util \
  $(SDK_ROOT)/components\softdevice\common \
  $(SDK_ROOT)/components\softdevice\s132\headers \
  $(SDK_ROOT)/components\softdevice\s132\headers\nrf52 \
  $(SDK_ROOT)/external\fprintf \
  $(SDK_ROOT)/external\segger_rtt \
  $(SDK_ROOT)/external\thedotfactory_fonts \
  $(SDK_ROOT)/integration\nrfx \
  $(SDK_ROOT)/integration\nrfx\legacy \
  $(SDK_ROOT)/modules\nrfx \
  $(SDK_ROOT)/modules\nrfx\drivers\include \
  $(SDK_ROOT)/modules\nrfx\hal \
  $(SDK_ROOT)/components\drivers_ext\ili9341 \
  $(SDK_ROOT)/components\drivers_ext\st7735 \
  $(SDK_ROOT)/components\libraries\uart \
  $(SDK_ROOT)/components\ble\ble_services\ble_nus \
  $(SDK_ROOT)/components\ble\ble_link_ctx_manager \
  $(SDK_ROOT)/components\libraries\fifo \
  $(SDK_ROOT)/components\ble\ble_services\ble_lbs \
  $(SDK_ROOT)/external\nexus\include \
  $(SDK_ROOT)/external\nexus \
  $(SDK_ROOT)/external\nexus\src \
  $(SDK_ROOT)/external\nexus\utils \
  $(SDK_ROOT)/external\nexus\examples\BLE\inc \
  $(AIRLINK_DIR)\Sources \
  $(AIRLINK_DIR)\Sources\FL_Specific \
  $(PROJ_DIR)\

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -Os -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
CFLAGS += -DBL_SETTINGS_ACCESS_ONLY
CFLAGS += -DBOARD_PCA10100
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF52833
CFLAGS += -DNRF52833_XXAA
CFLAGS += -DNRF52_PAN_74
CFLAGS += -DNRF_DFU_SVCI_ENABLED
CFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
CFLAGS += -DNRF_SD_BLE_API_VERSION=7
CFLAGS += -DS140
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -DSSL_FL 
#CFLAGS += -Wall
#CFLAGS += -Wno-traditional
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)
# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DAPP_TIMER_V2
ASMFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
ASMFLAGS += -DBL_SETTINGS_ACCESS_ONLY
ASMFLAGS += -DBOARD_PCA10100
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52833
ASMFLAGS += -DNRF52833_XXAA
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DNRF_DFU_SVCI_ENABLED
ASMFLAGS += -DNRF_DFU_TRANSPORT_BLE=1
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=7
ASMFLAGS += -DS140
ASMFLAGS += -DSOFTDEVICE_PRESENT

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52833_xxaa: CFLAGS += -D__HEAP_SIZE=8192
nrf52833_xxaa: CFLAGS += -D__STACK_SIZE=8192
nrf52833_xxaa: ASMFLAGS += -D__HEAP_SIZE=8192
nrf52833_xxaa: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: nrf52833_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52833_xxaa
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_softdevice erase

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52833_xxaa.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52833_xxaa.hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: s140_nrf52_7.2.0_softdevice.hex
	nrfjprog -f nrf52 --program $(SDK_ROOT)/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52833 --eraseall

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)
