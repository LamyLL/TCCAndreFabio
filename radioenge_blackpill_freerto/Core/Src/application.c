#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "radioenge_modem.h"
#include "main.h"
#include "ow.h"
#include <stdio.h>
#include <string.h>

extern osTimerId_t PeriodicSendTimerHandle;
extern osThreadId_t AppSendTaskHandle;
extern ADC_HandleTypeDef hadc1;
extern osEventFlagsId_t ModemStatusFlagsHandle;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim5;
extern osMessageQueueId_t ACControladorQueueHandle;

/* USER CODE BEGIN 0 */
char msg[256];

ow_t ds18;

void ds18_tim_cb(TIM_HandleTypeDef *htim)
{
    ow_callback(&ds18);
}

void ds18_done_cb(ow_err_t error)
{
    // Final do Callback DS18 (Sensor de temperatura)
}

/* USER CODE END 0 */


void LoRaWAN_RxEventCallback(uint8_t *data, uint32_t length, uint32_t port, int32_t rssi, int32_t snr)
{
    /*
    AC_CONTROLADOR_OBJ_t comando;
    memcpy((uint8_t *)(&comando),data,length/2);
    osMessageQueuePut(ACControladorQueueHandle, &comando, 0U, osWaitForever);
    */
}

void DutyCycleTaskCode ( void * argument )
{
    
    AC_CONTROLADOR_OBJ_t pwm_data;
    osStatus_t status;

    uint16_t history;
    

    while (1)
    {
        
        status = osMessageQueueGet(ACControladorQueueHandle, &pwm_data, NULL, osWaitForever); 
        if (status == osOK)
        {
            if (pwm_data.compressor_power >= 0 && pwm_data.compressor_power <= 100)
            {
                if(pwm_data.compressor_power == history)
                {
                    sprintf(msg, "Compressor_power mantido em %u%%\n", pwm_data.compressor_power);
                } else {
                    sprintf(msg, "Compressor_power ajustado para %u%%\n", pwm_data.compressor_power);
                }
                htim3.Instance->CCR2 = (htim3.Instance->ARR*pwm_data.compressor_power)/100;
                SendToUART(msg, strlen(msg));
            } else {
                sprintf(msg, "Erro: Compressor_power fora do intervalo (valor = %u)\n", pwm_data.compressor_power);
                SendToUART(msg, strlen(msg));
            }
            history = pwm_data.compressor_power;
        }
    }
    //vTaskDelete(NULL); // DEBBUGING, DELETAR DEPOIS!!!!!!!!!!!!!!!!!!!!!!!!!!
}


void PeriodicSendTimerCallback(void *argument)
{
}

void AppSendTaskCode(void *argument)
{
    // Structure Enviada
    SENSORES_OBJ_t leitura;
    leitura.seq_no = 0;
    // Leituras de ADC
    float nivel_float = 0.0f;
    float ph_float = 0.0f;
    float turb_float = 0.0f;
    float temp_float = 0.0f;
    float vazao_float = 0.0f;
    // Sensores
    uint32_t echo_time;
    uint16_t adc_value;
    uint8_t data[16];
    uint16_t temperatura;
    uint32_t vazao_pulsos = 0;
    // Inicialização do OneWire para o sensor de temperatura DS18B20
    ow_init_t ow_init_struct;
    ow_init_struct.tim_handle = &htim3;
    ow_init_struct.gpio = TEMP_DATA_GPIO_Port;
    ow_init_struct.pin = TEMP_DATA_Pin;
    ow_init_struct.tim_cb = ds18_tim_cb;
    ow_init_struct.done_cb = ds18_done_cb;
    ow_init_struct.rom_id_filter = 0;

    ow_init(&ds18, &ow_init_struct);


    while (1)
    {   
        // Sensor de Nível JSN-SR04T
        xTaskNotifyStateClear(NULL); // Remove qualquer notificação da tarefa
        //HAL_TIM_IC_Stop(&htim2, TIM_CHANNEL_1);
        //HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_2);
        __HAL_TIM_SET_COUNTER(&htim2, 0);
        //__HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_CC1 | TIM_FLAG_CC2);
        HAL_TIM_IC_Start(&htim2, TIM_CHANNEL_1); // Inicializa a captura da borda de subida
        HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2); // Inicializa a captura da borda de descida com interrupção
        HAL_GPIO_WritePin(NIVEL_TRIG_GPIO_Port,NIVEL_TRIG_Pin, GPIO_PIN_SET); // Gera pulso de trigger
        //vTaskDelay(pdMS_TO_TICKS(1)); // Bloqueia a tarefa por 1 ms
        delay_us(10); // Aguarda 10 us para garantir o pulso de trigger
        HAL_GPIO_WritePin(NIVEL_TRIG_GPIO_Port,NIVEL_TRIG_Pin, GPIO_PIN_RESET); // Desliga o pulso de trigger, iniciando a medida
        if (xTaskNotifyWait(0, 0xFFFFFFFF, &echo_time, pdMS_TO_TICKS(1000)) == pdTRUE)
        {
        nivel_float = ((float)echo_time * 343.0f) / 2000.0f;
        leitura.nivel_mm = (uint16_t)nivel_float;
        }
        else // Erro do sensor ou leitura demora mais que 100 ms
        {
        nivel_float = 0.0f;
        leitura.nivel_mm = 0;
        HAL_TIM_IC_Stop(&htim2, TIM_CHANNEL_1); // Garante que o timer pare, mesmo em caso de erro
        HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_2); // Garante que o timer pare, mesmo em caso de erro
        }

        
        // Sensor de PH ph4502c
        ADC1_Set_Channel(ADC_CHANNEL_2);
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 100);
        adc_value = HAL_ADC_GetValue(&hadc1); // Lê o valor do ADC
        ph_float = ((float)adc_value * 5.0f) / 4095.0f; // Calcula a tensão do sensor (até 5V), antes do divisor de tensão para 3.3
        ph_float = 7.0f + ((2.62f-ph_float)/0.18f); // Setpoint do sensor se encontra em 2.62 V para ph de 7
        leitura.ph_x100 = (uint16_t)(ph_float * 100.0f);


        // Sensor de Turbidez TS-300B
        ADC1_Set_Channel(ADC_CHANNEL_3);
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, 100);
        adc_value = HAL_ADC_GetValue(&hadc1); // Lê o valor do ADC
        turb_float = ((float)adc_value * 5.0f) / 4095.0f; // Calcula a tensão do sensor (até 5V), antes do divisor de tensão para 3.3
        turb_float = 4000.0f * (1.0f - (turb_float / 3.68f)); // Setpoint do sensor se encontra em ~3.68 V para água limpa ~0 NTU, 0 V significa ~4000 NTU
        leitura.turbidez_x10 = (uint16_t)(turb_float * 10.0f);


        // Sensor de Temperatura DS18B20
        ow_update_rom_id(&ds18);
        while (ow_is_busy(&ds18))
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        vTaskDelay(pdMS_TO_TICKS(10));
        ow_xfer_by_id(&ds18, 0, 0x44, NULL, 0, 0);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ow_xfer_by_id(&ds18, 0, 0xBE, NULL, 0, 9);
        while (ow_is_busy(&ds18))
        {
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        ow_read_resp(&ds18, data, 16);
        temperatura = (uint16_t)((data[1] << 8) | data[0]);
        temp_float = (float)temperatura / 16.0f;
        leitura.temperatura_x10 = (uint16_t)(temp_float * 10.0f);


        // Sensor de Vazão YF-B5
        __HAL_TIM_SET_COUNTER(&htim5, 0); // Reinicia a contagem de pulsos do sensor de vazão
        vTaskDelay(pdMS_TO_TICKS(1000)); // Aguarda 1 segundo para a contagem de pulsos
        vazao_pulsos = __HAL_TIM_GET_COUNTER(&htim5); // Obtém a frequência do sensor (pulsos em 1 segundo)
        vazao_float = (float)vazao_pulsos / 6.6f;
        leitura.vazao_litros_min_x10 = (uint16_t)(vazao_float * 10.0f);

        // Envio dos dados
        leitura.seq_no++;
        LoRaSendB(2, (uint8_t *)&leitura, sizeof(SENSORES_OBJ_t));
    }
}

/* USER CODE BEGIN 1 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim == &htim2)
    {

        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            uint32_t elapsed;
            elapsed = htim->Instance->CCR2 - htim->Instance->CCR1;

            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            xTaskNotifyFromISR(AppSendTaskHandle, elapsed, eSetValueWithOverwrite, &xHigherPriorityTaskWoken);
            // Notifica a tarefa AppSendTaskHandle com o valor do tempo decorrido (elapsed)

            HAL_TIM_IC_Stop(&htim2, TIM_CHANNEL_1); // Para o timer
            HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_2); // Para o timer

            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
            // Avisa o sistema para retomar a tarefa de maior prioridade
        }
    }
}

void delay_us(uint32_t us)
{
    uint32_t start = DWT->CYCCNT;
    uint32_t cycles = us * (HAL_RCC_GetHCLKFreq() / 1000000U);

    while ((DWT->CYCCNT - start) < cycles)
    {
    }
}
/* USER CODE END 1 */

void ReadFromADCTaskCode(void *argument)
{
}