# Add inputs and outputs from these tool invocations to the build variables 
# C_SRCS += ./src/example_sms.c 

C_SRCS += \
../src/main.c  \
../src/cli_opt_parser.c \
../src/cJSON.c \
../src/vector.c \
../src/gpio_cli.c \
../src/ioexpander.c \
../src/analog_cli.c \
../src/rtd_cli.c \
../src/timer_event.c \
../src/timer.c \
../src/digital_in.c \
../src/tools.c \
../src/max31865.c \
../src/ad7988.c \
../src/relay_cli.c \
../src/dac_cli.c \
../src/sd_card_cli.c \
../src/serial_cli.c \
../src/modbus_cli.c \
../src/nordic.c \
../src/analog_out_cli.c \
../src/motor_cli.c \
../src/version.c \
../src/qspi_flash.c \
../src/ram_cli.c \
../src/emmc_cli.c \
../src/usb_cli.c \
../src/frame_parser.c \
../src/tcp_interface.c \
../src/shared_mem.c \
../src/pwm_cli.c \
../src/simcard.c \
../src/power_select_cli.c \
../src/lvds.c \
../src/config.c \
../src/mcp48fvb22.c \
../src/digital_out.c \
../src/W25Q32.c \
../src/flash.c


OBJS += \
./src/main.o \
./src/cli_opt_parser.o\
./src/vector.o \
./src/cJSON.o \
./src/gpio_cli.o \
./src/ioexpander.o \
./src/analog_cli.o \
./src/rtd_cli.o \
./src/timer_event.o \
./src/timer.o \
./src/digital_in.o \
./src/tools.o \
./src/max31865.o \
./src/ad7988.o \
./src/relay_cli.o \
./src/dac_cli.o \
./src/sd_card_cli.o \
./src/serial_cli.o \
./src/modbus_cli.o \
./src/nordic.o \
./src/analog_out_cli.o \
./src/motor_cli.o \
./src/version.o \
./src/qspi_flash.o \
./src/ram_cli.o \
./src/emmc_cli.o \
./src/usb_cli.o \
./src/frame_parser.o \
./src/tcp_interface.o \
./src/shared_mem.o \
./src/pwm_cli.o \
./src/simcard.o \
./src/power_select_cli.o \
./src/lvds.o \
./src/config.o \
./src/mcp48fvb22.o \
./src/digital_out.o \
./src/W25Q32.o \
./src/flash.o

C_DEPS += \
./src/main.d \
./src/cli_opt_parser.d\
./src/vector.d \
./src/cJSON.d \
./src/gpio_cli.d \
./src/ioexpander.d \
./src/analog_cli.d \
./src/rtd_cli.d \
./src/timer_event.d \
./src/timer.d \
./src/digital_in.d \
./src/tools.d \
./src/max31865.d \
./src/ad7988.d \
./src/relay_cli.d \
./src/dac_cli.d \
./src/sd_card_cli.d \
./src/serial_cli.d \
./src/modbus_cli.d \
./src/nordic.d \
./src/analog_out_cli.d \
./src/motor_cli.d \
./src/version.d \
./src/qspi_flash.d \
./src/ram_cli.d \
./src/emmc_cli.d \
./src/usb_cli.d \
./src/frame_parser.d \
./src/tcp_interface.d \
./src/shared_mem.d \
./src/pwm_cli.d \
./src/simcard.d \
./src/power_select_cli.d \
./src/lvds.d \
./src/config.d \
./src/mcp48fvb22.d \
./src/digital_out.d \
./src/W25Q32.d \
./src/flash.d

