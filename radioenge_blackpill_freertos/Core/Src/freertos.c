/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticTimer_t osStaticTimerDef_t;
typedef StaticEventGroup_t osStaticEventGroupDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[ 128 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .priority = (osPriority_t) osPriorityLow1,
};
/* Definitions for ATParsingTask */
osThreadId_t ATParsingTaskHandle;
uint32_t ATParsingTaskBuffer[ 512 ];
osStaticThreadDef_t ATParsingTaskControlBlock;
const osThreadAttr_t ATParsingTask_attributes = {
  .name = "ATParsingTask",
  .cb_mem = &ATParsingTaskControlBlock,
  .cb_size = sizeof(ATParsingTaskControlBlock),
  .stack_mem = &ATParsingTaskBuffer[0],
  .stack_size = sizeof(ATParsingTaskBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal7,
};
/* Definitions for ATHandlingTask */
osThreadId_t ATHandlingTaskHandle;
uint32_t ATHandlingTaskBuffer[ 512 ];
osStaticThreadDef_t ATHandlingTaskControlBlock;
const osThreadAttr_t ATHandlingTask_attributes = {
  .name = "ATHandlingTask",
  .cb_mem = &ATHandlingTaskControlBlock,
  .cb_size = sizeof(ATHandlingTaskControlBlock),
  .stack_mem = &ATHandlingTaskBuffer[0],
  .stack_size = sizeof(ATHandlingTaskBuffer),
  .priority = (osPriority_t) osPriorityAboveNormal6,
};
/* Definitions for UARTProcTask */
osThreadId_t UARTProcTaskHandle;
uint32_t UARTProcTaskBuffer[ 512 ];
osStaticThreadDef_t UARTProcTaskControlBlock;
const osThreadAttr_t UARTProcTask_attributes = {
  .name = "UARTProcTask",
  .cb_mem = &UARTProcTaskControlBlock,
  .cb_size = sizeof(UARTProcTaskControlBlock),
  .stack_mem = &UARTProcTaskBuffer[0],
  .stack_size = sizeof(UARTProcTaskBuffer),
  .priority = (osPriority_t) osPriorityHigh7,
};
/* Definitions for ModemMngrTask */
osThreadId_t ModemMngrTaskHandle;
uint32_t ModemMngrTaskBuffer[ 512 ];
osStaticThreadDef_t ModemMngrTaskControlBlock;
const osThreadAttr_t ModemMngrTask_attributes = {
  .name = "ModemMngrTask",
  .cb_mem = &ModemMngrTaskControlBlock,
  .cb_size = sizeof(ModemMngrTaskControlBlock),
  .stack_mem = &ModemMngrTaskBuffer[0],
  .stack_size = sizeof(ModemMngrTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal7,
};
/* Definitions for AppSendTask */
osThreadId_t AppSendTaskHandle;
uint32_t SendTemperatureBuffer[ 1024 ];
osStaticThreadDef_t SendTemperatureControlBlock;
const osThreadAttr_t AppSendTask_attributes = {
  .name = "AppSendTask",
  .cb_mem = &SendTemperatureControlBlock,
  .cb_size = sizeof(SendTemperatureControlBlock),
  .stack_mem = &SendTemperatureBuffer[0],
  .stack_size = sizeof(SendTemperatureBuffer),
  .priority = (osPriority_t) osPriorityLow7,
};
/* Definitions for DutyCycleTask */
osThreadId_t DutyCycleTaskHandle;
uint32_t DutyCycleTaskBuffer[ 128 ];
osStaticThreadDef_t DutyCycleTaskControlBlock;
const osThreadAttr_t DutyCycleTask_attributes = {
  .name = "DutyCycleTask",
  .cb_mem = &DutyCycleTaskControlBlock,
  .cb_size = sizeof(DutyCycleTaskControlBlock),
  .stack_mem = &DutyCycleTaskBuffer[0],
  .stack_size = sizeof(DutyCycleTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for uartQueue */
osMessageQueueId_t uartQueueHandle;
uint8_t uartQueueBuffer[ 4 * sizeof( void* ) ];
osStaticMessageQDef_t uartQueueControlBlock;
const osMessageQueueAttr_t uartQueue_attributes = {
  .name = "uartQueue",
  .cb_mem = &uartQueueControlBlock,
  .cb_size = sizeof(uartQueueControlBlock),
  .mq_mem = &uartQueueBuffer,
  .mq_size = sizeof(uartQueueBuffer)
};
/* Definitions for ATQueue */
osMessageQueueId_t ATQueueHandle;
uint8_t ATQueueBuffer[ 4 * sizeof( void* ) ];
osStaticMessageQDef_t ATQueueControlBlock;
const osMessageQueueAttr_t ATQueue_attributes = {
  .name = "ATQueue",
  .cb_mem = &ATQueueControlBlock,
  .cb_size = sizeof(ATQueueControlBlock),
  .mq_mem = &ATQueueBuffer,
  .mq_size = sizeof(ATQueueBuffer)
};
/* Definitions for ModemSendQueue */
osMessageQueueId_t ModemSendQueueHandle;
uint8_t ModemSendQueueBuffer[ 4 * sizeof( void* ) ];
osStaticMessageQDef_t ModemSendQueueControlBlock;
const osMessageQueueAttr_t ModemSendQueue_attributes = {
  .name = "ModemSendQueue",
  .cb_mem = &ModemSendQueueControlBlock,
  .cb_size = sizeof(ModemSendQueueControlBlock),
  .mq_mem = &ModemSendQueueBuffer,
  .mq_size = sizeof(ModemSendQueueBuffer)
};
/* Definitions for ACControladorQueue */
osMessageQueueId_t ACControladorQueueHandle;
uint8_t ACControladorQueueBuffer[ 8 * sizeof( AC_CONTROLADOR_OBJ_t ) ];
osStaticMessageQDef_t ACControladorQueueControlBlock;
const osMessageQueueAttr_t ACControladorQueue_attributes = {
  .name = "ACControladorQueue",
  .cb_mem = &ACControladorQueueControlBlock,
  .cb_size = sizeof(ACControladorQueueControlBlock),
  .mq_mem = &ACControladorQueueBuffer,
  .mq_size = sizeof(ACControladorQueueBuffer)
};
/* Definitions for PeriodicSendTimer */
osTimerId_t PeriodicSendTimerHandle;
osStaticTimerDef_t PeriodicSendTimerControlBlock;
const osTimerAttr_t PeriodicSendTimer_attributes = {
  .name = "PeriodicSendTimer",
  .cb_mem = &PeriodicSendTimerControlBlock,
  .cb_size = sizeof(PeriodicSendTimerControlBlock),
};
/* Definitions for ModemLedTimer */
osTimerId_t ModemLedTimerHandle;
osStaticTimerDef_t ModemLedTimerControlBlock;
const osTimerAttr_t ModemLedTimer_attributes = {
  .name = "ModemLedTimer",
  .cb_mem = &ModemLedTimerControlBlock,
  .cb_size = sizeof(ModemLedTimerControlBlock),
};
/* Definitions for DutyCycleTimer */
osTimerId_t DutyCycleTimerHandle;
osStaticTimerDef_t DutyCycleTimerControlBlock;
const osTimerAttr_t DutyCycleTimer_attributes = {
  .name = "DutyCycleTimer",
  .cb_mem = &DutyCycleTimerControlBlock,
  .cb_size = sizeof(DutyCycleTimerControlBlock),
};
/* Definitions for ATCommandSemaphore */
osSemaphoreId_t ATCommandSemaphoreHandle;
const osSemaphoreAttr_t ATCommandSemaphore_attributes = {
  .name = "ATCommandSemaphore"
};
/* Definitions for ATResponseSemaphore */
osSemaphoreId_t ATResponseSemaphoreHandle;
const osSemaphoreAttr_t ATResponseSemaphore_attributes = {
  .name = "ATResponseSemaphore"
};
/* Definitions for UARTTXSemaphore */
osSemaphoreId_t UARTTXSemaphoreHandle;
const osSemaphoreAttr_t UARTTXSemaphore_attributes = {
  .name = "UARTTXSemaphore"
};
/* Definitions for RadioStateSemaphore */
osSemaphoreId_t RadioStateSemaphoreHandle;
const osSemaphoreAttr_t RadioStateSemaphore_attributes = {
  .name = "RadioStateSemaphore"
};
/* Definitions for LoRaTXSemaphore */
osSemaphoreId_t LoRaTXSemaphoreHandle;
const osSemaphoreAttr_t LoRaTXSemaphore_attributes = {
  .name = "LoRaTXSemaphore"
};
/* Definitions for ModemStatusFlags */
osEventFlagsId_t ModemStatusFlagsHandle;
osStaticEventGroupDef_t ModemStatusFlagsControlBlock;
const osEventFlagsAttr_t ModemStatusFlags_attributes = {
  .name = "ModemStatusFlags",
  .cb_mem = &ModemStatusFlagsControlBlock,
  .cb_size = sizeof(ModemStatusFlagsControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void ATParsingTaskCode(void *argument);
extern void ATHandlingTaskCode(void *argument);
extern void UARTProcTaskCode(void *argument);
extern void ModemManagerTaskCode(void *argument);
extern void AppSendTaskCode(void *argument);
extern void DutyCycleTaskCode(void *argument);
extern void PeriodicSendTimerCallback(void *argument);
extern void ModemLedCallback(void *argument);
extern void DutyCycleTimerCallback(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN PREPOSTSLEEP */
__weak void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
/* place for user code */
}

__weak void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{
/* place for user code */
}
/* USER CODE END PREPOSTSLEEP */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of ATCommandSemaphore */
  ATCommandSemaphoreHandle = osSemaphoreNew(1, 0, &ATCommandSemaphore_attributes);

  /* creation of ATResponseSemaphore */
  ATResponseSemaphoreHandle = osSemaphoreNew(1, 0, &ATResponseSemaphore_attributes);

  /* creation of UARTTXSemaphore */
  UARTTXSemaphoreHandle = osSemaphoreNew(1, 0, &UARTTXSemaphore_attributes);

  /* creation of RadioStateSemaphore */
  RadioStateSemaphoreHandle = osSemaphoreNew(1, 0, &RadioStateSemaphore_attributes);

  /* creation of LoRaTXSemaphore */
  LoRaTXSemaphoreHandle = osSemaphoreNew(1, 0, &LoRaTXSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  osSemaphoreRelease(ATCommandSemaphoreHandle);
  osSemaphoreRelease(ATResponseSemaphoreHandle);
  osSemaphoreRelease(UARTTXSemaphoreHandle);
  osSemaphoreRelease(RadioStateSemaphoreHandle);
  osSemaphoreRelease(LoRaTXSemaphoreHandle);
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of PeriodicSendTimer */
  PeriodicSendTimerHandle = osTimerNew(PeriodicSendTimerCallback, osTimerPeriodic, NULL, &PeriodicSendTimer_attributes);

  /* creation of ModemLedTimer */
  ModemLedTimerHandle = osTimerNew(ModemLedCallback, osTimerPeriodic, NULL, &ModemLedTimer_attributes);

  /* creation of DutyCycleTimer */
  DutyCycleTimerHandle = osTimerNew(DutyCycleTimerCallback, osTimerOnce, NULL, &DutyCycleTimer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of uartQueue */
  uartQueueHandle = osMessageQueueNew (4, sizeof(void*), &uartQueue_attributes);

  /* creation of ATQueue */
  ATQueueHandle = osMessageQueueNew (4, sizeof(void*), &ATQueue_attributes);

  /* creation of ModemSendQueue */
  ModemSendQueueHandle = osMessageQueueNew (4, sizeof(void*), &ModemSendQueue_attributes);

  /* creation of ACControladorQueue */
  ACControladorQueueHandle = osMessageQueueNew (8, sizeof(AC_CONTROLADOR_OBJ_t), &ACControladorQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of ATParsingTask */
  ATParsingTaskHandle = osThreadNew(ATParsingTaskCode, NULL, &ATParsingTask_attributes);

  /* creation of ATHandlingTask */
  ATHandlingTaskHandle = osThreadNew(ATHandlingTaskCode, NULL, &ATHandlingTask_attributes);

  /* creation of UARTProcTask */
  UARTProcTaskHandle = osThreadNew(UARTProcTaskCode, NULL, &UARTProcTask_attributes);

  /* creation of ModemMngrTask */
  ModemMngrTaskHandle = osThreadNew(ModemManagerTaskCode, NULL, &ModemMngrTask_attributes);

  /* creation of AppSendTask */
  AppSendTaskHandle = osThreadNew(AppSendTaskCode, NULL, &AppSendTask_attributes);

  /* creation of DutyCycleTask */
  DutyCycleTaskHandle = osThreadNew(DutyCycleTaskCode, NULL, &DutyCycleTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the event(s) */
  /* creation of ModemStatusFlags */
  ModemStatusFlagsHandle = osEventFlagsNew(&ModemStatusFlags_attributes);

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

