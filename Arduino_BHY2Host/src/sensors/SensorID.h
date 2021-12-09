#ifndef SENSOR_ID_H_
#define SENSOR_ID_H_

#define NUM_SUPPORTEND_SENSOR           77

enum SensorID {
  SENSOR_ID_ACC_PASS                 = 1,   /* Accelerometer passthrough */
  SENSOR_ID_ACC_RAW                  = 3,   /* Accelerometer uncalibrated */
  SENSOR_ID_ACC                      = 4,   /* Accelerometer corrected */
  SENSOR_ID_ACC_BIAS                 = 5,   /* Accelerometer offset */
  SENSOR_ID_ACC_WU                   = 6,   /* Accelerometer corrected wake up */
  SENSOR_ID_ACC_RAW_WU               = 7,   /* Accelerometer uncalibrated wake up */
  SENSOR_ID_GYRO_PASS                = 10,  /* Gyroscope passthrough */
  SENSOR_ID_GYRO_RAW                 = 12,  /* Gyroscope uncalibrated */
  SENSOR_ID_GYRO                     = 13,  /* Gyroscope corrected */
  SENSOR_ID_GYRO_BIAS                = 14,  /* Gyroscope offset */
  SENSOR_ID_GYRO_WU                  = 15,  /* Gyroscope wake up */
  SENSOR_ID_GYRO_RAW_WU              = 16,  /* Gyroscope uncalibrated wake up */
  SENSOR_ID_MAG_PASS                 = 19,  /* Magnetometer passthrough */
  SENSOR_ID_MAG_RAW                  = 21,  /* Magnetometer uncalibrated */
  SENSOR_ID_MAG                      = 22,  /* Magnetometer corrected */
  SENSOR_ID_MAG_BIAS                 = 23,  /* Magnetometer offset */
  SENSOR_ID_MAG_WU                   = 24,  /* Magnetometer wake up */
  SENSOR_ID_MAG_RAW_WU               = 25,  /* Magnetometer uncalibrated wake up */
  SENSOR_ID_GRA                      = 28,  /* Gravity vector */
  SENSOR_ID_GRA_WU                   = 29,  /* Gravity vector wake up */
  SENSOR_ID_LACC                     = 31,  /* Linear acceleration */
  SENSOR_ID_LACC_WU                  = 32,  /* Linear acceleration wake up */
  SENSOR_ID_RV                       = 34,  /* Rotation vector */
  SENSOR_ID_RV_WU                    = 35,  /* Rotation vector wake up */
  SENSOR_ID_GAMERV                   = 37,  /* Game rotation vector */
  SENSOR_ID_GAMERV_WU                = 38,  /* Game rotation vector wake up */
  SENSOR_ID_GEORV                    = 40,  /* Geo-magnetic rotation vector */
  SENSOR_ID_GEORV_WU                 = 41,  /* Geo-magnetic rotation vector wake up */
  SENSOR_ID_ORI                      = 43,  /* Orientation */
  SENSOR_ID_ORI_WU                   = 44,  /* Orientation wake up */
  SENSOR_ID_TILT_DETECTOR            = 48,  /* Tilt detector */
  SENSOR_ID_STD                      = 50,  /* Step detector */
  SENSOR_ID_STC                      = 52,  /* Step counter */
  SENSOR_ID_STC_WU                   = 53,  /* Step counter wake up */
  SENSOR_ID_SIG                      = 55,  /* Significant motion */
  SENSOR_ID_WAKE_GESTURE             = 57,  /* Wake gesture */
  SENSOR_ID_GLANCE_GESTURE           = 59,  /* Glance gesture */
  SENSOR_ID_PICKUP_GESTURE           = 61,  /* Pickup gesture */
  SENSOR_ID_AR                       = 63,  /* Activity recognition */
  SENSOR_ID_WRIST_TILT_GESTURE       = 67,  /* Wrist tilt gesture */
  SENSOR_ID_DEVICE_ORI               = 69,  /* Device orientation */
  SENSOR_ID_DEVICE_ORI_WU            = 70,  /* Device orientation wake up */
  SENSOR_ID_STATIONARY_DET           = 75,  /* Stationary detect */
  SENSOR_ID_MOTION_DET               = 77,  /* Motion detect */
  SENSOR_ID_ACC_BIAS_WU              = 91,  /* Accelerometer offset wake up */
  SENSOR_ID_GYRO_BIAS_WU             = 92,  /* Gyroscope offset wake up */
  SENSOR_ID_MAG_BIAS_WU              = 93,  /* Magnetometer offset wake up */
  SENSOR_ID_STD_WU                   = 94,  /* Step detector wake up */
  SENSOR_ID_BSEC                     = 115,  /* BSEC 1.x output */
  SENSOR_ID_TEMP                     = 128, /* Temperature */
  SENSOR_ID_BARO                     = 129, /* Barometer */
  SENSOR_ID_HUM                      = 130, /* Humidity */
  SENSOR_ID_GAS                      = 131, /* Gas */
  SENSOR_ID_TEMP_WU                  = 132, /* Temperature wake up */
  SENSOR_ID_BARO_WU                  = 133, /* Barometer wake up */
  SENSOR_ID_HUM_WU                   = 134, /* Humidity wake up */
  SENSOR_ID_GAS_WU                   = 135, /* Gas wake up */
  SENSOR_ID_STC_HW                   = 136, /* Hardware Step counter */
  SENSOR_ID_STD_HW                   = 137, /* Hardware Step detector */
  SENSOR_ID_SIG_HW                   = 138, /* Hardware Significant motion */
  SENSOR_ID_STC_HW_WU                = 139, /* Hardware Step counter wake up */
  SENSOR_ID_STD_HW_WU                = 140, /* Hardware Step detector wake up */
  SENSOR_ID_SIG_HW_WU                = 141, /* Hardware Significant motion wake up */
  SENSOR_ID_ANY_MOTION               = 142, /* Any motion */
  SENSOR_ID_ANY_MOTION_WU            = 143, /* Any motion wake up */
  SENSOR_ID_EXCAMERA                 = 144, /* External camera trigger */
  SENSOR_ID_GPS                      = 145, /* GPS */
  SENSOR_ID_LIGHT                    = 146, /* Light */
  SENSOR_ID_PROX                     = 147, /* Proximity */
  SENSOR_ID_LIGHT_WU                 = 148, /* Light wake up */
  SENSOR_ID_PROX_WU                  = 149, /* Proximity wake up */
  SENSOR_ID_BSEC_LEGACY              = 171, /* BSEC 1.x output (legacy, deprecated) */
  DEBUG_DATA_EVENT                   = 250, /* Binary or string debug data */
  TIMESTAMP_SMALL_DELTA              = 251, /* Incremental time change from previous read */
  TIMESTAMP_SMALL_DELTA_WU           = 245, /* Incremental time change from previous read wake up */
  TIMESTAMP_LARGE_DELTA              = 252, /* Incremental time change from previous read */
  TIMESTAMP_LARGE_DELTA_WU           = 246, /* Incremental time change from previous read wake up */
  TIMESTAMP_FULL                     = 253, /* Incremental time change from previous read */
  TIMESTAMP_FULL_WU                  = 247  /* Incremental time change from previous read wake up */
};

enum SensorPayload {
  PQUATERNION = 0,
  VECTOR3D = 1,
  PEULER = 2,
  P8BITSIGNED = 3,
  P8BITUNISIGNED = 4,
  P16BITSIGNED = 5,
  P16BITUNSIGNED = 6,
  P32BITSIGNED = 7,
  P32BITUNSIGNED = 8,
  P24BITUNSIGNED = 9,
  P40BITUNSIGNED = 10,
  PEVENT = 11,
  ACTIVITY = 12,
  DEBUG_DATA = 13
};

struct SensorStruct
{
	SensorID id;
	SensorPayload payload;
	float scaleFactor;
};

static SensorStruct SensorList[NUM_SUPPORTEND_SENSOR] = {
  {SENSOR_ID_ACC_PASS,            VECTOR3D,           1.0},
  {SENSOR_ID_ACC_RAW,             VECTOR3D,           1.0},
  {SENSOR_ID_ACC,                 VECTOR3D,           1.0},
  {SENSOR_ID_ACC_BIAS,            VECTOR3D,           1.0},
  {SENSOR_ID_ACC_WU,              VECTOR3D,           1.0},
  {SENSOR_ID_ACC_RAW_WU,          VECTOR3D,           1.0},
  {SENSOR_ID_GYRO_PASS,           VECTOR3D,           1.0},
  {SENSOR_ID_GYRO_RAW,            VECTOR3D,           1.0},
  {SENSOR_ID_GYRO,                VECTOR3D,           1.0},
  {SENSOR_ID_GYRO_BIAS,           VECTOR3D,           1.0},
  {SENSOR_ID_GYRO_WU,             VECTOR3D,           1.0},
  {SENSOR_ID_GYRO_RAW_WU,         VECTOR3D,           1.0},
  {SENSOR_ID_MAG_PASS,            VECTOR3D,           1.0},
  {SENSOR_ID_MAG_RAW,             VECTOR3D,           1.0},
  {SENSOR_ID_MAG,                 VECTOR3D,           1.0},
  {SENSOR_ID_MAG_BIAS,            VECTOR3D,           1.0},
  {SENSOR_ID_MAG_WU,              VECTOR3D,           1.0},
  {SENSOR_ID_MAG_RAW_WU,          VECTOR3D,           1.0},
  {SENSOR_ID_GRA,                 VECTOR3D,           1.0},
  {SENSOR_ID_GRA_WU,              VECTOR3D,           1.0},
  {SENSOR_ID_LACC,                VECTOR3D,           1.0},
  {SENSOR_ID_LACC_WU,             VECTOR3D,           1.0},
  {SENSOR_ID_RV,                  PQUATERNION,        1.0},
  {SENSOR_ID_RV_WU,               PQUATERNION,        1.0},
  {SENSOR_ID_GAMERV,              PQUATERNION,        1.0},
  {SENSOR_ID_GAMERV_WU,           PQUATERNION,        1.0},
  {SENSOR_ID_GEORV,               PQUATERNION,        1.0},
  {SENSOR_ID_GEORV_WU,            PQUATERNION,        1.0},
  {SENSOR_ID_ORI,                 PEULER,             0.01098},
  {SENSOR_ID_ORI_WU,              PEULER,             0.01098},
  {SENSOR_ID_TILT_DETECTOR,       PEVENT,             1.0},
  {SENSOR_ID_STD,                 PEVENT,             1.0},
  {SENSOR_ID_STC,                 P32BITUNSIGNED,     1.0},
  {SENSOR_ID_STC_WU,              P32BITUNSIGNED,     1.0},
  {SENSOR_ID_SIG,                 PEVENT,             1.0},
  {SENSOR_ID_WAKE_GESTURE,        PEVENT,             1.0},
  {SENSOR_ID_GLANCE_GESTURE,      PEVENT,             1.0},
  {SENSOR_ID_PICKUP_GESTURE,      PEVENT,             1.0},
  {SENSOR_ID_AR,                  ACTIVITY,           1.0},
  {SENSOR_ID_WRIST_TILT_GESTURE,  PEVENT,             1.0},
  {SENSOR_ID_DEVICE_ORI,          P8BITUNISIGNED,     1.0},
  {SENSOR_ID_DEVICE_ORI_WU,       P8BITUNISIGNED,     1.0},
  {SENSOR_ID_STATIONARY_DET,      PEVENT,             1.0},
  {SENSOR_ID_MOTION_DET,          PEVENT,             1.0},
  {SENSOR_ID_ACC_BIAS_WU,         VECTOR3D,           1.0},
  {SENSOR_ID_GYRO_BIAS_WU,        VECTOR3D,           1.0},
  {SENSOR_ID_MAG_BIAS_WU,         VECTOR3D,           1.0},
  {SENSOR_ID_STD_WU,              PEVENT,             1.0},
  {SENSOR_ID_TEMP,                P16BITSIGNED,       0.01},
  {SENSOR_ID_BARO,                P24BITUNSIGNED,     0.0078},
  {SENSOR_ID_HUM,                 P8BITUNISIGNED,     1.0},
  {SENSOR_ID_GAS,                 P32BITUNSIGNED,     1.0},
  {SENSOR_ID_TEMP_WU,             P16BITSIGNED,       0.01},
  {SENSOR_ID_BARO_WU,             P24BITUNSIGNED,     0.0078},
  {SENSOR_ID_HUM_WU,              P8BITUNISIGNED,     1.0},
  {SENSOR_ID_GAS_WU,              P32BITUNSIGNED,     1.0},
  {SENSOR_ID_STC_HW,              P32BITUNSIGNED,     1.0},
  {SENSOR_ID_STD_HW,              PEVENT,             1.0},
  {SENSOR_ID_SIG_HW,              PEVENT,             1.0},
  {SENSOR_ID_STC_HW_WU,           P32BITUNSIGNED,     1.0},
  {SENSOR_ID_STD_HW_WU,           PEVENT,             1.0},
  {SENSOR_ID_SIG_HW_WU,           PEVENT,             1.0},
  {SENSOR_ID_ANY_MOTION,          PEVENT,             1.0},
  {SENSOR_ID_ANY_MOTION_WU,       PEVENT,             1.0},
  {SENSOR_ID_EXCAMERA,            P8BITUNISIGNED,     1.0},
  {SENSOR_ID_GPS,                 VECTOR3D,           1.0},
  {SENSOR_ID_LIGHT,               P16BITUNSIGNED,     46.296},
  {SENSOR_ID_PROX,                P8BITUNISIGNED,     1.0},
  {SENSOR_ID_LIGHT_WU,            P16BITUNSIGNED,     46.296},
  {SENSOR_ID_PROX_WU,             P8BITUNISIGNED,     1.0},
  {DEBUG_DATA_EVENT,              DEBUG_DATA,         1.0},
  {TIMESTAMP_SMALL_DELTA,         P8BITUNISIGNED,     0.000015625},
  {TIMESTAMP_SMALL_DELTA_WU,      P8BITUNISIGNED,     0.000015625},
  {TIMESTAMP_LARGE_DELTA,         P8BITUNISIGNED,     0.000015625},
  {TIMESTAMP_LARGE_DELTA_WU,      P8BITUNISIGNED,     0.000015625},
  {TIMESTAMP_FULL,                P8BITUNISIGNED,     0.000015625},
  {TIMESTAMP_FULL_WU,             P8BITUNISIGNED,     0.000015625}
};

#endif
