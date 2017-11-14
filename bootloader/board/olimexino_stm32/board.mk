# Board settings
# - ARCH_DIR	used to select HAL	
# - MCU 		used for compiler-option (-mcpu)
# - CHIP		used for linker script name and passed as define	

ARCH_DIR = ../cores/stm32f1/core
MCU      = cortex-m3
CHIP     = STM32F103RBTx

CDEFS += -DSTM32 -DSTM32F1 -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER

SRC += $(BOARD_DIR)/board.c


#BOARD_FLAGS = -D __SAM3X8E__ -D NDEBUG
BOARD_INC   = .
