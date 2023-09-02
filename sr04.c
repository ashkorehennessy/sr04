//
// Created by ashkore on 23-9-2.
//

#include "sr04.h"

void sr04_init(sr04_t *sr04){
  // Enable trigger pin
  HAL_GPIO_WritePin(sr04->trig_port, sr04->trig_pin, GPIO_PIN_RESET);
  // Set input capture edge to rising
  __HAL_TIM_SET_CAPTUREPOLARITY(sr04->echo_htim, sr04->echo_channel, TIM_INPUTCHANNELPOLARITY_RISING);
  // Set capture flag to 0
  sr04->capture_flag = 0;
  // Enable echo pin
  HAL_TIM_IC_Start_IT(sr04->echo_htim, sr04->echo_channel);
}

void sr04_trigger(sr04_t *sr04){
  // Send pulse to trigger pin
  HAL_GPIO_WritePin(sr04->trig_port, sr04->trig_pin, GPIO_PIN_SET);
  __HAL_TIM_SET_COUNTER(sr04->echo_htim, 0);
  HAL_GPIO_WritePin(sr04->trig_port, sr04->trig_pin, GPIO_PIN_RESET);
}

void sr04_read_distance(sr04_t *sr04){
  // This function should be called in the timer input capture callback
  static uint32_t start_counter;
  static uint32_t end_counter;
  switch (sr04->capture_flag){
    case 0:
      start_counter = __HAL_TIM_GET_COUNTER(sr04->echo_htim);
      sr04->capture_flag = 1;
      __HAL_TIM_SET_CAPTUREPOLARITY(sr04->echo_htim, sr04->echo_channel, TIM_INPUTCHANNELPOLARITY_FALLING);
      break;
    case 1:
      end_counter = __HAL_TIM_GET_COUNTER(sr04->echo_htim);
      sr04->capture_flag = 0;
      // Calculate distance in mm
      sr04->distance = (end_counter - start_counter) * 340 / (SystemCoreClock / 1000000) / 2 / (1000 / sr04->echo_htim->Init.Prescaler);
      __HAL_TIM_SET_CAPTUREPOLARITY(sr04->echo_htim, sr04->echo_channel, TIM_INPUTCHANNELPOLARITY_RISING);
      break;
  }
}
