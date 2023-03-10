# Ultrafab Cli Framework

## Introduction
This document describes working of smartIO_cli as well as node-red application for UltrafabIQ project.

smartIO_cli is an API as well as to add new nodes for node-red application.
For more information read [smartIO_cli guide]()

## Requirements
1. Toolchain aarch64 GNU/Linux target (aarch64-none-linux-gnu) for cross compilation. Available [here](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads) for cross compilation for IMX8M.
1. IMX8MNANOD4-EVK or SmartIO board
1. Ultrafab node-red docker setup on SmartIO board

## Build and Installation for smartIO_cli 
**Please note: This is just for independent compilation of application without Yocto**

We can build CLI tool for host machine or for the target board in our case it is imx8m which can be cross compile using Linaro toolchain for aarch64.

Please download the [toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads) and untar it on your machine and note down its path

Please add the toolchain path in the PATH enviroment variable and set the compiler as follows

```bash
# include linaro toolchain bin folder into the PATH enviroment variable
export PATH=${PATH}:/mnt/d/workspace/OTS/Gateway/iMX8/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu/bin

# In my case I have untar the toolcahin in /mnt/d/workspace/OTS/Gateway/iMX8/gcc-arm-10.2-2020.11-x86_64-aarch64-none-linux-gnu folder


#set the cross compiler name
export CROSS_COMPILE=aarch64-none-linux-gnu-

```

**Finally run below command to build**
```bash
cd $project_dir/build
make clean && make # for clean build
make #for building the modified files only.

#This will create smartIO_cli executable which we can transfer to the board and run for board bring up and testing 
```
## How to Register a new task for CLI 
1. Create a src/\*.c and inc/\*.h file with the function definition and declaration respectively in the project directory. 
1. Create a task/function in this C file, this function will be called when user passes argument to run this CLI functionality
1. Create a helper function that will print the details of CLI functionality for this new task. 
1. Once functionality is defined then register the newly created task in the CLI tool, to do so call below function and pass the required arguments, to add this refer the main function in main.c file.
    ```c
    /**
     * @brief           Register a command line task that will be executed by cli
     * 
     * @param cmdName    Name of command that will be used to run a specific task from command line      
     * @param desc       Command description which will be used to print in helper
     * @param numberArg  Number of arguments required for processing of this task. 
     *                   This is useful in case we would like to run multiple commands in single execution
     * @param helper     Pointer to helper function for specific task help print, it will be called in case
     *                   of invalid arg or for help  
     * @param task       Task or thread to be run when this command is executed from command line
     * @param arg        Optional parameter to be passed from the calling of the task
     * 
     * @return int       registered status; 0 - Failed, 1 - Success
     */
    int cli_register_cmd(char *cmdName, char *desc, char numberArg, void *helper, void *task, void *arg );
    ```
1. After adding the cli_register_cmd we need to add this file in build/sources.mk file for compilation. Add the newly created file in the C_SRCS, OBJS and C_DEPS. e.g gpio_cli.c is the newly created file so we need to add ../src/gpio_cli.c, ./src/gpio_cli.o and ./src/gpio_cli.d files in the respective variables.
```bash
    C_SRCS += \
    ../src/main.c  \
    ../src/cli_opt_parser.c \
    ../src/cJSON.c \
    ../src/vector.c \
    ../src/gpio_cli.c \
    ../src/config.c 
    
    OBJS += \
    ./src/main.o \
    ./src/cli_opt_parser.o\
    ./src/vector.o \
    ./src/cJSON.o \
    ./src/gpio_cli.o \
    ./src/config.o 
    
    C_DEPS := \
    ./src/main.d \
    ./src/cli_opt_parser.d\
    ./src/vector.d \
    ./src/cJSON.d \
    ./src/gpio_cli.d \
    ./src/config.d 
```
1. Once we have added our file for compilation, run the make command as mentioned in `how to build` section. It will add our newly created task in CLI tool, you can see it by running `./smartIO_cli --help`



## How to use CLI tool
To check the available commands already added in this tool run the below command 
```
$ ./smartIO_cli #OR ./smartIO_cli --help  
No argument provided!!!!
 
 
Usage:  ./smartIO_cli [OPTIONS] COMMAND

 
Options: 
   -c COMMAND           CLI commands to be executed
   -h, --help           To print the help 
   -d number            To add delay, it is useful to execute mulitple commands with added delay between them
 
 
Commands: 
   LED             Test an LED or GPIO 
   ADC             Test an ADC or GPIO 
   DAC             Test an DAC or GPIO

 
Run './smartIO_cli COMMAND --help' for more information on a command.
```
 
As per the helper text from the tool we can see there are 3 CLI (LED, ADC, DAC) commands are available with its description.

We can execute a specific command from the CLI as below.
```
$ ./smartIO_cli -c LED 6 ON
CLI command LED is called with argc: 3
argv[0]: LED
argv[1]: 6
argv[2]: ON
```

**We can execute multiple commands with some delay as below.**
```bash
$ ./smartIO_cli -c LED 6 ON -d 1000 -c LED 6 OFF
CLI command LED is called with argc: 9
argv[0]: LED
argv[1]: 6
argv[2]: ON
1615383119
delay is added [7]: -c, 1000
1615383120
CLI command LED is called with argc: 3
argv[0]: LED
argv[1]: 6
argv[2]: OFF
```


## List of Integrated commands additional feature command


### Delay
    we have given a option to add user defin delay between two commands. To add delay we can use -d option and specify the delay in msec. Once -d  $delay is provided in between two cli commands then the number of specified delay will be added and next command will be executed once the delay expires.

Usage:

```bash
Usage:  ./smartIO_cli [OPTIONS] COMMAND

Options:
   -c COMMAND           CLI commands to be executed
   -h, --help           To print the help
   -d Delay             To add delay, it is useful to execute mulitple commands

Commands:
   LED             Test an LED or GPIO
   GPIO            Test a GPIO
   AIN             Test Analog input
   AOUT            Test Analog output
   IOExp           Test IOExpander and SET or RESET the IOExp ports

Run './smartIO_cli COMMAND --help' for more information on a command.
```
E.g.:  ./smartIO_cli -c GPIO 1 1 out 1 -d 5000 -c GPIO 1 1 out 0
        Here it will add the delay of 5 seconds between two commands on the GPIO


### GPIO interface

this command is used to SET, RESET and Read GPIO available on the board.

Usage:
```
 GPIO command Usage:
       ./smartIO_cli -c GPIO [PORT] [PIN] [DIR] [VALUE]
          available PORT: 1-5
          available PIN: 0-31
          available DIR:
              in - GPIO as input mode
              out - GPIO as output mode
              read - Read the GPIO without setting/changing its direction
          available VALUE:
              0 - Low
              1 - High
          Note: value parameter mandatory for CLI but will be ignored 'DIR as in'
```

e.g: ./smartIO_cli -c GPIO 1 1 out 1
    