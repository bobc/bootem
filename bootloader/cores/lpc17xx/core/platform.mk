# ARCHLIBDIR = arch/lpc17xx

INCLUDES += $(ARCH_DIR) $(ARCH_DIR)/CMSIS/inc $(ARCH_DIR)/include

ASRC += $(ARCH_DIR)/startup_LPC17xx.S

LD_EXTRA_FLAGS += -nostartfiles

SRC += $(ARCH_DIR)/CMSIS/src/system_LPC17xx.c \
	$(ARCH_DIR)/CMSIS/src/core_cm3.c \
	$(ARCH_DIR)/source/lpc17xx_nvic.c \
	$(ARCH_DIR)/source/lpc17xx_pinsel.c \
	$(ARCH_DIR)/source/lpc17xx_gpio.c \
	$(ARCH_DIR)/source/lpc17xx_uart.c \
	$(ARCH_DIR)/source/lpc17xx_clkpwr.c \
	$(ARCH_DIR)/pal_flash_iap.c \
	$(ARCH_DIR)/pal_target.c \
	$(ARCH_DIR)/pal_uart.c \
	$(ARCH_DIR)/pal_ios.c

