# ###*B*###
# ERIKA Enterprise - a tiny RTOS for small microcontrollers
#
# Copyright (C) 2002-2010  Evidence Srl
#
# This file is part of ERIKA Enterprise.  ERIKA Enterprise is free software;
# you can redistribute it and/or modify it under the terms of the
# GPLv2 + Linking Exception license (contained in the file `ee/LICENSE').
#
# ERIKA Enterprise is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the license for more details.
#
# ###*E*### */

## Makefile for Erika PPC projects
## Author: 2010-2011,  Bernardo  Dal Seno


# Destination directory
export OUTPUT_DIR = output

# Check the environment and use reasonable defaults
ifndef RTDRUID_ECLIPSE_HOME
RTDRUID_ECLIPSE_HOME = C:/Evidence/Evidence/eclipse
RTDRUID_SCRIPT_DIR =
endif
ifndef RTDRUID_SCRIPT_DIR
RTDRUID_SCRIPT_DIR=$(RTDRUID_ECLIPSE_HOME)/evidence
endif

RTDRUID_OUTPUT = $(addprefix $(OUTPUT_DIR)/, makefile eecfg.h eecfg.c \
	system.orti)

.PHONY: all clean

all: $(RTDRUID_OUTPUT)
	$(MAKE) -C $(OUTPUT_DIR) all

clean:
	@echo cleaning...
	rm -f -r $(OUTPUT_DIR)


ifeq ($(FAKE_RTDRUID),1)
# Simulate RT-Druid
ifndef EEBASE
EEBASE=../../../../
endif

$(RTDRUID_OUTPUT): $(OUTPUT_DIR)/% : fake_druid/%
	@echo "Substituing RT-Druid in generating $@..."
	@mkdir -p $(OUTPUT_DIR)
	cp $< $@
else

ifneq (ok,$(shell test -d "$(RTDRUID_ECLIPSE_HOME)" && echo ok ))
$(error The variable RTDRUID_ECLIPSE_HOME must point to a valid Eclipse RT-Druid installation)
endif
# Run RT-Druid
$(RTDRUID_OUTPUT): conf.oil
	@echo "Running RT-Druid..."
	@ECLIPSE_HOME="$(RTDRUID_ECLIPSE_HOME)" ;\
	export ECLIPSE_HOME ;\
	exec sh $(RTDRUID_SCRIPT_DIR)/start_ant.sh -buildfile $(RTDRUID_SCRIPT_DIR)/build.xml -Din_conf_oil=$< -Dconf_output_dir=$(OUTPUT_DIR) compile
endif
