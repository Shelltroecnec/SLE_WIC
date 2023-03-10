# Add inputs and outputs from these tool invocations to the build variables 
# C_SRCS += ./src/example_sms.c 

C_SRCS += \
../src/LL_Drivers/gpio.c  \
../src/LL_Drivers/i2c.c \
../src/LL_Drivers/led.c \
../src/LL_Drivers/mmio.c \
../src/LL_Drivers/pwm.c \
../src/LL_Drivers/serial.c \
../src/LL_Drivers/spi.c 

OBJS += \
./src/LL_Drivers/gpio.o \
./src/LL_Drivers/i2c.o\
./src/LL_Drivers/mmio.o \
./src/LL_Drivers/led.o \
./src/LL_Drivers/pwm.o \
./src/LL_Drivers/serial.o \
./src/LL_Drivers/spi.o 

C_DEPS += \
./src/LL_Drivers/gpio.d \
./src/LL_Drivers/i2c.d\
./src/LL_Drivers/mmio.d \
./src/LL_Drivers/led.d \
./src/LL_Drivers/pwm.d \
./src/LL_Drivers/serial.d \
./src/LL_Drivers/spi.d 

