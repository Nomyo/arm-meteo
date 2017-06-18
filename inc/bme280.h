#ifndef BME280_H
#define BME280_H

#include <stm32f4xx.h>

#define CHIP_ID 0x60
#define BME280_I2C_ADDR 0xEE
#define BME280_TEMPERATURE_REG 0xFA
#define BME280_CALIB_REG 0x88
#define BME280_CALIB_REG2 0xE1

struct BME280
{
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;

  uint8_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;

  uint8_t  dig_H1;
  int16_t  dig_H2;
  uint8_t  dig_H3;
  int16_t  dig_H4;
  int16_t  dig_H5;
  int8_t   dig_H6;

  int32_t t_fine;

  long temperature;
  long pressure;
  long humidity;
};

int initBME280(struct BME280 *bme);
void getTemperature(struct BME280 *bme);
void getPressure(struct BME280 *bme);
void getHumidity(struct BME280 *bme);
void calibrateBME280(struct BME280 *bme);
void retrieve_data(struct BME280 *bme);

#endif
