///////////////////////////////////////////////////////////////////////////////
// File:          Ringbuffer.c
//
// Description:   This contains the ringbuffer utility routines 
//                for training sessions.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
// KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
// PURPOSE.
//
// Copyright © datronicsoft.  2012
///////////////////////////////////////////////////////////////////////////////

#pragma warning(disable:4200) // zero-sized array

#include "driver.h"

//
// Local function prototypes
//
VOID
InternalReset(
    IN PSMPLDEVICE_RINGBUFFER pRingBuffer
    );

///////////////////////////////////////////////////////////////////////////////
// This routine is merely used to deallocate the
// ringbuffer structure and free all memory.
///////////////////////////////////////////////////////////////////////////////
VOID
RingbufferFree(
    PSMPLDEVICE_RINGBUFFER pRingBuffer
    )
{
    ExFreePoolWithTag(pRingBuffer, 'LPMS');

} // end RingbufferFree

///////////////////////////////////////////////////////////////////////////////
// This routine is used to allocate and initialize a ringbuffer structure.
///////////////////////////////////////////////////////////////////////////////
PSMPLDEVICE_RINGBUFFER
RingbufferInitialize(
    IN ULONG BufSize
    )
{
    PSMPLDEVICE_RINGBUFFER pRingBuffer =
        (PSMPLDEVICE_RINGBUFFER)ExAllocatePoolWithTag(NonPagedPool, BufSize + sizeof(SMPLDEVICE_RINGBUFFER), 'LPMS');

    if(!pRingBuffer) 
        return NULL;

    pRingBuffer->MaxSize = BufSize;

    InternalReset(pRingBuffer);

    return pRingBuffer;

} // end RingbufferInitialize

///////////////////////////////////////////////////////////////////////////////
// This routine puts one character into the specified ringbuffer.
///////////////////////////////////////////////////////////////////////////////
BOOLEAN
RingbufferInsertCharacter(
    IN OUT PSMPLDEVICE_RINGBUFFER pRingBuffer,
    IN UCHAR Character
    )
{
    //
    // This is an attempt to write into a full buffer. This should never happen!
    //
    if ( !(pRingBuffer->FreeSpace > 0) )
        return FALSE;

    //
    // Reserve space for one additional character in the buffer.
    //
    (pRingBuffer->FreeSpace)--;

    //
    // Write character into ringbuffer.
    //
    pRingBuffer->Buffer[pRingBuffer->WritePos]= Character;

    //
    // Adjust write position of ringbuffer.
    //
    pRingBuffer->WritePos = (pRingBuffer->WritePos + 1) % pRingBuffer->MaxSize;

    return TRUE;

} // end RingbufferInsertCharacter

///////////////////////////////////////////////////////////////////////////////
// This routine removes one character from the specified ringbuffer.
///////////////////////////////////////////////////////////////////////////////
BOOLEAN
RingbufferRemoveCharacter(
    IN OUT PSMPLDEVICE_RINGBUFFER pRingBuffer,
    OUT PUCHAR pCharacter
    )
{
    //
    // This is an attempt to read from an empty buffer. This should never happen!
    //
    if ( pRingBuffer->FreeSpace >= pRingBuffer->MaxSize )
        return FALSE;

    //
    // Remove space for one character in the buffer.
    //
    (pRingBuffer->FreeSpace)++;

    //
    // Read character from ringbuffer to caller supplied buffer.
    //
    *pCharacter = pRingBuffer->Buffer[pRingBuffer->ReadPos] ;

    //
    // Adjust read position of ringbuffer.
    //
    pRingBuffer->ReadPos = (pRingBuffer->ReadPos + 1) % pRingBuffer->MaxSize;

    return TRUE;

} // end RingbufferRemoveCharacter

///////////////////////////////////////////////////////////////////////////////
// This routine checks how many characters are available 
// in the ringbuffer.
///////////////////////////////////////////////////////////////////////////////
ULONG
RingbufferGetCountCharacters(
    IN OUT PSMPLDEVICE_RINGBUFFER pRingBuffer
    ) 
{
    return pRingBuffer->MaxSize - pRingBuffer->FreeSpace;

} // end RingbufferGetCountCharacters

///////////////////////////////////////////////////////////////////////////////
// This routine is a local helper function. It resets the ringbuffer to zero length
// and maximum free space. No synchronization is done here. Synchronisation is expected
// to be done by the calling function. The calling functions have to guarantee atomar
// operation!
///////////////////////////////////////////////////////////////////////////////
VOID
InternalReset(
    IN PSMPLDEVICE_RINGBUFFER pRingBuffer
    )
{
    pRingBuffer->FreeSpace = pRingBuffer->MaxSize;
    pRingBuffer->ReadPos = 0;
    pRingBuffer->WritePos = 0;

} // end InternalReset
