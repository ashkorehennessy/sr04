//
// Created by ashkore on 23-9-2.
//

#include "sr04.h"
#define DISTANCE_LIMIT 5000

void sr04_init(sr04_t *sr04_struct){
  // Enable trigger pin
  HAL_GPIO_WritePin(sr04_struct->trig_port, sr04_struct->trig_pin, GPIO_PIN_RESET);
  // Set input capture edge to rising
  __HAL_TIM_SET_CAPTUREPOLARITY(sr04_struct->echo_htim, sr04_struct->echo_channel, TIM_INPUTCHANNELPOLARITY_RISING);
  // Set capture flag to 0
  sr04_struct->capture_flag = 0;
  // Enable echo pin
  HAL_TIM_IC_Start_IT(sr04_struct->echo_htim, sr04_struct->echo_channel);
  HAL_TIM_Base_Start_IT(sr04_struct->echo_htim);
}

void sr04_trigger(sr04_t *sr04_struct){
  // Send pulse to trigger pin
  HAL_GPIO_WritePin(sr04_struct->trig_port, sr04_struct->trig_pin, GPIO_PIN_SET);
  HAL_Delay(1);
  HAL_GPIO_WritePin(sr04_struct->trig_port, sr04_struct->trig_pin, GPIO_PIN_RESET);
}

void sr04_read_distance(sr04_t *sr04_struct){
  // This function should be called in the timer input capture callback
  switch (sr04_struct->capture_flag){
    case 0:
      sr04_struct->start_counter = __HAL_TIM_GET_COUNTER(sr04_struct->echo_htim);
      sr04_struct->capture_flag = 1;
      sr04_struct->tim_update_count = 0;
      __HAL_TIM_SET_CAPTUREPOLARITY(sr04_struct->echo_htim, sr04_struct->echo_channel, TIM_INPUTCHANNELPOLARITY_FALLING);
      break;
    case 1:
      sr04_struct->end_counter = __HAL_TIM_GET_COUNTER(sr04_struct->echo_htim) + sr04_struct->tim_update_count * sr04_struct->echo_htim->Init.Period;
      sr04_struct->capture_flag = 0;
      // Calculate distance in mm
      sr04_struct->distance = (sr04_struct->end_counter - sr04_struct->start_counter) * 340 / (SystemCoreClock / 1000000) / 2 / (1000 / sr04_struct->echo_htim->Init.Prescaler);
      // Distance limit
      if(sr04_struct->distance > DISTANCE_LIMIT){
        sr04_struct->distance = sr04_struct->last_distance;
      }
      sr04_struct->last_distance = sr04_struct->distance;
      __HAL_TIM_SET_CAPTUREPOLARITY(sr04_struct->echo_htim, sr04_struct->echo_channel, TIM_INPUTCHANNELPOLARITY_RISING);
      break;
  }
}
