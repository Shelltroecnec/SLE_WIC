################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf


SDK_PATH   ?= "$(shell pwd)/../../.."

EXP_TARGETS = TestCodeMQ1710_2

LIBS  =  -static -Wl,--whole-archive -lrt -lpthread -Wl,--no-whole-archive -lm

#--------------------------------------------------------
# TOOLS BASIC CONFIG
# Note: No Need to change them
#--------------------------------------------------------
CPPFLAGS += -I./ \
            -I../inc \
            -I../inc/LL_Drivers \
			-I../inc/mqtt/inc 
            # -I$(SDK_PATH)/include \
            # -I$(SDK_PATH)/lib/interface/inc \
            # -I$(SDKTARGETSYSROOT)/usr/include \
            # -I$(SDKTARGETSYSROOT)/usr/include \
            # -I$(SDKTARGETSYSROOT)/usr/include/data \
            # -I$(SDKTARGETSYSROOT)/usr/include/dsutils \
            # -I$(SDKTARGETSYSROOT)/usr/include/qmi \
            # -I$(SDKTARGETSYSROOT)/usr/include/qmi-framework


LDFLAGS += ${EXP_LDLIBS}              \
           -L$(SDKTARGETSYSROOT)/usr/lib \
           -L$(SDK_PATH)/lib 

#
# NR_BUILD_FLAG = -static
# MQTT_FLAG = -DMQTT_INTERFACE
# MQTT_LIB = ../lib/libpaho-mqtt3a.so 
# MQTT_FLAG = -fsanitize=address
# MQTT_FLAG =  -static

# this is added to add shared object directly


# STD_LIB= ../lib/libpaho-mqtt3a.so  
        #  $(SDKTARGETSYSROOT)/usr/lib/libdsutils.so         \
        #  $(SDKTARGETSYSROOT)/usr/lib/libqmiservices.so     \
        #  $(SDKTARGETSYSROOT)/usr/lib/libqmi_cci.so         \
        #  $(SDKTARGETSYSROOT)/usr/lib/libqmi_common_so.so   \
        #  $(SDKTARGETSYSROOT)/usr/lib/libqmi.so             \
        #  $(SDKTARGETSYSROOT)/usr/lib/libmcm.so             \


# USR_LIB=$(SDK_PATH)/lib/libql_common_api.a
# USR_LIB+=$(SDK_PATH)/lib/libql_peripheral.a  #used for uart and other peripheral


-include ../makefile.defs
-include ./src/LL_Drivers/sources.mk
-include ./src/sources.mk

# All Target
all: ulad_daemon

# Tool invocations
ulad_daemon: $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'    
	$(CROSS_COMPILE)gcc $(MQTT_FLAG) $(MQTT_LIB) -s $(LDFLAGS) $(USR_LIB) $(OBJS) $(USR_LIB) $(STD_LIB) -o $@ $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '


-include $(C_DEPS)

./src/%.o: ../src/%.c Makefile
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	$(CROSS_COMPILE)gcc $(MQTT_FLAG) $(CPPFLAGS) $(LDFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@"  -c "$<" $(LIBS)
	@echo 'Finished building: $<'
	@echo ' '


#$(GPP) $(OPTIONS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"

# Other Targets
clean:
	-$(RM) *.o $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(OBJS)$(C_UPPER_DEPS)$(CXX_DEPS)$(C_DEPS)$(CPP_DEPS) ./ulad_daemon
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:


.PHONY:checkmake
checkmake:  
	@echo -e "CURDIR =		\n	${CURDIR}"  
	@echo -e "\nMAKE_VERSION =	\n	${MAKE_VERSION}"  
	@echo -e "\nMAKEFILE_LIST =	\n	${MAKEFILE_LIST}"  
	@echo -e "\nCOMPILE.c =		\n	${COMPILE.c}"
	@echo -e "\nCOMPILE.cc =	\n	${COMPILE.cc}"
	@echo -e "\nCOMPILE.cpp =	\n	${COMPILE.cpp}"
	@echo -e "\nLINK.cc =		\n	${LINK.cc}"
	@echo -e "\nLINK.o =		\n	${LINK.o}"
	@echo -e "\nCPPFLAGS =		\n	${CPPFLAGS}"
	@echo -e "\nCFLAGS =		\n	${CFLAGS}"
	@echo -e "\nCXXFLAGS =		\n	${CXXFLAGS}"
	@echo -e "\nLDFLAGS =		\n	${LDFLAGS}"
	@echo -e "\nLDLIBS =		\n	${LDLIBS}"


-include ../makefile.targets

