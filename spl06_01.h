#ifndef SPL06_01_H
#define SPL06_01_H

#define CONTINUOUS_PRESSURE     1
#define CONTINUOUS_TEMPERATURE  2
#define CONTINUOUS_P_AND_T      3
#define PRESSURE_SENSOR     0
#define TEMPERATURE_SENSOR  1

#include <stdint.h>

struct spl0601_calib_param_t {	
    int16_t c0;
    int16_t c1;
    int32_t c00;
    int32_t c10;
    int16_t c01;
    int16_t c11;
    int16_t c20;
    int16_t c21;
    int16_t c30;       
};

typedef struct spl0601_t {	
    struct spl0601_calib_param_t calib_param;/**<calibration data*/	
    uint8_t chip_id; /**<chip id*/	
    int32_t i32rawPressure;
    int32_t i32rawTemperature;
    int32_t i32kP;    
    int32_t i32kT;
    struct rt_i2c_bus_device *bus;
} spl0601_t;


int spl0601_init(spl0601_t *hdev, const char *busname);
void spl0601_rateset(spl0601_t *hdev, uint8_t iSensor, uint8_t u8OverSmpl, uint8_t u8SmplRate);
void spl0601_start_temperature(spl0601_t *hdev);
void spl0601_start_pressure(spl0601_t *hdev);
void spl0601_start_continuous(spl0601_t *hdev, uint8_t mode);
void spl0601_get_raw_temp(spl0601_t *hdev);
void spl0601_get_raw_pressure(spl0601_t *hdev);
float spl0601_get_temperature(spl0601_t *hdev);
float spl0601_get_pressure(spl0601_t *hdev);

#endif

