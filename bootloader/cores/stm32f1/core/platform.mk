# ARCHLIBDIR = arch/lpc17xx

INCLUDES += $(ARCH_DIR) $(ARCH_DIR)/CMSIS $(ARCH_DIR)/StdPeriph_Driver/inc

# ASRC += $(ARCH_DIR)/startup_LPC17xx.S

LD_EXTRA_FLAGS += -specs=nosys.specs -specs=nano.specs -nostdlib -nodefaultlibs

SRC += \
	$(ARCH_DIR)/startup.c \
	$(ARCH_DIR)/CMSIS/system_stm32f10x.c \
	$(ARCH_DIR)/CMSIS/core_cm3.c \
	$(ARCH_DIR)/StdPeriph_Driver/src/misc.c \
	$(ARCH_DIR)/StdPeriph_Driver/src/stm32f10x_flash.c \
	$(ARCH_DIR)/StdPeriph_Driver/src/stm32f10x_gpio.c \
	$(ARCH_DIR)/StdPeriph_Driver/src/stm32f10x_rcc.c \
	$(ARCH_DIR)/StdPeriph_Driver/src/stm32f10x_usart.c \
	$(ARCH_DIR)/pal_flash_iap.c \
	$(ARCH_DIR)/pal_target.c \
	$(ARCH_DIR)/pal_uart.c \
	$(ARCH_DIR)/pal_ios.c

