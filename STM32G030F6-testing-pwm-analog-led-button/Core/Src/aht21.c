/*
 * aht21.c
 *
 *  Created on: 27 de mar. de 2026
 *      Author: Clovisf
 */
#include "aht21.h"

static void AHT21_Convert(AHT21_t *dev);

// ------------------------------------------------------------
// Initialize sensor (non-blocking)
// ------------------------------------------------------------
void AHT21_Init(I2C_HandleTypeDef *hi2c, AHT21_t *dev)
{
    uint8_t init_cmd[3] = {0xBE, 0x08, 0x00};
    dev->state = AHT21_STATE_IDLE;

    HAL_I2C_Master_Transmit_IT(hi2c, AHT21_I2C_ADDR, init_cmd, 3);
}

// ------------------------------------------------------------
// Start measurement (non-blocking)
// ------------------------------------------------------------
void AHT21_StartMeasurement(I2C_HandleTypeDef *hi2c, AHT21_t *dev)
{
    if (dev->state != AHT21_STATE_IDLE)
        return;

    uint8_t trig_cmd[3] = {0xAC, 0x33, 0x00};

    HAL_I2C_Master_Transmit_IT(hi2c, AHT21_I2C_ADDR, trig_cmd, 3);

    dev->state = AHT21_STATE_TRIGGER_SENT;
    dev->timestamp = HAL_GetTick();
}

// ------------------------------------------------------------
// Called periodically from main loop or timer
// ------------------------------------------------------------
void AHT21_Process(I2C_HandleTypeDef *hi2c, AHT21_t *dev)
{
    switch (dev->state)
    {
        case AHT21_STATE_TRIGGER_SENT:
            if (HAL_GetTick() - dev->timestamp >= 120)
            {
                HAL_I2C_Master_Receive_IT(hi2c, AHT21_I2C_ADDR, dev->raw, 6);
                dev->state = AHT21_STATE_READ_REQUESTED;
            }
            break;

        case AHT21_STATE_DATA_READY:
            AHT21_Convert(dev);
            dev->state = AHT21_STATE_IDLE;
            break;
        case AHT21_STATE_READ_REQUESTED:
            if (HAL_GetTick() - dev->timestamp > 200) {
                // Sensor didn't respond — reset state machine
                dev->state = AHT21_STATE_IDLE;
            }
            break;

        default:
            break;
    }
}

// ------------------------------------------------------------
// Convert raw bytes to temperature & humidity
// ------------------------------------------------------------
static void AHT21_Convert(AHT21_t *dev)
{
    uint8_t *raw = dev->raw;

    uint32_t hum_raw = ((uint32_t)raw[1] << 12) |
                       ((uint32_t)raw[2] << 4)  |
                       ((raw[3] & 0xF0) >> 4);

    uint32_t temp_raw = ((uint32_t)(raw[3] & 0x0F) << 16) |
                        ((uint32_t)raw[4] << 8) |
                        raw[5];

    // The number 1048576.0 is equal to 2^20. So it represents
    // the maximum number achievable in a 20-bit scale.
    // Since both hum_raw and temp_raw are 20-bit, this is the
    // way of converting it to decimal.
    dev->humidity = ((float)hum_raw / 1048576.0f) * 100.0f;
    dev->temperature = ((float)temp_raw / 1048576.0f) * 200.0f - 50.0f;
}

// ------------------------------------------------------------
// I2C interrupt callbacks
// ------------------------------------------------------------
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    // Nothing needed here for now
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    extern AHT21_t aht21;   // declared in main.c
    aht21.state = AHT21_STATE_DATA_READY;
}
