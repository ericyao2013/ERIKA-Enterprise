/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2017  Evidence Srl
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

/*
 * Author: 2005 Antonio Romano
 * 	   2017 Giuseppe Serano
 */


#ifndef __INCLUDE_ATTINY_MCU_H__
#define __INCLUDE_ATTINY_MCU_H__

/*************************************************************************
 MCU Common Types
 *************************************************************************/

#include "mcu/common/inc/ee_mcu_common_types.h"

void EE_attiny_intvect(void);

/*************************************************************************
 MCU
 *************************************************************************/

__INLINE__ void EE_mcu_init(void)
{
  /* Interrupt Vector Table Recall. */
  EE_attiny_intvect();

  /* Open OSEK/VDX Kernels Disable Interrupts Inside ShutdownOS(). */
#if	( \
  !defined(__OO_BCC1__) && !defined(__OO_BCC2__) && \
  !defined(__OO_ECC1__) && !defined(__OO_ECC2__) \
)
  /* Interrupt Enabling */
  EE_hal_enableIRQ();
#endif	/*
	 * defined(__OO_BCC1__) || defined(__OO_BCC2__) ||
	 * defined(__OOSECC1__) || defined(__OO_ECC2__)
	 */
}

/*************************************************************************
 Timers
 *************************************************************************/
#ifdef __TIMER_0_USED__

#ifndef __TIMER_USED__
#define __TIMER_USED__
#endif

extern void EE_attiny_timer0_start(void);
extern void EE_attiny_timer0_stop(void);
extern EE_UREG EE_attiny_timer0_get(void);
extern void EE_attiny_init_timer0(EE_UREG);

__INLINE__ void __ALWAYS_INLINE__ EE_timer_0_start(void)
{
	EE_attiny_timer0_start();
}
	
__INLINE__ void __ALWAYS_INLINE__ EE_timer_0_stop(void)
{
	EE_attiny_timer0_stop();
}

__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_timer_0_get(void)
{
	EE_UREG time0=0;
	time0 = EE_attiny_timer0_get();
	return time0;
}

__INLINE__ void __ALWAYS_INLINE__ EE_timer_0_set(EE_UREG count_down) 
{
	/*EE_attiny_timer0_set(count_down); */
}

#endif /* __TIMER_0_USED__ */

#ifdef __TIMER_1_USED__

#ifndef __TIMER_USED__
#define __TIMER_USED__
#endif

extern void EE_attiny_timer1_start(void);
extern void EE_attiny_timer1_stop(void);
extern EE_UREG EE_attiny_timer1_get(void);
extern void EE_attiny_init_timer1(EE_UINT16);

__INLINE__ void __ALWAYS_INLINE__ EE_timer_1_start(void) 
{
	EE_attiny_timer1_start();
}

__INLINE__ void __ALWAYS_INLINE__ EE_timer_1_stop(void) 
{
	EE_attiny_timer1_stop();
}

__INLINE__ EE_UREG __ALWAYS_INLINE__ EE_timer_1_get(void) 
{ 
	EE_UINT16 time1;
	time1 = EE_attiny_timer1_get();
	return time1;
}

__INLINE__ void __ALWAYS_INLINE__ EE_timer_1_set(EE_UREG count_down)
{
	/*EE_attiny_timer1_set(count_down); */
}

#endif /* __TIMER_1_USED__ */

#ifdef __TIMER_USED__

__INLINE__ void __ALWAYS_INLINE__ EE_timer_init0(EE_UINT8 max_tick)
{
#ifdef __TIMER_0_USED__
	EE_attiny_init_timer0(max_tick);
#endif
}

__INLINE__ void __ALWAYS_INLINE__ EE_timer_init1(EE_UINT16 max_tick)
{
#ifdef __TIMER_1_USED__
	EE_attiny_init_timer1(max_tick);
#endif
}
#endif /* __TIMER_USED__ */

/*
 * Timer
 */

#ifdef __TIME_SUPPORT__
/* Timer size */
typedef EE_UINT16 EE_TIME;

/* read current time (used by EDF scheduler) */
__INLINE__ EE_TIME __ALWAYS_INLINE__ EE_hal_gettime(void)
{
	return EE_timer_1_get();
}
#endif

#endif	/* __INCLUDE_ATTINY_MCU_H__ */

