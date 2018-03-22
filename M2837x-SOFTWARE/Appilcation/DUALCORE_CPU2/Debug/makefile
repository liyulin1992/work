################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := D:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS

GEN_OPTS__FLAG := 
GEN_CMDS__FLAG := 

ORDERED_OBJS += \
"./F2837xD_Adc.obj" \
"./F2837xD_CodeStartBranch.obj" \
"./F2837xD_DefaultISR.obj" \
"./F2837xD_GlobalVariableDefs.obj" \
"./F2837xD_Gpio.obj" \
"./F2837xD_Ipc.obj" \
"./F2837xD_PieCtrl.obj" \
"./F2837xD_PieVect.obj" \
"./F2837xD_SysCtrl.obj" \
"./F2837xD_usDelay.obj" \
"./can.obj" \
"./interrupt.obj" \
"./main.obj" \
"./cla/cla_sqrt.obj" \
"./cla/sqrt.obj" \
"./drv/hbhk_adc.obj" \
"./drv/hbhk_can.obj" \
"./drv/hbhk_pwm.obj" \
"./flashburn/Fapi_UserDefinedFunctions.obj" \
"./flashburn/copydata.obj" \
"./flashburn/flash_program_test.obj" \
"../2837xD_FLASH_CLA_lnk_cpu1.cmd" \
"../F2837xD_Headers_nonBIOS_cpu2.cmd" \
"../flashburn/F021_API_F2837xD_FPU32.lib" \
$(GEN_CMDS__FLAG) \
-llibc.a \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include cla/subdir_vars.mk
-include drv/subdir_vars.mk
-include flashburn/subdir_vars.mk
-include subdir_rules.mk
-include cla/subdir_rules.mk
-include drv/subdir_rules.mk
-include flashburn/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
hbhk_cpu2.out \

EXE_OUTPUTS__QUOTED += \
"hbhk_cpu2.out" \

BIN_OUTPUTS += \
hbhk_cpu2.hex \

BIN_OUTPUTS__QUOTED += \
"hbhk_cpu2.hex" \


# All Target
all: hbhk_cpu2.out

# Tool invocations
hbhk_cpu2.out: $(OBJS) $(CMD_SRCS) $(LIB_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: C2000 Linker'
	"D:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -O0 --fp_mode=relaxed --define=CPU2 -g --diag_warning=225 --diag_wrap=off --display_error_number -z -m"hbhk_cpu2.map" --stack_size=0x200 --warn_sections -i"D:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/lib" -i"../../../F2837xD_common/lib" -i"D:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/include" --reread_libs --diag_wrap=off --display_error_number --xml_link_info="hbhk_cpu2_linkInfo.xml" --rom_model -o "hbhk_cpu2.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

hbhk_cpu2.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: C2000 Hex Utility'
	"D:/ti/ccsv7/tools/compiler/ti-cgt-c2000_16.9.1.LTS/bin/hex2000"  -o "hbhk_cpu2.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "F2837xD_Adc.obj" "F2837xD_CodeStartBranch.obj" "F2837xD_DefaultISR.obj" "F2837xD_GlobalVariableDefs.obj" "F2837xD_Gpio.obj" "F2837xD_Ipc.obj" "F2837xD_PieCtrl.obj" "F2837xD_PieVect.obj" "F2837xD_SysCtrl.obj" "F2837xD_usDelay.obj" "can.obj" "interrupt.obj" "main.obj" "cla\cla_sqrt.obj" "cla\sqrt.obj" "drv\hbhk_adc.obj" "drv\hbhk_can.obj" "drv\hbhk_pwm.obj" "flashburn\Fapi_UserDefinedFunctions.obj" "flashburn\copydata.obj" "flashburn\flash_program_test.obj" 
	-$(RM) "F2837xD_Adc.d" "F2837xD_DefaultISR.d" "F2837xD_GlobalVariableDefs.d" "F2837xD_Gpio.d" "F2837xD_Ipc.d" "F2837xD_PieCtrl.d" "F2837xD_PieVect.d" "F2837xD_SysCtrl.d" "can.d" "interrupt.d" "main.d" "cla\cla_sqrt.d" "drv\hbhk_adc.d" "drv\hbhk_can.d" "drv\hbhk_pwm.d" "flashburn\Fapi_UserDefinedFunctions.d" "flashburn\copydata.d" "flashburn\flash_program_test.d" 
	-$(RM) "cla\sqrt.d" 
	-$(RM) "F2837xD_CodeStartBranch.d" "F2837xD_usDelay.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

