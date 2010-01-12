#include "mcu/microchip_pic32/inc/ee_uart.h"
#include "ee_internal.h"
#include "cpu/pic32/inc/ee_irqstub.h"

#ifdef EE_UART_PORT_1_ISR_ENABLE
static void (*Rx1IsrFunction)(EE_UINT8 data) = NULL;
#endif
#ifdef EE_UART_PORT_2_ISR_ENABLE
static void (*Rx2IsrFunction)(EE_UINT8 data) = NULL;
#endif

EE_INT8 EE_uart_init(EE_UINT8 port, EE_UINT32 baud, EE_UINT16 byte_format, 
		     EE_UINT16 mode)
{
	if (port == EE_UART_PORT_1) {
		U1MODEbits.UARTEN = 0;		// Stop UART port
		// TODO - Interrupt clear!!!
		/*IEC0bits.U1RXIE = 0;		// Disable Interrupts
		IEC0bits.U1TXIE = 0;
		IFS0bits.U1RXIF = 0;		// Clear Interrupt flag bits
		IFS0bits.U1TXIF = 0;*/
		TRISFbits.TRISF2 = 1;		// Set In RX Pin
		TRISFbits.TRISF3 = 0;		// Set Out TX Pin
		if (mode == EE_UART_CTRL_FLOW) {	
			TRISDbits.TRISD14 = 1;	// Set In CTS Pin
			TRISDbits.TRISD15 = 0;	// Set Out RTS Pin
		}
		U1BRG  = ((EE_UART_INSTRUCTION_CLOCK / (16 * baud))) - 1; 
		//U1BRG = 21 // For 115200 with Fcy = 40MHz
		U1MODE = 0; 				// Operation settings and start port
		U1MODEbits.BRGH = 0;
		U1MODEbits.UARTEN = 1;
		if (mode == EE_UART_CTRL_FLOW) {	
			U1MODEbits.RTSMD = 1;
			U1MODEbits.UEN = 2;
		} else {	
			U1MODEbits.RTSMD = 0;
			U1MODEbits.UEN = 0;	
		}
		U1MODE |= byte_format & 0x07;	// Number of bit, Parity and Stop bits
		U1STA = 0;						// TX & RX interrupt modes
		U1STAbits.UTXEN = 1;
		return 1;
	} else if (port == EE_UART_PORT_2) {
		U2MODEbits.UARTEN = 0;		// Stop UART port
		/*IEC1bits.U2RXIE = 0;		// Disable Interrupts
		IEC1bits.U2TXIE = 0;
		IFS1bits.U2RXIF = 0;		// Clear Interrupt flag bits
		IFS1bits.U2TXIF = 0;*/
		TRISFbits.TRISF4 = 1;		// Set In RX Pin
		TRISFbits.TRISF5 = 0;		// Set Out TX Pin
		if (mode == EE_UART_CTRL_FLOW) {	
			TRISFbits.TRISF12 = 1;	// Set In RX Pin
			TRISFbits.TRISF13 = 0;	// Set Out TX Pin
		}
		U2BRG  = ((EE_UART_INSTRUCTION_CLOCK / (16 * baud))) - 1; 
		//U2BRG = 21; // For 115200 with Fcy = 40MHz
		U2MODE = 0;					// Operation settings and start port
		U2MODEbits.BRGH = 0;
		U2MODEbits.UARTEN = 1;
		if (mode == EE_UART_CTRL_FLOW) {	
			U2MODEbits.RTSMD = 1;
			U2MODEbits.UEN = 2;
		} else {
			U2MODEbits.RTSMD = 0;
			U2MODEbits.UEN = 0;	
		}
		U2MODE |= byte_format & 0x07;	// Number of bit, Parity and Stop bits
		U2STA = 0;						// TX & RX interrupt modes
		U2STAbits.UTXEN = 1;
		return 1;
	}
	return -EE_UART_ERR_BAD_PORT;
}

EE_INT8 EE_uart_close(EE_UINT8 port)
{
	if (port == EE_UART_PORT_1) {
		/* chris: TODO: Release something */
		return 1;
	} else if (port == EE_UART_PORT_2) {
		/* chris: TODO: Release something */
		return 1;
	}
	return -EE_UART_ERR_BAD_PORT;
}

EE_INT8 EE_uart_set_rx_callback(EE_UINT8 port, void (*RxFunc)(EE_UINT8 data), 
				EE_UINT8 rxmode)
{
	if (port == EE_UART_PORT_1) {
		#ifdef EE_UART_PORT_1_ISR_ENABLE
		Rx1IsrFunction = RxFunc;
		if (RxFunc) {
			// TODO: interrupt served
			/*U1STA &= 0x5FFF;		
			U1STA |= rxmode & 0xA000;
			IEC0bits.U1RXIE = 1;		
			IFS0bits.U1RXIF = 0;*/
		}
		return 1;
		#else
		return -EE_UART_ERR_INT_DISABLED;
		#endif
	} else if (port == EE_UART_PORT_2) {
		#ifdef EE_UART_PORT_2_ISR_ENABLE
		Rx2IsrFunction = RxFunc;
		if (RxFunc) {
			// TODO: interrupt served
			/*U2STA &= 0x5FFF;	
			U2STA |= rxmode & 0xA000;
			IEC1bits.U2RXIE = 1;		
			IFS1bits.U2RXIF = 0;*/
		}
		return 1;
		#else
		return -EE_UART_ERR_INT_DISABLED;
		#endif
	}
	return -EE_UART_ERR_BAD_PORT;
}

EE_INT8 EE_uart_write_byte(EE_UINT8 port, EE_UINT8 data)
{
	if (port == EE_UART_PORT_1) {
		/* Polling mode */
		while (U1STAbits.UTXBF) ;
		U1TXREG = data;
		while (!U1STAbits.TRMT) ;
		return 1;
	} else if (port == EE_UART_PORT_2) {
		/* Polling mode */
		while (U2STAbits.UTXBF) ;
		U2TXREG = data;
		while (!U2STAbits.TRMT) ;
		return 1;
	}
	return -EE_UART_ERR_BAD_PORT;
}

EE_INT8 EE_uart_read_byte(EE_UINT8 port, EE_UINT8 *data)
{
	if (port == EE_UART_PORT_1) {
		#ifdef EE_UART_PORT_1_ISR_ENABLE
		if (Rx1IsrFunction == NULL) {
		#endif
			/* Polling mode */
			if (U1STAbits.OERR) {
				U1STAbits.OERR = 0;
				return -EE_UART_ERR_OVERFLOW;
			}
			if (U1STAbits.URXDA) {
				*data = U1RXREG & 0x00FF;
				return 1;
			}
			return -EE_UART_ERR_NO_DATA;
		#ifdef EE_UART_PORT_1_ISR_ENABLE
		}
		return -EE_UART_ERR_INT_MODE;
		#endif
	} else if (port == EE_UART_PORT_2) {
		#ifdef EE_UART_PORT_2_ISR_ENABLE
		if (Rx2IsrFunction == NULL) {
		#endif
			/* Polling mode */
			if (U2STAbits.OERR) {
				U2STAbits.OERR = 0;
				return -EE_UART_ERR_OVERFLOW;
			}
			if (U2STAbits.URXDA) {
				*data = U2RXREG & 0x00FF;
				return 1;
			}
			return -EE_UART_ERR_NO_DATA;
		#ifdef EE_UART_PORT_2_ISR_ENABLE
		}
		return -EE_UART_ERR_INT_MODE;
		#endif
	}
	return -EE_UART_ERR_BAD_PORT;
}


#ifdef EE_UART_PORT_1_ISR_ENABLE
// TODO: ISR management!!!
ISR2(_U1RXInterrupt)
{
	if (Rx1IsrFunction != NULL) {
		/* Execute callback function */
		//Rx1IsrFunction(U1RXREG & 0x00FF);
	}
	//IFS0bits.U1RXIF = 0;           
}
#endif

#ifdef EE_UART_PORT_2_ISR_ENABLE
// TODO: ISR management!!!
ISR2(_U2RXInterrupt)
{
	if (Rx2IsrFunction != NULL) {
		/* Execute callback function */
		//Rx2IsrFunction(U2RXREG & 0x00FF);
	}
	//IFS1bits.U2RXIF = 0;           
}
#endif
