#include "VL53L0X.h"
#include "pico/stdlib.h"
#include <stdio.h>

#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#define I2C_SDA 4
#define I2C_SCL 5

#define BOOT_DELAY_MS 50
#define POLL_PERIOD_MS 30

#define NUM_INIT_RETRIES 5

#define NUM_SENSORS 4 

uint8_t xshutGpios[] = {14, 15, 17 ,16};

VL53L0X sensors[NUM_SENSORS];
uint16_t readings[NUM_SENSORS];

static bool initSensors(void)
{
  bool ret = true;
  // Initialize the current sensor's XSHUT GPIO
  // And keep it low
  printf("Initializing sensors\n");

  for (int i = 0; i < NUM_SENSORS; i++)
  {
    gpio_init(xshutGpios[i]);
    gpio_set_dir(xshutGpios[i], GPIO_OUT);
    gpio_put(xshutGpios[i], false);
  }

  sleep_ms(BOOT_DELAY_MS);

  // One by one, for each sensor
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    printf("Sensor %d XSHUT %d\n", i, xshutGpios[i]);

    // Turn on its XSHUT
    gpio_put(xshutGpios[i], true);

    // Wait tBOOT
    sleep_ms(BOOT_DELAY_MS);

    // Init sequence
    sensors[i] = VL53L0X(i2c0, VL53L0X_DEFAULT_ADDRESS);
    bool initSuccess = sensors[i].init();

    sleep_ms(BOOT_DELAY_MS);
    sensors[i].setTimeout(500);

    sleep_ms(BOOT_DELAY_MS);
    sensors[i].setAddress(VL53L0X_DEFAULT_ADDRESS + i + 1);

    sleep_ms(BOOT_DELAY_MS);
    sensors[i].startContinuous();

    sleep_ms(BOOT_DELAY_MS);

    if (!initSuccess)
    {
      printf("Sensor %d failed to initialize!\n", i);
      ret &= initSuccess;
    }
  }
  return ret;
}

static void deinitSensors(void)
{
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    gpio_put(xshutGpios[i], false);
  }
}

int main()
{
  // Initialize standard I/O over USB
  stdio_init_all();

  printf("~~~ NEGATIVE LATENCY DEMO ~~~\n");

  // Initialize I2C0 at 400kHz
  i2c_init(i2c0, 400 * 1000);
  gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA);
  gpio_pull_up(I2C_SCL);

  // Initialize our warning led
#ifdef CYW43_WL_GPIO_LED_PIN
  cyw43_arch_init();
#endif

  // Initialize VL53L0X sensors
  bool sensorsInited = false;
  for (int i = 0; i < NUM_INIT_RETRIES; i++)
  {
    sensorsInited = initSensors();
    if (sensorsInited)
    {
      printf("Sensors Init'd\n");
      break;
    } else {
      printf("Init sensors failed %d times! Trying again\n", i);
      deinitSensors();
      sleep_ms(100);
    }
  }

  if (!sensorsInited)
  {
    printf("Not all sensors initialized!\n");
  #ifdef CYW43_WL_GPIO_LED_PIN
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
  #endif
  }


  // Keep the program running
  while (1)
  {
    printf("[");
    for (int i = 0; i < NUM_SENSORS; i++)
    {
      readings[i] = sensors[i].readRangeContinuousMillimeters();
      if (sensors[i].timeoutOccurred())
      {
        // TODO handle or signal errors
        // sensors[i].getAddress();
      }
      printf("%d%c", readings[i], (i != NUM_SENSORS-1) ? ',' : ']');
    }
    printf("\n");
    sleep_ms(POLL_PERIOD_MS); // Adjust as needed
  }
}
