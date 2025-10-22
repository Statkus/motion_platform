/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTION_PLATFORM_CONFIGURATION_H__
#define __MOTION_PLATFORM_CONFIGURATION_H__

/* Exported defines ----------------------------------------------------------*/
#define NB_MOTORS 1

#define MESSAGE_SIZE   (NB_MOTORS * 2) + 4
#define MESSAGE_PERIOD 10000 // us

#define ROLL_PITCH_HEAVE_MOTORS_MIN_POS 0
#define ROLL_PITCH_HEAVE_MOTORS_MAX_POS 64000

#define M1_POSITION_ZERO_CENTERED 1
#define M1_MIN_POSITION           0
#define M1_MAX_POSITION           0
#define M1_NEEDS_CALIBRATION      1

#define MAX_PWM 250

#endif /* __MOTION_PLATFORM_CONFIGURATION_H__ */
