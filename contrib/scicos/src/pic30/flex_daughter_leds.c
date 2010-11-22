/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2006-2010  Simone Mannori, Roberto Bucher
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation, 
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

/** 
	@file flex_daughter_leds.c
	@brief www.scicos.org, www.scicoslab.org
	@author Roberto Bucher, SUPSI- Lugano
	@author Simone Mannori, ScicosLab developer
	@date 2006-2010
*/ 
 
 
//** 9 Feb 2008 : Revision notes by Simone Mannori 
//**

#include <machine.h>
#include <scicos_block4.h>

#include <ee.h>

/* FLEX DMB LEDS allocation 

Scicos	Function	dsPIC
Pin=1	LED1	RF0
Pin=2	LED2	RF1
Pin=3	LED3	RF2
Pin=4	LED4	RF3
Pin=5	LED5	RD8
Pin=6	LED6	RD9
Pin=7	LED7	RD10
Pin=8	LED8	RD11
*/

static void init(scicos_block *block)
{
	int pin = block->ipar[0];
	
	float led_threshold = block->rpar[0];
	

#if defined(__USE_DEMOBOARD__)	
	if ( (pin < 1) || (pin > 8) ) return; //** return if outside the allowed range
#elif defined(__USE_MOTIONBOARD__)
	if ( (pin < 1) || (pin > 2) ) return; //** return if outside the allowed range
#endif


	if( led_threshold < 0.01 || led_threshold > 0.99 )
		led_threshold = 0.5;
	
	EE_daughter_leds_init();
}

static void inout(scicos_block *block)
{
  float *u = block->inptr[0];

	int pin = block->ipar[0];

	float led_threshold = block->rpar[0];

	if( led_threshold < 0.01 || led_threshold > 0.99 )
		led_threshold = 0.5;

#if defined(__USE_DEMOBOARD__)	
	if ( (pin < 1) || (pin > 8) ) return; //** return if outside the allowed range
#elif defined(__USE_MOTIONBOARD__)
	if ( (pin < 1) || (pin > 2) ) return; //** return if outside the allowed range
#endif

	if (u[0] > led_threshold) {     //** threshold is parametric 
		switch (pin) { //** set the bit to one 
			case 1:
				EE_led_0_on();
				break;
			case 2:
				EE_led_1_on();
				break;
#if defined(__USE_DEMOBOARD__)	
			case 3:
				EE_led_2_on();
				break;
			case 4:
				EE_led_3_on();
				break;
			case 5:
				EE_led_4_on();
				break;
			case 6:
				EE_led_5_on();
				break;
			case 7:
				EE_led_6_on();
				break;
			case 8:
				EE_led_7_on();
				break;
#endif
		}
	} else {
		switch (pin) { //** set the bit to zero 
			case 1:
				EE_led_0_off();
				break;
			case 2:
				EE_led_1_off();
				break;
#if defined(__USE_DEMOBOARD__)	
			case 3:
				EE_led_2_off();
				break;
			case 4:
				EE_led_3_off();
				break;
			case 5:
				EE_led_4_off();
				break;
			case 6:
				EE_led_5_off();
				break;
			case 7:
				EE_led_6_off();
				break;
			case 8:
				EE_led_7_off();
				break;
#endif
		}
	}
}

static void end(scicos_block *block)
{
	int pin = block->ipar[0];
	
	if ( (pin < 1) || (pin > 8) )
		return; //** return if outside the alowed range

	EE_leds_off(); //** set the bit to zero at the ending 
}

void flex_daughter_leds(scicos_block *block,int flag)
{
 switch (flag) {
    case OutputUpdate:  /* set output */
      inout(block);
      break;

    case StateUpdate: /* get input */
      break;

    case Initialization:  /* initialisation */
      init(block);
      break;

    case Ending:  /* ending */
      end(block);
      break;
  }
}
