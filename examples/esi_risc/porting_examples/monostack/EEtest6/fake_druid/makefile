# # Date             : 1/18/2010
# RT-Druid version : 
#

# these lines override the .bashrc definitions
#EEBASE = ../../..
#APPBASE = .

###
### Setting EEOPT variable:
###

EEOPT =


############################################################################
#
# User options
#
############################################################################
EEOPT += DEBUG
EEOPT += NODEPS
EEOPT += __ASSERT__


############################################################################
#
# Automatic options
#
############################################################################
EEOPT += __RTD_CYGWIN__
EEOPT += __ESI_RISC__
EEOPT += __OO_BCC1__
EEOPT += __ALLOW_NESTED_IRQ__
EEOPT += __OO_EXTENDED_STATUS__
EEOPT += __MONO__
EEOPT += __OO_NO_ALARMS__
EEOPT += __OO_NO_RESOURCES__
EEOPT += __OO_AUTOSTART_TASK__
EEOPT += __OO_STARTOS_OLD__


############################################################################
#
# EnSilica eSi-RISC
#
############################################################################
EEBASE  := $(shell cygpath `cygpath -ms "C:\ERIKA"`)
APPBASE := 
OUTBASE := Debug

TARGET_OPTIONS := -msoc=esi3200_cycloneiiidemo -O2

############################################################################
#
# Flags
#
############################################################################
CFLAGS += $(TARGET_OPTIONS)
ASFLAGS += $(TARGET_OPTIONS)
LDFLAGS += $(TARGET_OPTIONS)
LDDEPS  += 
LIBS    += 

# Link in the lightweight startup code for a smaller final image
LDFLAGS +=  -mcrt0s

############################################################################
#
# Sources
#
############################################################################
APP_SRCS += eecfg.c main.c


############################################################################
#
# end
#
############################################################################
include $(EEBASE)/pkg/cfg/rules.mk
