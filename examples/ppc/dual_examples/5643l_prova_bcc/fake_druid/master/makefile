

############################################################################
#
# Makefile created by RT-Druid
# 
# Cpu 0(master)
#
############################################################################

# Include common informations
include ../common.mk

# Base dir
APPBASE ?= ../..


############################################################################
#
# EE options
#
############################################################################
EEOPT += USE_PRAGMAS
EEOPT += __PPCE200Z6__
EEOPT += __VLE__
EEOPT += __DIAB__
EEOPT += __RN__
EEOPT += __RN_TASK__
EEOPT += __OO_NO_ALARMS__
EEOPT += __OO_AUTOSTART_TASK__


############################################################################
#
# Sources
#
############################################################################
APP_SRCS +=  common.c master.c eecfg.c


############################################################################
#
# Default rules
#
############################################################################
include $(EEBASE)/pkg/cfg/rules.mk

