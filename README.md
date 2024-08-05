# STM32 HAL library for HC-SR04 ultrasonic sensor

A simple library for HC-SR04 ultrasonic sensor. It uses STM32 HAL library.

Copy the library header and source files to your project folder(Inc and Src folders).

## Usage

Configure STM32CubeMX to use GPIO pins for trigger pin, use TIM input capture mode for echo pin.

In this example, I used PA9 for trigger pin and TIM1 CH1(PA8) for echo pin.

Set timer prescaler to 72, use internal clock source.

Set channel 1 to input capture mode, set polarity to rising edge.

Enable interrupt in NVIC settings.

Include the header file in your main.h file.
    
```c
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sr04.h"
/* USER CODE END Includes */
```

Declare the sensor struct in your main.c file.

```c
/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
sr04_t sr04;
/* USER CODE END PV */
```

Configure the sensor struct and initialize the sensor.

```c
  /* USER CODE BEGIN 2 */
  sr04.trig_port = GPIOA;
  sr04.trig_pin = GPIO_PIN_9;
  sr04.echo_htim = &htim1;
  sr04.echo_channel = TIM_CHANNEL_1;
  sr04_init(&sr04);
  /* USER CODE END 2 */
```

Trigger the sensor in main loop.

```c
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    sr04_trigger(&sr04);
    HAL_Delay(100);
    printf("Distance: %lu mm\r\n", sr04.distance);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
```

In stm32f1xx_it.c file, use extern to import the sensor struct.
```c
/* USER CODE BEGIN EV */
extern sr04_t sr04;
/* USER CODE END EV */
```

Increase the update count in timer interrupt callback function. This is used to calibrate the distance.

```c
void TIM1_UP_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_IRQn 0 */
  // Update interrupt handling
  if (__HAL_TIM_GET_FLAG(sr04.echo_htim, TIM_FLAG_UPDATE) != RESET)
  {
      if (__HAL_TIM_GET_IT_SOURCE(sr04.echo_htim, TIM_IT_UPDATE) != RESET)
      {
          sr04.tim_update_count++;
      }
  }
  /* USER CODE END TIM1_UP_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_IRQn 1 */

  /* USER CODE END TIM1_UP_IRQn 1 */
}
```

Replace ```TIM_FLAG_CC1``` and ```TIM_IT_CC1``` to actual capture channel number.

Read the distance in timer interrupt callback function.

```c
void TIM1_CC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_CC_IRQn 0 */
  // Capture interrupt handling
  if (__HAL_TIM_GET_FLAG(&htim1, TIM_FLAG_CC1) != RESET)
  {
      if (__HAL_TIM_GET_IT_SOURCE(&htim1, TIM_IT_CC1) != RESET)
      {
          sr04_read_distance(&sr04);
      }
  }
  /* USER CODE END TIM1_CC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_CC_IRQn 1 */

  /* USER CODE END TIM1_CC_IRQn 1 */
}
```

The distance will be stored in sr04.distance variable, the unit is mm.

---

General purpose timer version:

```c
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
  // Capture interrupt handling
  if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_CC1) != RESET)
  {
      if (__HAL_TIM_GET_IT_SOURCE(&htim4, TIM_IT_CC1) != RESET)
      {
          sr04_read_distance(&sr04);
      }
  }
  // Update interrupt handling
  if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) != RESET)
  {
      if (__HAL_TIM_GET_IT_SOURCE(&htim4, TIM_IT_UPDATE) != RESET)
      {
          sr04.tim_update_count++;
      }
  }
  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
}
```

## Multiple sensor instances

You can use multiple sensor instances by declaring and configuring multiple sensor structs.

Interrupt callback function:

```c
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
    // Capture interrupt handling
    if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_CC1) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(&htim4, TIM_IT_CC1) != RESET)
        {
            sr04_read_distance(&sr04_1);
        }
    }
    if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_CC2) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(&htim4, TIM_IT_CC2) != RESET)
        {
            sr04_read_distance(&sr04_2);
        }
    }
    // Update interrupt handling
    if (__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_UPDATE) != RESET)
    {
        if (__HAL_TIM_GET_IT_SOURCE(&htim4, TIM_IT_UPDATE) != RESET)
        {
            sr04_1.tim_update_count++;
            sr04_2.tim_update_count++;
        }
    }
  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
}
```


