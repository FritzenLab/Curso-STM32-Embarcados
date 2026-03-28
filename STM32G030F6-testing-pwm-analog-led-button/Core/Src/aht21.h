#ifndef AHT21_H
#define AHT21_H

#include "stm32g0xx_hal.h"

#define AHT21_I2C_ADDR     (0x38 << 1)

typedef enum {
    AHT21_STATE_IDLE,
    AHT21_STATE_TRIGGER_SENT,
    AHT21_STATE_WAITING,
    AHT21_STATE_READ_REQUESTED,
    AHT21_STATE_DATA_READY
} AHT21_State_t;

typedef struct {
    float temperature;
    float humidity;
    uint8_t raw[6];
    AHT21_State_t state;
    uint32_t timestamp;
    uint32_t last_error; // to capture the las i2c error
} AHT21_t;

void AHT21_Init(I2C_HandleTypeDef *hi2c, AHT21_t *dev);
void AHT21_StartMeasurement(I2C_HandleTypeDef *hi2c, AHT21_t *dev);
void AHT21_Process(I2C_HandleTypeDef *hi2c, AHT21_t *dev);

#endif
