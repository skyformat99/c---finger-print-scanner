#include "_Uart.h"
#include "GPIORegs.h"
#include "INTRRegs.h"
#include "INTRDefs.h"
#include <stdarg.h>
#include "types.h"
//#include "Timer.c"
#include  "_Uart_Addr.h"
#include <stdio.h>
extern void HaltFunc();



#define W_REG(reg, data) (*reg) = (data)
#define R_REG(reg) (*reg)

static void UartIntr (void) __attribute__ ((interrupt ("IRQ")));









/*
 * UART Initialization
 */
void _Uart_Init()
{
	
	
    /*
        FIFO control register, FIFO disable
    */
    
    
	*(UFCON1) = UCON_RESET;

    /*
        MODEM control register, AFC disable
    */
	*(UMCON1) = UMCON_RESET;

    /*
        Line control register : Normal,No parity,1 stop,8 bits
    */
	*(ULCON1) = (ULCON_MODE_NORM | ULCON_PARITY_NONE | ULCON_STOP_ONE | ULCON_LENGTH_8);

    /*
        Uart Control register
    */
	*(UCON1) = (UCON_CLK_PCLK | UCON_TXINT_LEVEL | UCON_RXERR_ENABLE | UCON_TXMODE_INT | UCON_RXMODE_INT);

    /*
        Baud rate divisior register
    */
	*(UBRDIV1) = 325;//FOR 9600 BPS.

	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
	__asm("nop");
}


/*
 * ==============================================================================================================
 */

void _Uart_SendChar(U8 cChar)
{

   
            while((*(UFSTAT1)) & UFSTAT_TXFULL);
          
     
    

    W_REG((UTXH1), cChar);
}


/*
 * ==============================================================================================================
 */


void _Uart_SendString( U8 *sString)
{
    while(*sString)
    {
        _Uart_SendChar((*(sString++)));
    }
}


/*
 * ==============================================================================================================
 */

U8 _Uart_Getch()
{
        
            while(!((*(UFSTAT1)) & UFSTAT_RXCOUNT));
               
    
    return (U8)(*(URXH1));
}

/*
 * ==============================================================================================================
 */
U8 receive_buffer[12];
int received = 0;

static void  UartIntr(void)
{
    int recvStringLength=12;
    volatile U32 rUart;

    if (*INTPND & BIT_UART1) {
        rUart = *UTRSTAT1;
        if(rUart & 0x2) {
            *SUBSRCPND = BIT_SUB_TXD1;
        }
        if(rUart & 0x1) {
            while ((*UFSTAT1) & 0x3F)
            {
                /*
                    Receive data ready
                */
                receive_buffer[received++] = (U8)*(URXH1);
                recvStringLength --;
            }

            
            *SUBSRCPND = BIT_SUB_RXD1;
        }

        *SRCPND = BIT_UART1;
        *INTPND = BIT_UART1;
    }

}
//send by uart and recv by uart response from fps
U8* _Uart_Send_get(U8* StringToSend)
{
   
   int  StringToSendLength = 12;

   

    /*
     *  Disable interrupts & ubInterrupts from UART1
    */
    *INTMSK |=  (BIT_UART1);
    *INTSUBMSK  |= (BIT_SUB_TXD1 | BIT_SUB_RXD1);


    /*
     Set iSRs for UART1
    */

    pISR_UART1 = (U32)UartIntr;


    /*
        Set  UART1 to provide pulse interrupts,
        and get recieve timeout interrupts.
        Use UCON registers
    */
    *(UCON1) = 0xc5;

    /*
         reset UART FIFOs, in order to clear any redundant
        FIFO data.
      
    */
    *(UFCON1) = 0x6;

    /*
         let the reset operation to complete.
    */
    Timer_Bdelay_Milli (1);

    /*
        FIFO mode enabled, reciever threshold is 8 bytes,
        transmitter threshold is 0 bytes.
        We will get a reciever interrupt either once we
        get at least 8 bytes, or a timeout occured (we have
        to set also the line control register for that.
        Use UFCON registers.
    */
    *(UFCON1) = 0x11;

    /*
        Set frame to Normal,No parity,1 stop,8 bits
        Use ULCON registers
    */
    *(ULCON1) = 0x3;


 //rUTXH1 is a pointer to a byte address.
    while (StringToSendLength)
   {
        /*
            Write one byte of the string to the UART transmitter FIFO
        */
        *(UTXH1) = (*StringToSend);
        StringToSendLength--; //counter
        StringToSend++;    //pointer
    }


    /*
    * Enable interrupts.
    */
   *INTSUBMSK &= ~((BIT_SUB_TXD1<< (3)) | (BIT_SUB_RXD1 << (3)));
    *INTMSK &= ~ (BIT_UART1);


    while (StringToSendLength != 0);//wail until all data shifted

    /*
     * 	 disable interrupts for recieve
    */

    *INTMSK |=  (BIT_UART1);
    *INTSUBMSK  |= (BIT_SUB_TXD1 | BIT_SUB_RXD1);
    Timer_Bdelay_Milli (1);
    for (; StringToSendLength < received;)
            Uart_Printf ("0x%x  ", receive_buffer[StringToSendLength ++]);
    Uart_Printf ("\nUart operation completed successfuly\n");



    return receive_buffer;

  }



