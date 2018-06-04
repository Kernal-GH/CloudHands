
ifeq ($(RTE_SDK),)
$(error "Please define RTE_SDK environment variable")
endif

# Default target, can be overriden by command line or environment
RTE_TARGET ?= x86_64-native-linuxapp-gcc

include $(RTE_SDK)/mk/rte.vars.mk
unexport RTE_SRCDIR RTE_OUTPUT RTE_EXTMK

DIRS-y += util assemble app packets core
.PHONY: all clean $(DIRS-y)

clean all: $(DIRS-y)

$(DIRS-y):
	$(MAKE) -C $@ $(MAKECMDGOALS)

include $(RTE_SDK)/mk/rte.app.mk
