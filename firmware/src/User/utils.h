#ifndef __UTILS_H__
#define __UTILS_H__
#include <stdint.h>


typedef enum
{
  USB_OK = 0,
  USB_FAIL,
} USB_Status;

typedef struct
{
  uint8_t  *buf;
  uint16_t  head;
  uint16_t tail;
  uint16_t size;
  uint8_t  lock;
  uint8_t  buffArr[128];
} FIFO_Utils_TypeDef;

void FifoInit(FIFO_Utils_TypeDef *f);
uint16_t FifoWrite(FIFO_Utils_TypeDef *f, void *buf, uint16_t  nbytes);
uint16_t FifoRead(FIFO_Utils_TypeDef *f, void *buf, uint16_t nbytes);
uint16_t collect_bits(uint8_t *p, uint16_t offset, uint8_t size, int is_signed);

#endif
