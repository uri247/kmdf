///////////////////////////////////////////////////////////////////////////////
// File:          Hw8250.c
//
// Description:   825 chip hardware access functionality
//                for the simple sample SMPL device driver.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright © datronicsoft.  2012
///////////////////////////////////////////////////////////////////////////////
#include "driver.h"

#include "Hw8250.h"

ULONG Hw8550InternalGetDivisor(ULONG baudRate);

///////////////////////////////////////////////////////////////////////////////
// Does           : Initializes the device.
// Parameters     : port           - Port of a 8250
//                  baudrate       - Baudrate to init 8250 to
//                  dataBits       - Number of data bits (5..8)
//                  stopBits       - Number of stop bits (1..2)
//                  parity         - 0 == NONE, 1 == ODD, 2 == EVEN
// Returns        : Status of the operation.
// Postcondition  : Ready to start interrupt processing when returning
//                  STATUS_SUCCESS.
///////////////////////////////////////////////////////////////////////////////
NTSTATUS Hw8250Init(PUCHAR port, ULONG baudRate, ULONG dataBits,
                        ULONG stopBits, ULONG parity)
{
    NTSTATUS result = STATUS_SUCCESS;
    ULONG divisor;
    UCHAR dataFormat;

    // disable all interrupts
    WRITE_INTERRUPT_ENABLE(port, 0x00);

    // set up baudrate
    divisor = Hw8550InternalGetDivisor(baudRate);
    WRITE_LINE_CONTROL(port, LCR_DLAB);
    WRITE_DIVISOR_LATCH(port, divisor);

    // set up data bits (defaults to 8 data bits)
    switch(dataBits) {
        case 5:
            dataFormat = LCR_DATA_5;
            break;

        case 6:
            dataFormat = LCR_DATA_6;
            break;

        case 7:
            dataFormat = LCR_DATA_7;
            break;

        case 8:
        default:
            dataFormat = LCR_DATA_8;
            break;
    }

    // set up stop bits (defaults to 1 stop bit)
    if(stopBits == 2) {
        dataFormat |= LCR_STOP_2;
    }
    else {
        dataFormat |= LCR_STOP_1;
    }

    // set up parity (defaults to none)
    switch(parity) {
        case 4:
            dataFormat |= LCR_PARITY_SPACE;
            break;
        
        case 3:
            dataFormat |= LCR_PARITY_MARK;
            break;
        
        case 2:
            dataFormat |= LCR_PARITY_EVEN;
            break;
        
        case 1:
            dataFormat |= LCR_PARITY_ODD;
            break;

        case 0:
        default:
            dataFormat |= LCR_PARITY_NONE;
            break;
    }

    WRITE_LINE_CONTROL(port, dataFormat);
    WRITE_MODEM_CONTROL(port, MCR_GPO1 | MCR_RTS | MCR_DTR);

    return result;

} // end Hw8250Init

///////////////////////////////////////////////////////////////////////////////
// Does           : Returns the divisor for the desired baudrate.
// Parameters     : baudRate		- Desired baud reate
// Returns        : Divisor value.
///////////////////////////////////////////////////////////////////////////////
ULONG Hw8550InternalGetDivisor(ULONG baudRate)
{
    typedef struct {
        ULONG baudRate;
        ULONG divisor;
    } BAUD_TAB_ENTRY;
    
    // supported baudrates
    BAUD_TAB_ENTRY tab[] = {
             50, 0x0900,      75, 0x0600,     150, 0x0300,
            300, 0x0180,     600, 0x00c0,    1200, 0x0060,
           2400, 0x0030,    4800, 0x0018,    9600, 0x000c,
          19200, 0x0006,   38400, 0x0003,   56000, 0x0002,
              0, 0x0001 };
    ULONG i;

    for(i = 0; i < (sizeof(tab)/sizeof(BAUD_TAB_ENTRY))-1; i++) {
        if(baudRate <= tab[i].baudRate) {
            break;
        }
    }

    return tab[i].divisor;

} // end Hw8550InternalGetDivisor
