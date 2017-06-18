#ifndef INT_H
#define INT_H

#include <bme280.h>

struct int_bme280
{
  void (*run)(struct BME280 *param);
  void *param;
};

struct int_led
{
  void (*run)(unsigned int n);
};

struct int_led *pint_led;
struct int_bme280 *pint_bme;

#endif /* !INT_H */
