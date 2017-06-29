#include <i2c1.h>
#include <bme280.h>
#include <uart.h>

void calibrateBME280(struct BME280 *bme)
{
  uint8_t data[18];
  int i = 0;

  // Humidity CTRL
  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0xf2/* BME280_CALIB_REG */);
  I2C_write(I2C1, 0x01);
  I2C_stop(I2C1);

    // Humidity CTRL
  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0xf4/* BME280_CALIB_REG */);
  I2C_write(I2C1, 0x27);
  I2C_stop(I2C1);

    // Humidity CTRL
  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0xf5/* BME280_CALIB_REG */);
  I2C_write(I2C1, 0xa0);
  I2C_stop(I2C1);

  // Calibrate
  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(I2C1, BME280_CALIB_REG);
  I2C_stop(I2C1);

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Receiver);

  for (; i < 5; ++i)
    data[i] = I2C_read_ack(I2C1);
  data[i] = I2C_read_nack(I2C1);

  bme->dig_T1 = (data[1] << 8) | data[0];
  bme->dig_T2 = (data[3] << 8) | data[2];
  bme->dig_T3 = (data[5] << 8) | data[4];

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0x8e);
  I2C_stop(I2C1);

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Receiver);

  for (i = 0; i < 16; ++i)
    data[i] = I2C_read_ack(I2C1);
  data[i] = I2C_read_nack(I2C1);

  bme->dig_P1 = (data[1] << 8) | data[0];
  bme->dig_P2 = (data[3] << 8) | data[2];
  bme->dig_P3 = (data[5] << 8) | data[4];
  bme->dig_P4 = (data[7] << 8) | data[6];
  bme->dig_P5 = (data[9] << 8) | data[8];
  bme->dig_P6 = (data[11] << 8) | data[10];
  bme->dig_P7 = (data[13] << 8) | data[12];
  bme->dig_P8 = (data[15] << 8) | data[14];
  bme->dig_P9 = (data[17] << 8) | data[16];

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0xa1);
  I2C_stop(I2C1);
  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Receiver);

  uint8_t rdata = I2C_read_ack(I2C1);
  data[0] = I2C_read_nack(I2C1);

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0xe1);
  I2C_stop(I2C1);

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Receiver);

  for (i = 1; i < 6; ++i)
    data[i] = I2C_read_ack(I2C1);
  data[i] = I2C_read_nack(I2C1);

  bme->dig_H1 = data[0];
  bme->dig_H2 = (data[2] << 8) | data[1];
  bme->dig_H3 = data[3];
  bme->dig_H4 = (data[4] << 4) | ((data[5]) & 0x0f);
  bme->dig_H5 = (data[6] << 8) | ((data[5] >> 4) & 0x0f);
  bme->dig_H6 = data[7];
}

void getTemperature(struct BME280 *bme)
{
  int32_t var1 = 0;
  int32_t var2 = 0;
  char data[3];
  int i = 0;
  int32_t t_raw = 0;
  int32_t temperature = 0;

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(I2C1, BME280_TEMPERATURE_REG);
  I2C_stop(I2C1);

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Receiver);

  for (; i < 2; ++i)
    data[i] = I2C_read_ack(I2C1);
  data[i] = I2C_read_nack(I2C1);

  t_raw = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);

  var1 = ((((t_raw >> 3) - (bme->dig_T1 << 1))) * (bme->dig_T2)) >> 11;
  var2 = (((((t_raw >> 4) -
  	     (bme->dig_T1)) * ((t_raw >> 4) - (bme->dig_T1))) >> 12)
  	  * (bme->dig_T3)) >> 14;

  bme->t_fine = var1 + var2;

  temperature = (bme->t_fine * 5 + 128) >> 8;
  bme->temperature = (long)temperature / 100;
}

void getPressure(struct BME280 *bme)
{
  int32_t var1 = 0;
  int32_t var2 = 0;
  uint32_t press = 0;
  uint32_t adc_P = 0;
  uint8_t data[3];
  int i = 0;

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0xF7);
  I2C_stop(I2C1);

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Receiver);

  for (; i < 2; ++i)
    data[i] = I2C_read_ack(I2C1);
  data[i] = I2C_read_nack(I2C1);
  adc_P = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);

  var1 = (bme->t_fine >> 1) - 64000;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * bme->dig_P6;
  var2 = var2 + ((var1 * bme->dig_P5) << 1);
  var2 = (var2 >> 2) + (bme->dig_P4 << 16);
  var1 = (((bme->dig_P3 * (((var1 >> 2)*(var1 >> 2)) >> 13)) >> 3)
	  + ((bme->dig_P2 * var1) >> 1)) >> 18;
  var1 = ((32768 + var1) * bme->dig_P1) >> 15;

  if (var1 == 0) {
    return ;
  }

  press = (((1048576 - adc_P) - (var2 >> 12))) * 3125;
  if(press < 0x80000000) {
    press = (press << 1) / var1;
  } else {
    press = (press / var1) * 2;
  }
  var1 = ((int32_t)bme->dig_P9 * ((int32_t)(((press >> 3) * (press >> 3)) >> 13))) >> 12;
  var2 = (((int32_t)(press >> 2)) * (int32_t)bme->dig_P8) >> 13;
  press = (press + ((var1 + var2 + bme->dig_P7) >> 4));

  bme->pressure = (long)press / 100;
}

void getHumidity(struct BME280 *bme)
{
  int32_t adc_H;
  char data[2];
  int i = 0;
  long th;

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0xfd);
  I2C_stop(I2C1);

  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Receiver);

  for (; i < 1; i++)
    data[i] = I2C_read_ack(I2C1);
  data[i] = I2C_read_nack(I2C1);

  adc_H = (data[0] << 8) | data[1];

  int32_t v_x1;

  v_x1 = bme->t_fine - 76800;
  v_x1 =  (((((adc_H << 14) -(((int32_t)bme->dig_H4) << 20) - (((int32_t)bme->dig_H5) * v_x1)) +
	     ((int32_t)16384)) >> 15) * (((((((v_x1 * (int32_t)bme->dig_H6) >> 10) *
					     (((v_x1 * ((int32_t)bme->dig_H3)) >> 11) + 32768)) >> 10)\
					   + 2097152) *
					  (int32_t)bme->dig_H2 + 8192) >> 14));
  v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * (int32_t)bme->dig_H1) >> 4));
  v_x1 = (v_x1 < 0 ? 0 : v_x1);
  v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);

  th = (long)(v_x1 >> 12);
  bme->humidity = th / 1024;
}

void retrieve_data(struct BME280 *bme)
{
  getTemperature(bme);
  getPressure(bme);
  getHumidity(bme);

  UART_printbme(bme);
}

int initBME280(struct BME280 *bme)
{
  uint8_t received_data;
  I2C_start(I2C1, 0xee/* BME280_I2C_ADDR */, I2C_Direction_Transmitter);
  I2C_write(I2C1, 0xD0); // CHIP REGISTER
  I2C_stop(I2C1);
  I2C_start(I2C1, BME280_I2C_ADDR, I2C_Direction_Receiver);
  received_data = I2C_read_ack(I2C1);

  if (received_data != CHIP_ID)
    return 1;

  calibrateBME280(bme);
  return 0;
}
