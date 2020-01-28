PROJECT_NAME     := ble_app_template_pca10040_s132
TARGETS          := nrf52832_xxaa
OUTPUT_DIRECTORY := _build

SDK_ROOT := .
PROJ_DIR := .

$(OUTPUT_DIRECTORY)/nrf52832_xxaa.out: \
  LINKER_SCRIPT  := ble_app_template_gcc_nrf52.ld

# $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52.S \
# Source files common to all targets
SRC_FILES += \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c \
  $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c \
  $(SDK_ROOT)/components/libraries/button/app_button.c \
  $(SDK_ROOT)/components/libraries/util/app_error.c \
  $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c \
  $(SDK_ROOT)/components/libraries/util/app_error_weak.c \
  $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c \
  $(SDK_ROOT)/components/libraries/timer/app_timer.c \
  $(SDK_ROOT)/components/libraries/util/app_util_platform.c \
  $(SDK_ROOT)/components/libraries/crc16/crc16.c \
  $(SDK_ROOT)/components/libraries/fds/fds.c \
  $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c \
  $(SDK_ROOT)/components/libraries/util/nrf_assert.c \
  $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c \
  $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c \
  $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c \
  $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf.c \
  $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c \
  $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c \
  $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c \
  $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c \
  $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c \
  $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c \
  $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c \
  $(SDK_ROOT)/components/libraries/sensorsim/sensorsim.c \
  $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c \
  $(SDK_ROOT)/components/boards/boards.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c \
  $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c \
  $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c \
  $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp.c \
  $(SDK_ROOT)/components/libraries/bsp/bsp_btn_ble.c \
  $(PROJ_DIR)/main.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c \
  $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c \
  $(SDK_ROOT)/components/ble/peer_manager/auth_status_tracker.c \
  $(SDK_ROOT)/components/ble/common/ble_advdata.c \
  $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_params.c \
  $(SDK_ROOT)/components/ble/common/ble_conn_state.c \
  $(SDK_ROOT)/components/ble/common/ble_srv_common.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/id_manager.c \
  $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c \
  $(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_database.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_id.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c \
  $(SDK_ROOT)/components/ble/peer_manager/peer_manager_handler.c \
  $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c \
  $(SDK_ROOT)/components/ble/peer_manager/security_manager.c \
  $(SDK_ROOT)/external/utf_converter/utf.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c \
  $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c \

#INC_VAR= -I .

# Include folders common to all targets
INC_FOLDER = -I./components/nfc/ndef/generic/message
INC_FOLDER += -I./components/nfc/t2t_lib
INC_FOLDER += -I./components/nfc/t4t_parser/hl_detection_procedure
INC_FOLDER += -I./components/ble/ble_services/ble_ancs_c
INC_FOLDER += -I./components/ble/ble_services/ble_ias_c
INC_FOLDER += -I./components/libraries/pwm
INC_FOLDER += -I./components/softdevice/s132/headers/nrf52
INC_FOLDER += -I./components/libraries/usbd/class/cdc/acm
INC_FOLDER += -I./components/libraries/usbd/class/hid/generic
INC_FOLDER += -I./components/libraries/usbd/class/msc
INC_FOLDER += -I./components/libraries/usbd/class/hid
INC_FOLDER += -I./modules/nrfx/hal
INC_FOLDER += -I./components/nfc/ndef/conn_hand_parser/le_oob_rec_parser
INC_FOLDER += -I./components/libraries/log
INC_FOLDER += -I./components/ble/ble_services/ble_gls
INC_FOLDER += -I./components/libraries/fstorage
INC_FOLDER += -I./components/nfc/ndef/text
INC_FOLDER += -I./components/libraries/mutex
INC_FOLDER += -I./components/libraries/gpiote
INC_FOLDER += -I./components/libraries/bootloader/ble_dfu
INC_FOLDER += -I./components/nfc/ndef/connection_handover/common
INC_FOLDER += -I./components/boards
INC_FOLDER += -I./components/nfc/ndef/generic/record
INC_FOLDER += -I./components/nfc/t4t_parser/cc_file
INC_FOLDER += -I./components/ble/ble_advertising
INC_FOLDER += -I./external/utf_converter
INC_FOLDER += -I./components/ble/ble_services/ble_bas_c
INC_FOLDER += -I./modules/nrfx/drivers/include
INC_FOLDER += -I./components/libraries/experimental_task_manager
INC_FOLDER += -I./components/ble/ble_services/ble_hrs_c
INC_FOLDER += -I./components/nfc/ndef/connection_handover/le_oob_rec
INC_FOLDER += -I./components/libraries/queue
INC_FOLDER += -I./components/libraries/pwr_mgmt
INC_FOLDER += -I./components/ble/ble_dtm
INC_FOLDER += -I./components/toolchain/cmsis/include
INC_FOLDER += -I./components/ble/ble_services/ble_rscs_c
INC_FOLDER += -I./components/ble/common
INC_FOLDER += -I./components/ble/ble_services/ble_lls
INC_FOLDER += -I./components/libraries/bsp
INC_FOLDER += -I./components/nfc/ndef/connection_handover/ac_rec
INC_FOLDER += -I./components/ble/ble_services/ble_bas
INC_FOLDER += -I./components/libraries/mpu
INC_FOLDER += -I./components/libraries/experimental_section_vars
INC_FOLDER += -I./components/softdevice/s132/headers
INC_FOLDER += -I./components/ble/ble_services/ble_ans_c
INC_FOLDER += -I./components/libraries/slip
INC_FOLDER += -I./components/libraries/delay
INC_FOLDER += -I./components/libraries/csense_drv
INC_FOLDER += -I./components/libraries/memobj
INC_FOLDER += -I./components/ble/ble_services/ble_nus_c
INC_FOLDER += -I./components/softdevice/common
INC_FOLDER += -I./components/ble/ble_services/ble_ias
INC_FOLDER += -I./components/libraries/usbd/class/hid/mouse
INC_FOLDER += -I./components/libraries/low_power_pwm
INC_FOLDER += -I./components/nfc/ndef/conn_hand_parser/ble_oob_advdata_parser
INC_FOLDER += -I./components/ble/ble_services/ble_dfu
INC_FOLDER += -I./external/fprintf
INC_FOLDER += -I./components/libraries/svc
INC_FOLDER += -I./components/libraries/atomic
INC_FOLDER += -I./components
INC_FOLDER += -I./components/libraries/scheduler
INC_FOLDER += -I./components/libraries/cli
INC_FOLDER += -I./components/ble/ble_services/ble_lbs
INC_FOLDER += -I./components/ble/ble_services/ble_hts
INC_FOLDER += -I./components/libraries/crc16
INC_FOLDER += -I./components/nfc/t4t_parser/apdu
INC_FOLDER += -I./components/libraries/util
INC_FOLDER += -I./config
INC_FOLDER += -I./components/libraries/usbd/class/cdc
INC_FOLDER += -I./components/libraries/csense
INC_FOLDER += -I./components/libraries/balloc
INC_FOLDER += -I./components/libraries/ecc
INC_FOLDER += -I./components/libraries/hardfault
INC_FOLDER += -I./components/ble/ble_services/ble_cscs
INC_FOLDER += -I./components/libraries/hci
INC_FOLDER += -I./components/libraries/timer
INC_FOLDER += -I./integration/nrfx
INC_FOLDER += -I./components/nfc/t4t_parser/tlv
INC_FOLDER += -I./components/libraries/sortlist
INC_FOLDER += -I./components/libraries/spi_mngr
INC_FOLDER += -I./components/libraries/led_softblink
INC_FOLDER += -I./components/nfc/ndef/conn_hand_parser
INC_FOLDER += -I./components/libraries/sdcard
INC_FOLDER += -I./components/nfc/ndef/parser/record
INC_FOLDER += -I./modules/nrfx/mdk
INC_FOLDER += -I./components/ble/ble_services/ble_cts_c
INC_FOLDER += -I./components/ble/ble_services/ble_nus
INC_FOLDER += -I./components/libraries/twi_mngr
INC_FOLDER += -I./components/ble/ble_services/ble_hids
INC_FOLDER += -I./components/libraries/strerror
INC_FOLDER += -I./components/libraries/crc32
INC_FOLDER += -I./components/nfc/ndef/connection_handover/ble_oob_advdata
INC_FOLDER += -I./components/nfc/t2t_parser
INC_FOLDER += -I./components/nfc/ndef/connection_handover/ble_pair_msg
INC_FOLDER += -I./components/libraries/usbd/class/audio
INC_FOLDER += -I./components/libraries/sensorsim
INC_FOLDER += -I./components/nfc/t4t_lib
INC_FOLDER += -I./components/ble/peer_manager
INC_FOLDER += -I./components/libraries/mem_manager
INC_FOLDER += -I./components/libraries/ringbuf
INC_FOLDER += -I./components/ble/ble_services/ble_tps
INC_FOLDER += -I./components/nfc/ndef/parser/message
INC_FOLDER += -I./components/ble/ble_services/ble_dis
INC_FOLDER += -I./components/nfc/ndef/uri
INC_FOLDER += -I./components/ble/nrf_ble_gatt
INC_FOLDER += -I./components/ble/nrf_ble_qwr
INC_FOLDER += -I./components/libraries/gfx
INC_FOLDER += -I./components/libraries/button
INC_FOLDER += -I./modules/nrfx
INC_FOLDER += -I./components/libraries/twi_sensor
INC_FOLDER += -I./integration/nrfx/legacy
INC_FOLDER += -I./components/libraries/usbd/class/hid/kbd
INC_FOLDER += -I./components/nfc/ndef/connection_handover/ep_oob_rec
INC_FOLDER += -I./external/segger_rtt
INC_FOLDER += -I./components/libraries/atomic_fifo
INC_FOLDER += -I./components/ble/ble_services/ble_lbs_c
INC_FOLDER += -I./components/nfc/ndef/connection_handover/ble_pair_lib
INC_FOLDER += -I./components/libraries/crypto
INC_FOLDER += -I./components/ble/ble_racp
INC_FOLDER += -I./components/libraries/fds
INC_FOLDER += -I./components/nfc/ndef/launchapp
INC_FOLDER += -I./components/libraries/atomic_flags
INC_FOLDER += -I./components/ble/ble_services/ble_hrs
INC_FOLDER += -I./components/ble/ble_services/ble_rscs
INC_FOLDER += -I./components/nfc/ndef/connection_handover/hs_rec
INC_FOLDER += -I./components/libraries/usbd
INC_FOLDER += -I./components/nfc/ndef/conn_hand_parser/ac_rec_parser
INC_FOLDER += -I./components/libraries/stack_guard
INC_FOLDER += -I./components/libraries/log/src

# Libraries common to all targets
LIB_FILES += \

# Optimization flags
OPT = -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DBOARD_PCA10040
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DNRF52
CFLAGS += -DNRF52832_XXAA
CFLAGS += -DNRF52_PAN_74 -g
CFLAGS += -DNRF_SD_BLE_API_VERSION=6
CFLAGS += -DS132
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DSWI_DISABLE0
CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall -Werror
#CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)

# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
#ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DBOARD_PCA10040
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52
ASMFLAGS += -DNRF52832_XXAA
ASMFLAGS += -DNRF52_PAN_74
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=6
ASMFLAGS += -DS132
ASMFLAGS += -DSOFTDEVICE_PRESENT
ASMFLAGS += -DSWI_DISABLE0

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
#LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52832_xxaa: CFLAGS += -D__HEAP_SIZE=8192
nrf52832_xxaa: CFLAGS += -D__STACK_SIZE=8192
nrf52832_xxaa: ASMFLAGS += -D__HEAP_SIZE=8192
nrf52832_xxaa: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: nrf52832_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52832_xxaa
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_softdevice erase

C_SOURCE_FILE_NAMES = $(notdir $(SRC_FILES))
C_PATHS = $(call remduplicates, $(dir $(SRC_FILES) ) )
OBJECTS = $(addprefix $(OUTPUT_DIRECTORY)/, $(SRC_FILES:.c=.o) )

vpath %.c $(C_PATHS)

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52832_xxaa.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52832_xxaa.hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: s132_nrf52_6.1.1_softdevice.hex
	nrfjprog -f nrf52 --program $(SDK_ROOT)/components/softdevice/s132/hex/s132_nrf52_6.1.1_softdevice.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ../config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)

## Create build directories
$(OUTPUT_DIRECTORY):
	echo $(MAKEFILE_NAME)
	$(MK) $@

$(OUTPUT_DIRECTORY)/%.o: %.c
	@echo Compiling file: $(notdir $<)
	$(CC) $(CFLAGS) $(INC_FOLDER) -c -o $(OUTPUT_DIRECTORY)/$(notdir $@) $<

archive: $(OUTPUT_DIRECTORY) $(OBJECTS)
	@echo Building: $(OBJECTS)
	$(AR) -rc $(TOPDIR)/$(TMP_LIB) $(OUTPUT_DIRECTORY)/*.o
