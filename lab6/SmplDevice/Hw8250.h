///////////////////////////////////////////////////////////////////////////////
// File:          Hw8250.
//
// Description:   This module contains the definitions for the  
//                hardware access routines of the 8250 chip.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright © datronicsoft.  2011
///////////////////////////////////////////////////////////////////////////////

#pragma once

// register layout
#define PORT_TRANSMITTER_BUFFER			0x00
#define PORT_RECEIVER_BUFFER			0x00
#define PORT_INTERRUPT_ENABLE			0x01
#define PORT_INTERRUPT_IDENTIFICATION	0x02
#define PORT_LINE_CONTROL				0x03
#define PORT_MODEM_CONTROL				0x04
#define PORT_LINE_STATUS				0x05
#define PORT_MODEM_STATUS				0x06
#define PORT_DIVISOR_LATCH_LSB			0x00
#define PORT_DIVISOR_LATCH_MSB			0x01


// how to write
#define WRITE_TRANSMITTER_BUFFER(base,val)		WRITE_PORT_UCHAR((UCHAR *)(base+PORT_TRANSMITTER_BUFFER),val); KeStallExecutionProcessor(50)
#define WRITE_INTERRUPT_ENABLE(base,val)		WRITE_PORT_UCHAR((UCHAR *)(base+PORT_INTERRUPT_ENABLE),val); KeStallExecutionProcessor(50)
#define WRITE_LINE_CONTROL(base,val)			WRITE_PORT_UCHAR((UCHAR *)(base+PORT_LINE_CONTROL),val); KeStallExecutionProcessor(50)
#define WRITE_MODEM_CONTROL(base,val)			WRITE_PORT_UCHAR((UCHAR *)(base+PORT_MODEM_CONTROL),val); KeStallExecutionProcessor(50)
#define WRITE_DIVISOR_LATCH(base,val)													\
{																						\
	WRITE_PORT_UCHAR((UCHAR *)(base+PORT_DIVISOR_LATCH_LSB), (UCHAR)((val) & 0x00ff));	\
	WRITE_PORT_UCHAR((UCHAR *)(base+PORT_DIVISOR_LATCH_MSB), (UCHAR)((val) >> 8));		\
	KeStallExecutionProcessor(50); \
}

// how to read
#define READ_RECEIVER_BUFFER(base)				READ_PORT_UCHAR((UCHAR *)(base+PORT_RECEIVER_BUFFER))
#define READ_INTERRUPT_ENABLE(base)				READ_PORT_UCHAR((UCHAR *)(base+PORT_INTERRUPT_ENABLE))
#define READ_INTERRUPT_IDENTIFICATION(base)		READ_PORT_UCHAR((UCHAR *)(base+PORT_INTERRUPT_IDENTIFICATION))
#define READ_LINE_CONTROL(base)					READ_PORT_UCHAR((UCHAR *)(base+PORT_LINE_CONTROL))
#define READ_MODEM_CONTROL(base)				READ_PORT_UCHAR((UCHAR *)(base+PORT_MODEM_CONTROL))
#define READ_LINE_STATUS(base)					READ_PORT_UCHAR((UCHAR *)(base+PORT_LINE_STATUS))
#define READ_MODEM_STATUS(base)					READ_PORT_UCHAR((UCHAR *)(base+PORT_MODEM_STATUS))
#define READ_DIVISOR_LATCH(base,pval)										\
{																			\
	UCHAR lsb = READ_PORT_UCHAR((UCHAR *)(base+PORT_DIVISOR_LATCH_LSB));	\
	UCHAR msb = READ_PORT_UCHAR((UCHAR *)(base+PORT_DIVISOR_LATCH_MSB));	\
	*(pval) = (((USHORT)msb) << 8) + lsb;									\
}

// interrupt enable
#define IER_RECEIVER_FULL		0x01
#define IER_TRANSMITTER_EMPTY	0x02
#define IER_RECEIVER_ERROR		0x04
#define IER_STATUS_CHANGE		0x08

// interrupt identification register
#define IIR_STATUS_CHANGE		0x00
#define IIR_TRANSMITTER_EMPTY	0x02
#define IIR_RECEIVER_FULL		0x04
#define IIR_RECEIVER_ERROR		0x06
#define IIR_NOT_FIRED			0x01

// line control
#define LCR_DLAB				0x80
#define LCR_BREAK				0x40
#define LCR_PARITY_NONE			0x00
#define LCR_PARITY_ODD			0x08
#define LCR_PARITY_EVEN			0x18
#define LCR_PARITY_MARK			0x28
#define LCR_PARITY_SPACE		0x38
#define LCR_STOP_1				0x00
#define LCR_STOP_2				0x04
#define LCR_DATA_5				0x00
#define LCR_DATA_6				0x01
#define LCR_DATA_7				0x02
#define LCR_DATA_8				0x03

// modem control
#define MCR_DTR					0x01
#define MCR_RTS					0x02
#define MCR_GPO0				0x04
#define MCR_GPO1				0x08
#define MCR_LOOPBACK			0x10

// line status register
#define LSR_EMPTY				0x20

// port           - Port of a 8250
// baudRate       - Baudrate to init 8250 to
// dataBits       - Number of data bits (5..8)
// stopBits       - Number of stop bits (1..2)
// parity         - 0 == NONE, 1 == ODD, 2 == EVEN

NTSTATUS Hw8250Init(PUCHAR port, ULONG baudRate, ULONG dataBits,
	                ULONG stopBits, ULONG parity);

// how to enable transmitter and receiver interrupts
// WRITE_INTERRUPT_ENABLE(port, IER_RECEIVER_FULL | IER_TRANSMITTER_EMPTY);

// how to disable interrupts
// WRITE_INTERRUPT_ENABLE(port, 0x00);

// how to read the interrupt id
// UCHAR intId;
// intId = READ_INTERRUPT_IDENTIFICATION(port);
// check (intId & IIR_NOT_FIRED) == 0 to be sure that the interrupt has fired
// then switch case the intId:
// case IIR_RECEIVER_ERROR: received some kind of error (invalid in kernel mode)
// case IIR_TRANSMITTER_EMPTY: transmitter buffer is empty (feed one more character)
// case IIR_RECEIVER_FULL: receiver buffer full (drop one character)
// case IIR_STATUS_CHANGE: modem status changed (some asserted something)

