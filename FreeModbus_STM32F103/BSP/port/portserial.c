/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"
#include "stm32f10x.h"
#include "bsp_usart.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR(void);  //发送中断处理
static void prvvUARTRxISR(void);  //接收中断处理

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)//适配中断使能
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */

    if (xRxEnable)
    {
        //使能串口接收中断
        USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    }
    else
    {
        USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
    }

    if (xTxEnable)
    {
        //使能串口发送完成中断
        USART_ITConfig(USART1, USART_IT_TC, ENABLE);
    }
    else
    {
        USART_ITConfig(USART1, USART_IT_TC, DISABLE);
    }


}

BOOL
xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)//适配串口初始化
{
    USART_Config(ulBaudRate);
    return TRUE;
}

BOOL
xMBPortSerialPutByte(CHAR ucByte)//适配串口发送函数
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
    USART_SendData(USART1, ucByte);
    return TRUE;
}

BOOL
xMBPortSerialGetByte(CHAR *pucByte)//适配串口接收函数
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    *pucByte = USART_ReceiveData(USART1);
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR(void)
{
    pxMBFrameCBTransmitterEmpty();
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR(void)
{
    pxMBFrameCBByteReceived();
}

//串口中断服务函数
void USART1_IRQHandler(void)//适配中断服务函数
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)//接收中断
    {
        prvvUARTRxISR();
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
    if (USART_GetITStatus(USART1, USART_IT_ORE) == SET) //接收溢出中断
    {
        USART_ClearITPendingBit(USART1, USART_IT_ORE);
        prvvUARTRxISR();
    }
    if (USART_GetITStatus(USART1, USART_IT_TC) == SET) //发送完成中断
    {
        prvvUARTTxReadyISR();
        USART_ClearITPendingBit(USART1, USART_IT_TC);//
    }


}
