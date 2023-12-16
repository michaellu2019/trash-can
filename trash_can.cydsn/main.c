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
#include "dc_motor.h"
#include "servo.h"

// flag to enable UART debugging, must set to 0 if not using 
// or else code will not run without being plugged into computer via USB
#define DEBUG_PROGRAM 1
char debug_data_buf[64] = "";

/*
 * Channel 1: Right Joystick X
 * Channel 2: Right Joystick Y
 * Channel 3: Left Joystick Y
 * Channel 4: Left Joystick X
 * Channel 5: Right Trigger Button
 * Channel 6: Right Bumper Button
*/

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    
    // configure USB connection for UART debugging
    #if DEBUG_PROGRAM
    USBUART_Start(0, USBUART_5V_OPERATION);
    while (USBUART_GetConfiguration() == 0) {}
    USBUART_PutString("Init...");
    #endif
    
    // start the program milliseconds timer
    init_milliseconds();
    uint32 prev_time_ms = MILLISECONDS;
    uint32 prev_log_time_ms = MILLISECONDS;
    
    // turn off the blue status LED
    Status_LED_Write(0);
    
    init_servos();
    SERVO head_servo;
    construct_servo(&head_servo, HEAD_SERVO, 90);
    
    init_dc_motors();
    DC_MOTOR left_drive_dc_motor;
    construct_dc_motor(&left_drive_dc_motor, LEFT_DRIVE_DC_MOTOR, LEFT_DRIVE_DC_MOTOR_MAX_PWM, LEFT_DRIVE_DC_MOTOR_COUNTS_PER_ROT, MAXFLOAT, -MAXFLOAT);
    
    DC_MOTOR right_drive_dc_motor;
    construct_dc_motor(&right_drive_dc_motor, RIGHT_DRIVE_DC_MOTOR, RIGHT_DRIVE_DC_MOTOR_MAX_PWM, RIGHT_DRIVE_DC_MOTOR_COUNTS_PER_ROT, MAXFLOAT, -MAXFLOAT);
    
    DC_MOTOR center_lift_dc_motor;
    construct_dc_motor(&center_lift_dc_motor, CENTER_LIFT_DC_MOTOR, CENTER_LIFT_DC_MOTOR_MAX_PWM, CENTER_LIFT_DC_MOTOR_COUNTS_PER_ROT, MAXFLOAT, -MAXFLOAT);
    
    DC_MOTOR shoulder_cam_dc_motor;
    construct_dc_motor(&shoulder_cam_dc_motor, SHOULDER_CAM_DC_MOTOR, SHOULDER_CAM_DC_MOTOR_MAX_PWM, SHOULDER_CAM_DC_MOTOR_COUNTS_PER_ROT, MAXFLOAT, -MAXFLOAT);
    
    // construct all RC channel readers to get remote joystick values
    init_rc_channels();
    RC_CH rc_ch1;
    construct_rc_ch(&rc_ch1, 1);
    RC_CH rc_ch2;
    construct_rc_ch(&rc_ch2, 2);
    RC_CH rc_ch3;
    construct_rc_ch(&rc_ch3, 3);
    RC_CH rc_ch4;
    construct_rc_ch(&rc_ch4, 4);
    RC_CH rc_ch5;
    construct_rc_ch(&rc_ch5, 5);
    
    float start_time = MILLISECONDS;
    
    int converting_mode = 0;

    for(;;)
    {   
        // get RC remote values
        get_rc_ch_value(&rc_ch1);
        get_rc_ch_value(&rc_ch2);
        get_rc_ch_value(&rc_ch3);
        get_rc_ch_value(&rc_ch4);
        get_rc_ch_value(&rc_ch5);
        
        converting_mode = rc_ch5.value < RC_CH5_THRESHOLD_VALUE && abs(abs(rc_ch5.value) - abs(RC_CH5_THRESHOLD_VALUE)) < abs((int) RC_CH5_THRESHOLD_VALUE) * 2;
        Status_LED_Write(converting_mode);
        
        if (!converting_mode) {
            set_dc_motor_pwm(&center_lift_dc_motor, 0);
            set_dc_motor_pwm(&shoulder_cam_dc_motor, 0);
                
            // Left Drive Motor
            if (rc_ch3.connected && rc_ch3.value != 0) {
                // if the RC channel 3 joystick is moved, drive the motor with a proportional PWM value
                set_dc_motor_pwm(&left_drive_dc_motor, rc_ch3.value);
            } else {
                set_dc_motor_pwm(&left_drive_dc_motor, 0);
            }
            
            // Right Drive DC Motor
            if (rc_ch2.connected && rc_ch2.value != 0) {
                // if the RC channel 2 joystick is moved, drive the motor with a proportional PWM value
                set_dc_motor_pwm(&right_drive_dc_motor, rc_ch2.value);
            } else {
                set_dc_motor_pwm(&right_drive_dc_motor, 0);
            }
            
            // Pendulum Channel/Motor
            if (rc_ch4.connected && rc_ch4.value != 0) {
                uint8 head_servo_pos = (rc_ch4.value * 50)/(RC_MAX_VALUE) + 90;
                set_servo_pos(&head_servo, head_servo_pos);
            } else {
                set_servo_pos(&head_servo, 90);
            }
            
            if (rc_ch1.connected && rc_ch1.value != 0) {
                
            } else {
                
            }
        } else {
            // Center Lift Motor
            if (rc_ch3.connected && abs(rc_ch3.value) > RC_CONVERTING_THRESHOLD_VALUE) {
                set_dc_motor_pwm(&center_lift_dc_motor, rc_ch3.value);
            } else {
                set_dc_motor_pwm(&center_lift_dc_motor, 0);
            }
            
            // Shoulder Cam Motor
            if (rc_ch2.connected && abs(rc_ch2.value) > RC_CONVERTING_THRESHOLD_VALUE) {
                set_dc_motor_pwm(&shoulder_cam_dc_motor, rc_ch2.value);
            } else {
                set_dc_motor_pwm(&shoulder_cam_dc_motor, 0);
            }
        }

        #if DEBUG_PROGRAM
            char debug[64] = "";
            sprintf(debug, "[%ld] %d %d %d %d %d", MILLISECONDS, rc_ch1.value, rc_ch2.value, rc_ch3.value, rc_ch4.value, rc_ch5.value);
            sprintf(debug, "%s - %d %d %d", debug, (int) (left_drive_dc_motor.pwm * 1), (int) (right_drive_dc_motor.pwm * 1), (int) (center_lift_dc_motor.pwm * 1));
            
            sprintf(debug, "%s\r\n", debug);
            USBUART_PutString(debug);
        #endif
    }
}

/* [] END OF FILE */
