/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KIT_LED_Pin GPIO_PIN_13
#define KIT_LED_GPIO_Port GPIOC
#define NIVEL_ECHO_Pin GPIO_PIN_0
#define NIVEL_ECHO_GPIO_Port GPIOA
#define VAZAO_OUT_Pin GPIO_PIN_1
#define VAZAO_OUT_GPIO_Port GPIOA
#define PH_PO_Pin GPIO_PIN_2
#define PH_PO_GPIO_Port GPIOA
#define TURB_OUT_Pin GPIO_PIN_3
#define TURB_OUT_GPIO_Port GPIOA
#define NIVEL_TRIG_Pin GPIO_PIN_4
#define NIVEL_TRIG_GPIO_Port GPIOA
#define TEMP_DATA_Pin GPIO_PIN_5
#define TEMP_DATA_GPIO_Port GPIOA
#define LED_SINAL_Pin GPIO_PIN_6
#define LED_SINAL_GPIO_Port GPIOA
#define BOMBA_Pin GPIO_PIN_7
#define BOMBA_GPIO_Port GPIOA
#define STM_TX_Pin GPIO_PIN_9
#define STM_TX_GPIO_Port GPIOA
#define STM_RX_Pin GPIO_PIN_10
#define STM_RX_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
void ADC1_Set_Channel(uint32_t channel);

typedef struct
{
    uint16_t seq_no;                  // Número sequencial da leitura
    uint16_t nivel_mm;                // Nível/distância em mm
    uint16_t ph_x100;                 // pH × 100
    uint16_t turbidez_x10;            // Turbidez × 10 NTU
    uint16_t temperatura_x10;         // Temperatura × 10 °C
    uint16_t vazao_litros_min_x10;    // Vazão × 10 L/min
} __attribute__((packed)) SENSORES_OBJ_t;

  typedef struct {
    uint16_t compressor_power;
    uint8_t warning_status;
    } __attribute__((packed)) AC_CONTROLADOR_OBJ_t;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
