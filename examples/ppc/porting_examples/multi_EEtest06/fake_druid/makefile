# # Date             : 11/30/2009
# RT-Druid version :
#

# these lines override the .bashrc definitions
#EEBASE = ../../..
#APPBASE = .

PLATFORM=LINUX
###
### Setting EEOPT variable:
###

EEOPT =


############################################################################
#
# User options
#
############################################################################
EEOPT += __ASSERT__
#EEOPT += NODEPS

############################################################################
#
# Automatic options
#
############################################################################
EEOPT += __MPC5674F__
EEOPT += __PPCE200Z7__
EEOPT += __DIAB__

EEOPT += __OO_ECC1__
EEOPT += __OO_EXTENDED_STATUS__
EEOPT += __MULTI__

EEOPT += __OO_STARTOS_OLD__
EEOPT += EE_ISR_DYNAMIC_TABLE

############################################################################
#
# Freescale MPC5674F, PPC E200Z7
#
############################################################################
EEBASE  := $(abspath $(shell dirname $$(pwd)/../../../../../../))
APPBASE := $(abspath $(shell pwd)/../)
OUTBASE := Debug


############################################################################
#
# Initialization (for Multiboot)
#
############################################################################


############################################################################
#
# Flags
#
############################################################################
CFLAGS  +=
ASFLAGS +=
LDFLAGS +=
LDDEPS  +=
LIBS    +=



############################################################################
#
# Sources
#
############################################################################
APP_SRCS += eecfg.c main.c dirty_regs.S


############################################################################
#
# end
#
############################################################################
include $(EEBASE)/pkg/cfg/rules.mk
