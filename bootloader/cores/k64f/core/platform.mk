# ARCHLIBDIR = arch/lpc17xx

INCLUDES += $(ARCH_DIR) $(ARCH_DIR)/device $(ARCH_DIR)/../variants/$(BOARD)

ASRC += $(ARCH_DIR)/device/startup_MK64F12.S

CDEFS += -DNDEBUG -DCPU_MK64FN1M0VMD12

SRC += \
	$(wildcard $(ARCH_DIR)/*.c) \
	$(wildcard $(ARCH_DIR)/device/*.c)

LD_EXTRA_FLAGS += -specs=nosys.specs -specs=nano.specs

