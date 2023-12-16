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
#include "dc_motor.h"

const int NUM_DC_MOTORS = 3;
const float DC_MOTOR_SPEED_TOLERANCE = 0.0;
const float DC_MOTOR_POS_TOLERANCE = 2.0;

// initialize all PSoC components for DC motors
void init_dc_motors() {
    Left_Drive_DC_Motor_PWM_Start();
    Right_Drive_DC_Motor_PWM_Start();
}

// build a DC motor struct object with the specified settings
void construct_dc_motor(DC_MOTOR *dc_motor, uint8 id, int16 max_pwm, uint16 counts_per_rot, float max_pos, float min_pos) {
    dc_motor->id = id;
    dc_motor->max_pwm = max_pwm;
    dc_motor->pwm = 0;
    dc_motor->counts_per_rot = counts_per_rot;
    dc_motor->enc_count = 0;
    dc_motor->max_pos = max_pos;
    dc_motor->min_pos = min_pos;
    dc_motor->pos = 0;
    dc_motor->prev_pos = 0;
    dc_motor->vel = 0;
    dc_motor->prev_vel = 0;
    dc_motor->vel_filt = 0;
}

// drive the DC motor with a specific PWM
void set_dc_motor_pwm(DC_MOTOR *dc_motor, int16 pwm) {
    // if the PWM value provided is outside of the max PWM value of the motor, clip it
    if (abs(pwm) > dc_motor->max_pwm) {
        if (pwm < 0) 
            dc_motor->pwm = -dc_motor->max_pwm;
        if (pwm > 0) 
            dc_motor->pwm = dc_motor->max_pwm;
    } else {
        dc_motor->pwm = pwm;
    }
    
    // write the PWM value to the appropriate pins for the correct DC motor, based on the DBH-12V motor driver specs
    if (dc_motor->id == LEFT_DRIVE_DC_MOTOR) {
        if (pwm < 0) {
            Left_Drive_DC_Motor_PWM_WriteCompare(abs(dc_motor->pwm));
            Left_Drive_DC_Motor_Direction_Write(1);
        } else if (pwm > 0) {
            Left_Drive_DC_Motor_PWM_WriteCompare(abs(dc_motor->pwm));
            Left_Drive_DC_Motor_Direction_Write(0);
        } else {
            Left_Drive_DC_Motor_PWM_WriteCompare(0);
            Left_Drive_DC_Motor_Direction_Write(0);
        }
    }
    
    // write the PWM value to the appropriate pins for the correct DC motor, based on the DBH-12V motor driver specs
    if (dc_motor->id == RIGHT_DRIVE_DC_MOTOR) {
        if (pwm < 0) {
            Right_Drive_DC_Motor_PWM_WriteCompare(abs(dc_motor->pwm));
            Right_Drive_DC_Motor_Direction_Write(1);
        } else if (pwm > 0) {
            Right_Drive_DC_Motor_PWM_WriteCompare(abs(dc_motor->pwm));
            Right_Drive_DC_Motor_Direction_Write(0);
        } else {
            Right_Drive_DC_Motor_PWM_WriteCompare(0);
            Right_Drive_DC_Motor_Direction_Write(0);
        }
    }
    
    if (dc_motor->id == CENTER_LIFT_DC_MOTOR) {
        if (pwm < 0) {
            Center_Lift_DC_Motor_Direction_1_Write(1);
            Center_Lift_DC_Motor_Direction_2_Write(0);
        } else if (pwm > 0) {
            Center_Lift_DC_Motor_Direction_1_Write(0);
            Center_Lift_DC_Motor_Direction_2_Write(1);
        } else {
            Center_Lift_DC_Motor_Direction_1_Write(1);
            Center_Lift_DC_Motor_Direction_2_Write(1);
        }
    }
    
    if (dc_motor->id == SHOULDER_CAM_DC_MOTOR) {
        if (pwm < 0) {
            Shoulder_Cam_DC_Motor_Direction_1_Write(0);
            Shoulder_Cam_DC_Motor_Direction_2_Write(1);
        } else if (pwm > 0) {
            Shoulder_Cam_DC_Motor_Direction_1_Write(1);
            Shoulder_Cam_DC_Motor_Direction_2_Write(0);
        } else {
            Shoulder_Cam_DC_Motor_Direction_1_Write(1);
            Shoulder_Cam_DC_Motor_Direction_2_Write(1);
        }
    }
}

/* [] END OF FILE */
