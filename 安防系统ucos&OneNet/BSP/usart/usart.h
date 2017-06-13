#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"

#define USARTRECV_MAXLEN	1024

typedef struct{
	char buf[USARTRECV_MAXLEN];
	u16  len;
    u8   flag;
}UsartRecv_t;

extern UsartRecv_t  Usart1Recv;
extern UsartRecv_t  Usart2Recv;
extern UsartRecv_t  Usart3Recv;

extern void usart1_init(u32 bound);
extern void usart2_init(u32 bound);
extern void usart3_init(u32 bound);

extern void USART_OUT(USART_TypeDef* USARTx, char *Data,...);
extern void usart2_write(USART_TypeDef* USARTx, uint8_t *Data,uint32_t len);
#endif
