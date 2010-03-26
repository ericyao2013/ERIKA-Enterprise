/*
Copyright (C) 2009 -  Claudio Salvadori and Christian Nastasi
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#define _hv7131gp_c_

#include "hv7131gp.h"
#include "hv7131gp_hal.h"

/******************************************************************************/
/*                         Global Variables                                   */
/******************************************************************************/




/******************************************************************************/
/*                          Local Variables                                   */
/******************************************************************************/
volatile void (* capture_complete_func) (hv7131gp_status_t) = NULL;

volatile uint8_t *frame_buffer = NULL;

volatile uint8_t  frame_height = 0;
volatile uint8_t  frame_width = 0;
volatile int16_t frame_size = 0;

/* Configuration static variables */
static uint8_t act_res; 					//actual resolution
static int16_t act_w, act_h, act_x, act_y;	//size and position of the window


/******************************************************************************/
/*                             Local Functions                                */
/******************************************************************************/
/* ----------------------------------------------------------------------------
|  Initialization sequence for HV7131GP Camera (see refman):                   |
|                                                                              |
|  0 - Set ENABLE = RESETB = high and start master clock;                      |
|    - Wait more than 1 ms;                                                    |
|  1 - Set RESETB = low (start reset condition);                               |
|    - Wait more than 4 master clock cycles;                                   |
|  2 - Set RESETB = high (stop reset condition);                               |
|    NOTE: after 2000000 master clock cycles the 1st VSYNC should be reced!    |
|  3 - Wait 2086000 master clock cycles (logic stable time);                   |
 ---------------------------------------------------------------------------- */
static void init_sequence(void)
{

	HV7131GP_PIN_ENABLE_HIGH();
	HV7131GP_PIN_RESETB_HIGH();
	HV7131GP_PIN_MCLK_START();
	
	//Delay more than 1ms: for example 1500 us
	hv7131gp_hal_delay_us(HV7131GP_MORE_THAN_1_MS);
	
	//First sequence
	HV7131GP_PIN_RESETB_LOW();
	//Delay more than 4MCLK cycles: 1us (MCLK = 13.3MHz)
	hv7131gp_hal_delay_us(HV7131GP_MORE_THAN_4_MCLK_CYCLES);

	//Second sequence
	HV7131GP_PIN_RESETB_HIGH();
	//Delay more than 2086000MCLK cycles: 250ms (MCLK = 13.3MHz)
	hv7131gp_hal_delay_us(HV7131GP_MORE_THAN_2086000_MCLK_CYCLES);

}

/* ----------------------------------------------------------------------------
|  Acknoledgement from the camera init: wait for 1st interrupt                 |
|  Returns:                                                                    |
|  1 - 1st interrupt is arrived after a timer                                  |
|                                                                              |
|  0 - 1st interrupt is not arrived                                            |
 ---------------------------------------------------------------------------- */
static hv7131gp_status_t init_ack(void)
{

	//TODO: insert a time-out
	while (!HV7131GP_VSYNC_IF_VALUE) ;
	
	/*Reset the IF flag*/
	HV7131GP_VSYNC_RESET_IF();
	
	/*Check the correct lowering of the interrupt pin */
	while (HV7131GP_VSYNC_IF_VALUE) ;	
	
	return HV7131GP_SUCCESS;
}

/******************************************************************************/
/*                          Function Definitions                              */
/******************************************************************************/

/* ----------------------------------------------------------------------------
|  Write a HV7131GP Camera Register:                                           |
|                                                                              |
|  - TODO: make some check on the register value?                              |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_reg_write(hv7131gp_reg_t reg, uint8_t val)
{
	return hv7131gp_i2c_hal_reg_write(reg, val);
}

/* ----------------------------------------------------------------------------
|  Read a HV7131GP Camera Register:                                            |
|                                                                              |
|  - TODO: make some check on the register value?                              |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_reg_read(hv7131gp_reg_t reg, uint8_t *val)
{
	return hv7131gp_i2c_hal_reg_read(reg, val);
}

/* ----------------------------------------------------------------------------
|  Initializes HV7131GP Camera:                                                |
|                                                                              |
|  - init the ENABLE, RESETB, MCLK pins;                                       |
|  - init the I2C interface;                                                   |
|  - begin the initialization sequence described in the device ref. man.       |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_init(void)
{
	/* Initializing pins */
	HV7131GP_PIN_ENABLE_INIT();
	HV7131GP_PIN_RESETB_INIT();
	HV7131GP_PIN_MCLK_INIT(HV7131GP_MCLK_PERIOD);


	HV7131GP_PIN_VSYNC_INIT_POSITIVE();
	HV7131GP_PIN_HSYNC_INIT();
	HV7131GP_PIN_VCLK_INIT();
	HV7131GP_PIN_EOF_INIT();

	HV7131GP_PIN_Y_INIT();

	
	
	/* Initialize the internal peripheral, setting the comunication 
								protocol */
	if (hv7131gp_hal_init(0) != HV7131GP_SUCCESS)
		return -HV7131GP_HAL_INIT_ERR;


	/* Start camera initialization sequence (see device refman) */
	init_sequence();
	
	//The answer to the init procedure: Vsync interrupt
	/*TODO: change this in future by mean of callbacks! */
	if(init_ack() != HV7131GP_SUCCESS)
		return HV7131GP_ERR_DEV_NOINIT;

	return hv7131gp_init_configure(); 
}

/* ----------------------------------------------------------------------------
|  Start configuration of HV7131GP Camera:                                     |
|                                                                              |
|  This functios is called inside the HV7131GP init function to preset the     |
|  some variable; in this way is prevented the un-initialization of certain    |
|  variables that are absolutely necessary to capture image, and is possible   |
|  to use the camera without calling anyone configure function.                |
|  Default settings are:						       |
|  1. 160x120 resolution						       |
|  2. Full window       						       |
|  3. Time division: 16							       |
|  4. HBLANK = 208							       |
|  5. VBLANK = 8							       |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_init_configure(void)
{
	hv7131gp_status_t help;

	//Time division: 16
	help = hv7131gp_configure_time_divisor(HV7131GP_T_5);

	if (help != HV7131GP_SUCCESS)
		return help;

	//Resolution: 160x120
	help = hv7131gp_configure_resolution(HV7131GP_1_16_SUB);

	if (help != HV7131GP_SUCCESS)
		return help;

	//Full window
	help = hv7131gp_window_set(0x280, 0x1E0, 0x2, 0x2);

	if (help != HV7131GP_SUCCESS)
		return help;

	//VBLANK: 8
	help = hv7131gp_vblank_set(8);

	if (help != HV7131GP_SUCCESS)
		return help;

	//HBLANK: 208
	help = hv7131gp_hblank_set(208);

	if (help != HV7131GP_SUCCESS)
		return help;

	/* *****set size***** */
	/*TODO: perchè questo hard-coding??? */
	frame_width = 160;
	frame_height = 120;
	frame_size = frame_height * frame_width;

	return HV7131GP_SUCCESS;
}
/* ----------------------------------------------------------------------------
|  Highest level configure function for HV7131GP Camera:                      |
|                                                                              |
|  160x120 resolution, variable time divisor, fixed HBLANK (208) and           |
|  fixed VBLANK(8)                                                             |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_configure(hv7131gp_Q_Value_t Q)
{
	hv7131gp_status_t help;

	/* Set 1:16 subsampling and X-Flip */
	help = hv7131gp_configure_resolution(HV7131GP_1_16_SUB);
	if (help != HV7131GP_SUCCESS)
		return help;

	/* Set complete window*/
	help = hv7131gp_window_set(640, 480, 2, 2);
	if (help != HV7131GP_SUCCESS)
		return help;

	// Set HBLANK values
	help = hv7131gp_hblank_set(208);
	if (help != HV7131GP_SUCCESS)
		return help;

	// Set VBLANK values
	help = hv7131gp_vblank_set(8);
	if (help != HV7131GP_SUCCESS)
		return help;

	switch(Q)
	{
	case HV7131GP_160x120_FAST:

		//Configure time divisor (D = 2)
		help = hv7131gp_configure_time_divisor(HV7131GP_T_2);
		if (help != HV7131GP_SUCCESS)
			return help;
		break;

	case HV7131GP_160x120_MEDIUM:

		//Configure time divisor (D = 16)
		help = hv7131gp_configure_time_divisor(HV7131GP_T_5);
		if (help != HV7131GP_SUCCESS)
			return help;

		break;

	case HV7131GP_160x120_SLOW:

		//Configure time divisor (D = 128)
		help = hv7131gp_configure_time_divisor(HV7131GP_T_8);
		if (help != HV7131GP_SUCCESS)
			return help;

		break;
	default:
		return HV7131GP_FAILURE;
		break;
	}

	return HV7131GP_SUCCESS;
}

/* ----------------------------------------------------------------------------
|  Configure the clock division on HV7131GP Camera:                            |
|                                                                              |
|  This function set via i2c the value of the master clock divisor, and then   |
|  the VCLK (pixel clock) and the frame rate. For more infos read the related  |
|  documentation.							       |
|  1. MCLK divided by 1							       |
|  2. MCLK divided by 2							       |
|  3. MCLK divided by 4							       |
|  4. MCLK divided by 8							       |
|  5. MCLK divided by 16						       |
|  6. MCLK divided by 32						       |
|  7. MCLK divided by 64						       |
|  8. MCLK divided by 128						       |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_configure_time_divisor(hv7131gp_T_D_Value_t div)
{
	hv7131gp_status_t help;

	switch(div)
	{
	case HV7131GP_T_1:	//Divisor set to 1
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRB, HV7131GP_DCF_1);

		if (help != HV7131GP_SUCCESS)
			return help;

		break;

	case HV7131GP_T_2:	//Divisor set to 2
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRB, HV7131GP_DCF_2);

		if (help != HV7131GP_SUCCESS)
			return help;

		break;

	case HV7131GP_T_3:	//Divisor set to 4
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRB, HV7131GP_DCF_4);

		if (help != HV7131GP_SUCCESS)
			return help;

		break;

	case HV7131GP_T_4:	//Divisor set to 8
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRB, HV7131GP_DCF_8);

		if (help != HV7131GP_SUCCESS)
			return help;

		break;

	case HV7131GP_T_5:	//Divisor set to 16
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRB, HV7131GP_DCF_16);

		if (help != HV7131GP_SUCCESS)
			return help;

		break;

	case HV7131GP_T_6:	//Divisor set to 32
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRB, HV7131GP_DCF_32);

		if (help != HV7131GP_SUCCESS)
			return help;

		break;

	case HV7131GP_T_7:	//Divisor set to 64
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRB, HV7131GP_DCF_64);

		if (help != HV7131GP_SUCCESS)
			return help;
		break;

	case HV7131GP_T_8:	//Divisor set to 128
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRB, HV7131GP_DCF_128);

		if (help != HV7131GP_SUCCESS)
			return help;

		break;

	default:
		return HV7131GP_FAILURE;
		break;

	}
	return HV7131GP_SUCCESS;
}

/* ----------------------------------------------------------------------------
|  Configure the image resolution on HV7131GP Camera:                          |
|                                                                              |
|  This function set via i2c the possible value of sub-sampling                |
|  1. No sub-sampling 	(640x480)					       |
|  2. 1/4 sub-sampling 	(320x240)					       |
|  3. 1/16 sub-sampling	(160x120)					       |
|  									       |
|  This function can change the value frame_width, frame_height 	       |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_configure_resolution(hv7131gp_R_Value_t res)
{
	hv7131gp_status_t help;

	switch(res)
	{
	case HV7131GP_NO_SUB : /* 640x480 pixel resolution*/

		//Y only configuration in the 8 pin Y[0..7]
		help = hv7131gp_reg_write(HV7131GP_REG_OUTFMT,
					  HV7131GP_OUTFMT_DEFAULT & (~HV7131GP_8BIT_OUTPUT));

		if (help != HV7131GP_SUCCESS)
			return help;

		//Set the resolution value: No sub-sampling (640x480)
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRA,
					  HV7131GP_VIDEO_NORMAL | HV7131GP_X_FLIP);

		if (help != HV7131GP_SUCCESS)
			return help;

		// Save the actual resolution
		act_res = 1; //Divide the window sizes by 1 (640x480)

		break;

	case HV7131GP_1_4_SUB: /* 320x240 pixel resolution */

		//Y only configuration in the 8 pin Y[0..7]
		help = hv7131gp_reg_write(HV7131GP_REG_OUTFMT,
					  HV7131GP_OUTFMT_DEFAULT & (~HV7131GP_8BIT_OUTPUT));

		if (help != HV7131GP_SUCCESS)
			return help;

		//Set the resolution value: 1/4 sub-sampling (320x240)
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRA,
					  HV7131GP_VIDEO_SUB_4 | HV7131GP_X_FLIP);

		if (help != HV7131GP_SUCCESS)
			return help;

		// Save the actual resolution
		act_res = 2; //Divide the window sizes by 2 (320x240)

		break;

	case HV7131GP_1_16_SUB: /* 160x120 pixel resolution */

		//Y only configuration in the 8 pin Y[0..7]
		help = hv7131gp_reg_write(HV7131GP_REG_OUTFMT,
					  HV7131GP_OUTFMT_DEFAULT & (~HV7131GP_8BIT_OUTPUT));

		if (help != HV7131GP_SUCCESS)
			return help;

		//Set the resolution value: 1/4 sub-sampling (160x120)
		help = hv7131gp_reg_write(HV7131GP_REG_SCTRA,
					  HV7131GP_VIDEO_SUB_16 | HV7131GP_X_FLIP);

		if (help != HV7131GP_SUCCESS)
			return help;

		// Save the actual resolution
		act_res = 4; //Divide the window sizes by 4 (160x120)

		break;

	default:
		return HV7131GP_FAILURE;
		break;
	}

	/* *****Set size***** */
	frame_width 	= act_w / act_res;
	frame_height 	= act_h / act_res;
	frame_size = frame_height * frame_width;

	return HV7131GP_SUCCESS;
}

/* ----------------------------------------------------------------------------
|  Set the window size and position on HV7131GP Camera:                        |
|                                                                              |
|  This function set via i2c size and position of a window                     |
|									       |
|  This function can change the value frame_width, frame_height 	       |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_window_set(int16_t width, int16_t height, int16_t x, int16_t y)
{
	hv7131gp_status_t help;

	//Check the window size: a window have to be smaller size than 640x480
	if((width > 0x280) || (height > 0x1E0))
		return HV7131GP_FAILURE;

	//Check the window position: inferior limit
	if((x < 0x2) || (y < 0x2))
		return HV7131GP_FAILURE;

	//Check the window position: superior limit
	if((x > 0x280) || (y > 0x1E0))
		return HV7131GP_FAILURE;

	/* *****Set window size***** */

	//Set window width (high value)
	help = hv7131gp_reg_write(HV7131GP_REG_WIWH, (width >> 8) & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	//Set window width (low value)
	help = hv7131gp_reg_write(HV7131GP_REG_WIWL, width & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	//Save the actual window width
	act_w = width;

	//Set window height (high value)
	help = hv7131gp_reg_write(HV7131GP_REG_WIHH, (height >> 8) & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	//Set window height (low value)
	help = hv7131gp_reg_write(HV7131GP_REG_WIHL, height & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	//Save the actual window height
	act_h = height;

	/* *****Set window position***** */

	//Set window x position (high value)
	help = hv7131gp_reg_write(HV7131GP_REG_CSAH, (x >> 8) & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	//Set window x position (low value)
	help = hv7131gp_reg_write(HV7131GP_REG_CSAL, x & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	//Save the actual window x position
	act_x = x;

	//Set window y position (high value)
	help = hv7131gp_reg_write(HV7131GP_REG_RSAH, (y >> 8) & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	//Set window y position (low value)
	help = hv7131gp_reg_write(HV7131GP_REG_RSAL, y & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	//Save the actual window y position
	act_y = y;

	/* *****Set size***** */
	frame_width 	= width / act_res;
	frame_height 	= height / act_res;
	frame_size = frame_height * frame_width;

	return HV7131GP_SUCCESS;

}

/* ----------------------------------------------------------------------------
|  Set the HBLANK value on HV7131GP Camera:                        	       |
|                                                                              |
|  This function set via i2c the HBLANK value		                       |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_hblank_set(int16_t hb)
{
	hv7131gp_status_t help;

	
	//hb check
	if (hb < 208) 
		return HV7131GP_FAILURE;

	//Setting low bits of HBLANK
	help = hv7131gp_reg_write(HV7131GP_REG_HBLANKL, hb & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	//Setting high bits of HBLANK
	help = hv7131gp_reg_write(HV7131GP_REG_HBLANKH, (hb >> 8) & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	return HV7131GP_SUCCESS;
}

/* ----------------------------------------------------------------------------
|  Set the VBLANK value on HV7131GP Camera:                        	       |
|                                                                              |
|  This function set via i2c the VBLANK value		                       |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_vblank_set(int16_t vb)
{
	hv7131gp_status_t help;

	//vb check
	if (vb < 8) 
		return HV7131GP_FAILURE;

	//Setting low bits of VBLANK
	help = hv7131gp_reg_write(HV7131GP_REG_VBLANKL, vb & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	//Setting high bits of VBLANK
	help = hv7131gp_reg_write(HV7131GP_REG_VBLANKH, (vb >> 8) & (0x00FF));

	if (help != HV7131GP_SUCCESS)
		return help;

	return HV7131GP_SUCCESS;
}
/* ----------------------------------------------------------------------------
|  Configure the resolution and the window size in safe mode:                  |
|                                                                              |
|  High level function that set the window values and the resolution, after    |
|  a check about the compatibility between the maximum picture size in the     |
|  selected hardware and the real size defined by the settings.                |
|									       |
|  The values width, height, x, y everytime are referred to 640x480 resolution |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_configure_safe(int16_t width, int16_t height, int16_t x,
		 			int16_t y, hv7131gp_R_Value_t res) {
	hv7131gp_status_t help;

	switch(res)
	{
	case HV7131GP_NO_SUB:
		frame_size = width * height; 	//Size requested (no sub-sampling)
		if(frame_size <= HV7131GP_MAX_SIZE){

			//Set the window
			help = hv7131gp_window_set(width, height, x, y);
			if (help != HV7131GP_SUCCESS)
				return help;

			//Set resolution
			help = hv7131gp_configure_resolution(res);
			if (help != HV7131GP_SUCCESS)
				return help;

			//Set variables
			frame_width = width;
			frame_height = height;

			return HV7131GP_SUCCESS;
		} else {
			//Out of the range: reset variables
			frame_size = 0;
			frame_width = 0;
			frame_height = 0;

			return HV7131GP_FAILURE;
		}
		break;
	case HV7131GP_1_4_SUB:
		frame_size = width * height >> 2; //Size requested (1/4 sub-sampling)
		if(frame_size <= HV7131GP_MAX_SIZE){

			//Set the window
			help = hv7131gp_window_set(width, height, x, y);
			if (help != HV7131GP_SUCCESS)
				return help;

			//Set resolution
			help = hv7131gp_configure_resolution(res);
			if (help != HV7131GP_SUCCESS)
				return help;

			//Set variables
			frame_width = width >> 1;
			frame_height = height >> 1;

			return HV7131GP_SUCCESS;
		} else {
			//Out of the range: reset variables
			frame_size = 0;
			frame_width = 0;
			frame_height = 0;

			return HV7131GP_FAILURE;
		}
		break;
	case HV7131GP_1_16_SUB:
		frame_size = width * height >> 4; //Size requested (1/16 sub-sampling)
		if(frame_size <= HV7131GP_MAX_SIZE){

			//Set the window
			help = hv7131gp_window_set(width, height, x, y);
			if (help != HV7131GP_SUCCESS)
				return help;

			//Set resolution
			help = hv7131gp_configure_resolution(res);
			if (help != HV7131GP_SUCCESS)
				return help;

			//Set variables
			frame_width = width >> 2;
			frame_height = height >> 2;

			return HV7131GP_SUCCESS;
		} else {
			//Out of the range: reset variables//Out of the range: reset variables
			frame_size = 0;
			frame_width = 0;
			frame_height = 0;

			return HV7131GP_FAILURE;
		}

		break;
	default:
		return HV7131GP_FAILURE;
		break;
	}


}

/* ----------------------------------------------------------------------------
|  Start Capture from HV7131GP Camera :                                        |
|                                                                              |
|  - TODO:                                                                     |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_capture(uint8_t *image, void (* func) (hv7131gp_status_t))
{
	if (image == NULL)
		return HV7131GP_ERR_MEM_NULL;
	
	frame_buffer = image;
	capture_complete_func = func;

	HV7131GP_VSYNC_RESET_IF();

	//Enables the interrupt associated with the VSYN
	HV7131GP_PIN_VSYNC_START();	/* Initializes the Frame Sync */

    return HV7131GP_SUCCESS;
}

/* ----------------------------------------------------------------------------
|  Start Capture from HV7131GP Camera :                                        |
|                                                                              |
|  - TODO:                                                                     |
 ---------------------------------------------------------------------------- */
uint8_t hv7131gp_get_width(void)
{
	return frame_width;
}

uint8_t hv7131gp_get_height(void)
{
	return frame_height;
}

uint16_t hv7131gp_get_size(void)
{
	return frame_size;
}

uint8_t hv7131gp_get_Y_average(void)
{
	uint8_t Y = 0;
	/*Read the Y mean value*/
	if (hv7131gp_reg_read(HV7131GP_REG_YFMEAN, &Y) != HV7131GP_SUCCESS )
		for (;;) ;

	return Y;
}

/* ----------------------------------------------------------------------------
|  Set HV7131GP to sleep mode:                                        |
|                                                                              |
|  - TODO:                                                                     |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_set_sleep_status(void)
{
	uint8_t dummy = 0;

	/*Read the current value of SCTRB register*/
	if (hv7131gp_reg_read(HV7131GP_REG_SCTRB, &dummy) != HV7131GP_SUCCESS )
		for (;;) ;

	/*Set sleep mode*/
	return hv7131gp_reg_write(HV7131GP_REG_SCTRB, dummy | HV7131GP_SLEEP_MODE);
}

/* ----------------------------------------------------------------------------
|  Set HV7131GP to active mode:                                                |
|                                                                              |
|  - TODO:                                                                     |
 ---------------------------------------------------------------------------- */
hv7131gp_status_t hv7131gp_set_active_status(void)
{
	uint8_t dummy = 0;

	/*Read the current value of SCTRB register*/
	if (hv7131gp_reg_read(HV7131GP_REG_SCTRB, &dummy) != HV7131GP_SUCCESS )
		for (;;) ;

	/*Set sleep mode*/
	return hv7131gp_reg_write(HV7131GP_REG_SCTRB, dummy & (~HV7131GP_SLEEP_MODE));
}