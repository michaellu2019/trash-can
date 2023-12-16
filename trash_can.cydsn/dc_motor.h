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
#pragma once
#include "project.h"
#include "math.h"

#define LEFT_DRIVE_DC_MOTOR 0
#define RIGHT_DRIVE_DC_MOTOR 1
#define CENTER_LIFT_DC_MOTOR 2
#define SHOULDER_CAM_DC_MOTOR 3

#define COUNTS_PER_QUADRATURE 4.0
#define LEFT_DRIVE_DC_MOTOR_COUNTS_PER_ROT 8400.0
#define RIGHT_DRIVE_DC_MOTOR_COUNTS_PER_ROT 8400.0
#define CENTER_LIFT_DC_MOTOR_COUNTS_PER_ROT 8400.0
#define SHOULDER_CAM_DC_MOTOR_COUNTS_PER_ROT 8400.0

#define LEFT_DRIVE_DC_MOTOR_MAX_PWM 240
#define RIGHT_DRIVE_DC_MOTOR_MAX_PWM 240
#define CENTER_LIFT_DC_MOTOR_MAX_PWM 240
#define SHOULDER_CAM_DC_MOTOR_MAX_PWM 240

const int NUM_DC_MOTORS;
const float DC_MOTOR_SPEED_TOLERANCE;
const float DC_MOTOR_POS_TOLERANCE;

typedef struct dc_motor {
    uint8 id;
    int16 pwm;
    int16 max_pwm;
    uint16 counts_per_rot;
    int16 enc_count;
    
    float max_pos;
    float min_pos;
    float pos;
    float prev_pos;
    float vel;
    float prev_vel;
    float vel_filt;
} DC_MOTOR;

void init_dc_motors();

void construct_dc_motor(DC_MOTOR *dc_motor, uint8 id, int16 max_pwm, uint16 counts_per_rot, float max_pos, float min_pos);

void set_dc_motor_pwm(DC_MOTOR *dc_motor, int16 pwm);

/* [] END OF FILE */
