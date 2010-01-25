/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2008  Evidence Srl
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

#include "ee.h"
#include "test/assert/inc/ee_assert.h"

#define TRUE 1

#if defined(__HCS12XS__)
	unsigned int EE_TIMER_PRESCALER = 128;
	unsigned int EE_PRESCALE_FACTOR = EE_PRESCALE_FACTOR_128;
	unsigned long int EE_BUS_CLOCK = 2e6; 
	unsigned int EE_TIMER_PERIOD = 10;	//ms 
#endif

/* assertion data */
EE_TYPEASSERTVALUE EE_assertions[13];

DeclareTask(Task1);
DeclareTask(Task2);

TASK(Task1)
{
  StatusType s;
  EventMaskType EventMask;
  TaskStateType TaskState;

  EE_assert(1, (TRUE), -1);

#ifdef __EXTENDED_STATUS__
  s = ActivateTask(Task2);
  EE_assert(2, (s==E_OK), 1);
#else
  ActivateTask(Task2);
  EE_assert(2, TRUE, 1);
#endif

  s = SetRelAlarm(Alarm1, 1, 0);
  EE_assert(3, (s==E_OK), 2);

  EE_oo_counter_tick(Counter1);
  ForceSchedule();
  EE_assert(4, (TRUE), 3);

#ifdef __EXTENDED_STATUS__
  s = GetEvent(Task2, &EventMask);
  EE_assert(5, (s==E_OK && EventMask==Event1), 4);
#else
  GetEvent(Task2, &EventMask);
  EE_assert(5, (EventMask==Event1), 4);
#endif

#ifdef __EXTENDED_STATUS__
  s = Schedule();
  EE_assert(8, (s==E_OK), 7);
#else
  Schedule();
  EE_assert(8, TRUE, 7);
#endif

  s = SetRelAlarm(Alarm1, 1, 0);
  EE_assert(9, (s==E_OK), 8);

  EE_oo_counter_tick(Counter1);
  ForceSchedule();
  EE_assert(10, (TRUE), 9);

#ifdef __EXTENDED_STATUS__
  s = GetTaskState(Task2, &TaskState);
  EE_assert(11, (s==E_OK && TaskState==READY), 10);
#else
  GetTaskState(Task2, &TaskState);
  EE_assert(11, (TaskState==READY), 10);
#endif

  TerminateTask();
}

TASK(Task2)
{
#ifdef __EXTENDED_STATUS__
  StatusType s;
#endif

  EE_assert(6, TRUE, 5);

#ifdef __EXTENDED_STATUS__
  s = ClearEvent(Event1);
  EE_assert(7, (s==E_OK), 6);
#else
  ClearEvent(Event1);
  EE_assert(7, TRUE, 6);
#endif

#ifdef __EXTENDED_STATUS__
  s = WaitEvent(Event1);
  EE_assert(12, (s==E_OK), 11);
#else
  WaitEvent(Event1);
  EE_assert(12, (TRUE), 11);
#endif

  TerminateTask();
}

#if defined(__HCS12XS__)
#include "cpu/cosmic_hs12xs/inc/ee_irqstub.h"
volatile int timer_fired=0;
ISR2(CounterISR)
{
	unsigned int val = TC0;
	int diff;
	timer_fired++;
	/* clear the interrupt source */
	TFLG1 = 0x01;	// Clear interrupt flag

	//if (  ((signed)(TCNT-TC0)) >= 0) 	// to avoid spurious interrupts...
	if (  ((signed)(TCNT-TC0)) >= 0)
	{
		do
		{
			CounterTick(Counter1);	
			val += (unsigned int)(EE_TIMER0_STEP);
   			TC0 = val;					// to manage critical courses...
   			diff=((signed)(TCNT-val));
		}while( diff >= 0);
	}
}
#endif

int main(int argc, char **argv)
{
#if defined(__ARM7GNU__) && defined(__JANUS__)
  EE_janus_IRQ_enable_EIC();
#endif

  StartOS(OSDEFAULTAPPMODE);

  EE_assert_range(0,1,12);
  EE_assert_last();
}

#ifdef __JANUS__
EE_UINT16 fiq_arm0_handler(EE_UINT16 etu0_fir)
{
  return etu0_fir;
}
#endif
