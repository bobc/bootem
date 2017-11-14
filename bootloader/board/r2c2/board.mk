# Board settings
# - ARCH_DIR	used to select HAL	
# - MCU 		used for compiler-option (-mcpu)
# - CHIP		used for linker script name and passed as define	

ARCH_DIR = ../cores/lpc17xx/core
MCU      = cortex-m3
CHIP     = LPC1758


SRC += $(BOARD_DIR)/board.c


#BOARD_FLAGS = -D __SAM3X8E__ -D NDEBUG
#BOARD_INC   = .
