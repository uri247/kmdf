typedef struct _SMPLDEVICE_RINGBUFFER
{
    ULONG MaxSize;
    ULONG FreeSpace;
    ULONG ReadPos;
    ULONG WritePos;
    UCHAR Buffer[1];

} SMPLDEVICE_RINGBUFFER,*PSMPLDEVICE_RINGBUFFER;


PSMPLDEVICE_RINGBUFFER RingbufferInitialize(ULONG BufSize);
VOID RingbufferFree(PSMPLDEVICE_RINGBUFFER pRingBuffer);
PSMPLDEVICE_RINGBUFFER RingbufferInitialize(IN ULONG BufSize);
BOOLEAN RingbufferInsertCharacter(IN OUT PSMPLDEVICE_RINGBUFFER pRingBuffer, IN UCHAR Character);
BOOLEAN RingbufferRemoveCharacter(IN OUT PSMPLDEVICE_RINGBUFFER pRingBuffer, OUT PUCHAR pCharacter);
ULONG RingbufferGetCountCharacters(IN OUT PSMPLDEVICE_RINGBUFFER pRingBuffer);
