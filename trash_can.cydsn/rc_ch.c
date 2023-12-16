/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include "milliseconds.h"
#include "rc_ch.h"

const int RC_TIMER_PERIOD = 500000 - 1;
const int RC_TIMER_MAX_VALUE = 45000;
const int RC_TIMER_MID_VALUE = 36000;
const int RC_TIMER_MIN_VALUE = 23500;
const int RC_TIMER_MID_TOLERANCE = 600;
const int RC_CONNECTION_TIMEOUT = 70;

const int RC_MAX_VALUE = 240;
const int RC_MIN_VALUE = 0;

const int RC_CH1_MIN_VALUE = 25;
const int RC_CH5_THRESHOLD_VALUE = 15;
const int RC_CONVERTING_THRESHOLD_VALUE = 100;

static uint8 isr_rc_ch1_timer_int_flag;
static uint8 isr_rc_ch2_timer_int_flag;
static uint8 isr_rc_ch3_timer_int_flag;
static uint8 isr_rc_ch4_timer_int_flag;
static uint8 isr_rc_ch5_timer_int_flag;
static volatile uint32 isr_rc_ch1_timer_count;
static volatile uint32 isr_rc_ch2_timer_count;
static volatile uint32 isr_rc_ch3_timer_count;
static volatile uint32 isr_rc_ch4_timer_count;
static volatile uint32 isr_rc_ch5_timer_count;

// interrupt service routines to read the RC timer registers 
// and count the number of high values
CY_ISR(RC_Ch1_Timer_ISR_Handler)
{
    isr_rc_ch1_timer_int_flag = RC_Ch1_Timer_ReadStatusRegister();    
    if (isr_rc_ch1_timer_int_flag & RC_Ch1_Timer_STATUS_CAPTURE)
        isr_rc_ch1_timer_count = RC_TIMER_PERIOD - RC_Ch1_Timer_ReadCapture();    
    if (isr_rc_ch1_timer_int_flag & RC_Ch1_Timer_STATUS_TC)
        isr_rc_ch1_timer_count = 0;
    
    RC_Ch1_Timer_Stop();
    RC_Ch1_Timer_WriteCounter(RC_TIMER_PERIOD);
}

CY_ISR(RC_Ch2_Timer_ISR_Handler)
{
    isr_rc_ch2_timer_int_flag = RC_Ch2_Timer_ReadStatusRegister();    
    if (isr_rc_ch2_timer_int_flag & RC_Ch2_Timer_STATUS_CAPTURE)
        isr_rc_ch2_timer_count = RC_TIMER_PERIOD - RC_Ch2_Timer_ReadCapture();    
    if (isr_rc_ch2_timer_int_flag & RC_Ch2_Timer_STATUS_TC)
        isr_rc_ch2_timer_count = 0;
    
    RC_Ch2_Timer_Stop();
    RC_Ch2_Timer_WriteCounter(RC_TIMER_PERIOD);
}

CY_ISR(RC_Ch3_Timer_ISR_Handler)
{
    isr_rc_ch3_timer_int_flag = RC_Ch3_Timer_ReadStatusRegister();    
    if (isr_rc_ch3_timer_int_flag & RC_Ch3_Timer_STATUS_CAPTURE)
        isr_rc_ch3_timer_count = RC_TIMER_PERIOD - RC_Ch3_Timer_ReadCapture();    
    if (isr_rc_ch3_timer_int_flag & RC_Ch3_Timer_STATUS_TC)
        isr_rc_ch3_timer_count = 0;
    
    RC_Ch3_Timer_Stop();
    RC_Ch3_Timer_WriteCounter(RC_TIMER_PERIOD);
}

CY_ISR(RC_Ch4_Timer_ISR_Handler)
{
    isr_rc_ch4_timer_int_flag = RC_Ch4_Timer_ReadStatusRegister();    
    if (isr_rc_ch4_timer_int_flag & RC_Ch4_Timer_STATUS_CAPTURE)
        isr_rc_ch4_timer_count = RC_TIMER_PERIOD - RC_Ch4_Timer_ReadCapture();    
    if (isr_rc_ch4_timer_int_flag & RC_Ch4_Timer_STATUS_TC)
        isr_rc_ch4_timer_count = 0;
    
    RC_Ch4_Timer_Stop();
    RC_Ch4_Timer_WriteCounter(RC_TIMER_PERIOD);
}

CY_ISR(RC_Ch5_Timer_ISR_Handler)
{
    isr_rc_ch5_timer_int_flag = RC_Ch5_Timer_ReadStatusRegister();    
    if (isr_rc_ch5_timer_int_flag & RC_Ch5_Timer_STATUS_CAPTURE)
        isr_rc_ch5_timer_count = RC_TIMER_PERIOD - RC_Ch5_Timer_ReadCapture();    
    if (isr_rc_ch5_timer_int_flag & RC_Ch5_Timer_STATUS_TC)
        isr_rc_ch5_timer_count = 0;
    
    RC_Ch5_Timer_Stop();
    RC_Ch5_Timer_WriteCounter(RC_TIMER_PERIOD);
}

// initialize all PSoC components for reading RC channels
void init_rc_channels() {
    RC_Ch1_Timer_Interrupt_StartEx(RC_Ch1_Timer_ISR_Handler);
    RC_Ch1_Timer_Start();
    RC_Ch2_Timer_Interrupt_StartEx(RC_Ch2_Timer_ISR_Handler);
    RC_Ch2_Timer_Start();
    RC_Ch3_Timer_Interrupt_StartEx(RC_Ch3_Timer_ISR_Handler);
    RC_Ch3_Timer_Start();
    RC_Ch4_Timer_Interrupt_StartEx(RC_Ch4_Timer_ISR_Handler);
    RC_Ch4_Timer_Start();
    RC_Ch5_Timer_Interrupt_StartEx(RC_Ch5_Timer_ISR_Handler);
    RC_Ch5_Timer_Start();
}

// construct an RC channel struct object
void construct_rc_ch(RC_CH* rc_ch, uint8 ch_number) {
    rc_ch->ch_number = ch_number;
    rc_ch->connected = 0;
    rc_ch->value = 0;
    rc_ch->last_connected = 0;
    rc_ch->last_timer_count = 0;
}

// get the duty cycle count of an RC channel
void get_rc_ch_value(RC_CH* rc_ch) {
    uint8 isr_rc_timer_int_flag = rc_ch->ch_number == 1 ? isr_rc_ch1_timer_int_flag :
                                  rc_ch->ch_number == 2 ? isr_rc_ch2_timer_int_flag :
                                  rc_ch->ch_number == 3 ? isr_rc_ch3_timer_int_flag :
                                  rc_ch->ch_number == 4 ? isr_rc_ch4_timer_int_flag :
                                                          isr_rc_ch5_timer_int_flag;
    uint32 isr_rc_timer_count = rc_ch->ch_number == 1 ? isr_rc_ch1_timer_count :
                                rc_ch->ch_number == 2 ? isr_rc_ch2_timer_count :
                                rc_ch->ch_number == 3 ? isr_rc_ch3_timer_count :
                                rc_ch->ch_number == 4 ? isr_rc_ch4_timer_count :
                                                        isr_rc_ch5_timer_count;
    
    int16 ch_value = 0;
    uint8 rc_ch_read_value = rc_ch->ch_number == 1 ? RC_Ch1_Read() :
                             rc_ch->ch_number == 2 ? RC_Ch2_Read() :
                             rc_ch->ch_number == 3 ? RC_Ch3_Read() :
                             rc_ch->ch_number == 4 ? RC_Ch4_Read() :
                                                     RC_Ch5_Read();
    
    // failsafe so that if the rc channel is disconnected, i.e. no value for a while
    // shut off the channel so the motor stops running and nobody dies
    if (rc_ch_read_value) {
        rc_ch->last_connected = MILLISECONDS;
        rc_ch->connected = 1;
    } else {
        if (MILLISECONDS - rc_ch->last_connected > RC_CONNECTION_TIMEOUT) {
            rc_ch->connected = 0;
        }
    }
    
    // if the RC  channel is connected, remap the duty cycle count value to an integer between -255 and 255
    if (rc_ch->connected && isr_rc_timer_int_flag) {
        uint32 rc_ch_timer_count = (isr_rc_timer_count < RC_TIMER_MIN_VALUE || 
                                    isr_rc_timer_count > RC_TIMER_MAX_VALUE) ? 
                                    rc_ch->last_timer_count : isr_rc_timer_count;
        
        if (abs(RC_TIMER_MID_VALUE - rc_ch_timer_count) < RC_TIMER_MID_TOLERANCE) {
            rc_ch->value = RC_MIN_VALUE;
        } else {
            const int RC_TIMER_AMPLITUDE = rc_ch_timer_count > RC_TIMER_MID_VALUE ? RC_TIMER_MAX_VALUE - RC_TIMER_MID_VALUE : RC_TIMER_MID_VALUE - RC_TIMER_MIN_VALUE;
            rc_ch->value = (RC_MAX_VALUE - RC_MIN_VALUE) * ((int32) rc_ch_timer_count - RC_TIMER_MID_VALUE)/RC_TIMER_AMPLITUDE;
        }
        
        /*char debug[64] = "";
        sprintf(debug, "[%ld] %d: %d --> %d", MILLISECONDS, rc_ch->ch_number, rc_ch_timer_count, rc_ch->value);
        
        sprintf(debug, "%s\r\n", debug);
        USBUART_PutString(debug);*/
        
        if (rc_ch->value < -RC_MAX_VALUE)
            rc_ch->value = -RC_MAX_VALUE;
        if (rc_ch->value > RC_MAX_VALUE)
            rc_ch->value = RC_MAX_VALUE;
        
        rc_ch->last_timer_count = rc_ch_timer_count;
        isr_rc_timer_int_flag = 0;
        
        if (rc_ch->ch_number == 1)
            RC_Ch1_Timer_Start();
        if (rc_ch->ch_number == 2)
            RC_Ch2_Timer_Start();
        if (rc_ch->ch_number == 3)
            RC_Ch3_Timer_Start();
        if (rc_ch->ch_number == 4)
            RC_Ch4_Timer_Start();
        if (rc_ch->ch_number == 5)
            RC_Ch5_Timer_Start();
    }
}

/* [] END OF FILE */
