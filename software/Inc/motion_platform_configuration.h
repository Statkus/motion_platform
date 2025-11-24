/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOTION_PLATFORM_CONFIGURATION_H__
#define __MOTION_PLATFORM_CONFIGURATION_H__

/* Exported defines ----------------------------------------------------------*/
#define NB_MOTORS 6

#define MESSAGE_SIZE      (NB_MOTORS * 2) + 4
#define MESSAGE_PERIOD_US 10000

// Motor 1
#define M1_POSITION_ZERO_CENTERED 0
#define M1_MIN_POSITION           0
#define M1_MAX_POSITION           64000
#define M1_NEEDS_CALIBRATION      0

// Motor 2
#define M2_POSITION_ZERO_CENTERED 0
#define M2_MIN_POSITION           0
#define M2_MAX_POSITION           64000
#define M2_NEEDS_CALIBRATION      0

// Motor 3
#define M3_POSITION_ZERO_CENTERED 0
#define M3_MIN_POSITION           0
#define M3_MAX_POSITION           64000
#define M3_NEEDS_CALIBRATION      0

// Motor 4
#define M4_POSITION_ZERO_CENTERED 0
#define M4_MIN_POSITION           0
#define M4_MAX_POSITION           64000
#define M4_NEEDS_CALIBRATION      0

// Motor 5
#define M5_POSITION_ZERO_CENTERED 1
#define M5_MIN_POSITION           0
#define M5_MAX_POSITION           0
#define M5_NEEDS_CALIBRATION      1

// Motor 6
#define M6_POSITION_ZERO_CENTERED 1
#define M6_MIN_POSITION           0
#define M6_MAX_POSITION           0
#define M6_NEEDS_CALIBRATION      1

#define MAX_PWM 250

#endif /* __MOTION_PLATFORM_CONFIGURATION_H__ */
