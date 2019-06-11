################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
F021_API/%.obj: ../F021_API/%.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/Users/Shashikant/Downloads/32Bit-master/Bootloader/Boot v3 Bootloader" --include_path="C:/Users/Shashikant/Downloads/32Bit-master/Bootloader/Boot v3 Bootloader/F021_API" --include_path="C:/Users/Shashikant/Downloads/32Bit-master/Bootloader/Boot v3 Bootloader/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="F021_API/$(basename $(<F)).d_raw" --obj_directory="F021_API" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

F021_API/%.obj: ../F021_API/%.asm $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/bin/armcl" -mv7R4 --code_state=32 --float_support=VFPv3D16 --include_path="C:/Users/Shashikant/Downloads/32Bit-master/Bootloader/Boot v3 Bootloader" --include_path="C:/Users/Shashikant/Downloads/32Bit-master/Bootloader/Boot v3 Bootloader/F021_API" --include_path="C:/Users/Shashikant/Downloads/32Bit-master/Bootloader/Boot v3 Bootloader/include" --include_path="C:/ti/ccsv8/tools/compiler/ti-cgt-arm_18.1.3.LTS/include" -g --diag_warning=225 --diag_wrap=off --display_error_number --enum_type=packed --abi=eabi --preproc_with_compile --preproc_dependency="F021_API/$(basename $(<F)).d_raw" --obj_directory="F021_API" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


