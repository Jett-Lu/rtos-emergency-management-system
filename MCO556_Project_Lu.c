/*
File: MCO556_Project_Lu.c
Authors: Jett Lu
Date: 2023-12-04
Description: A Multitasking System with Real-time Requirements - See the MCO556 Final Project document for details.
*/

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "peripherals.h"

// Time interval for the software timer
#define SW_TIMER_PERIOD_MS (2000 / portTICK_PERIOD_MS)
// Blink period for Task3 (Emergency Protocol)
#define TASK3_BLINK_PERIOD_MS (125)

// Task priorities
#define Task1_PRIORITY (tskIDLE_PRIORITY + 1)
#define Task2_PRIORITY (tskIDLE_PRIORITY + 1)
#define Task3_PRIORITY (tskIDLE_PRIORITY + 2)

// Binary semaphores for task synchronization
SemaphoreHandle_t xSemaphore_Task1;
SemaphoreHandle_t xSemaphore_Task2;
SemaphoreHandle_t xSemaphore_Task3;

// Software Timer handle
TimerHandle_t SwTimerHandle = NULL;
BaseType_t xHigherPriorityTaskWoken = pdFALSE;

// Forward declarations for functions
static void SwTimerCallback(TimerHandle_t xTimer);
static void Task1(void *pvParameters);
static void Task2(void *pvParameters);
static void Task3(void *pvParameters);

// Delay function
void delay(void)
{
    uint32_t i = 20000000U;
    while (i--)
    {
        __NOP();
    }
}

// PIT timer channel 0 interrupt handler
void PIT_CHANNEL_0_IRQHANDLER(void)
{
    PIT_ClearStatusFlags(PIT, PIT_CHANNEL_0, kPIT_TimerFlag);
    LED_RED_TOGGLE(); // Toggle the state of the RED LED
}

// SW2 GPIOC interrupt handler
void SW2_GPIOC_IRQHANDLER(void)
{
    GPIO_PortClearInterruptFlags(GPIOC, 1U << 6U);
    xTimerStopFromISR(SwTimerHandle, &xHigherPriorityTaskWoken);
    PRINTF("SW2 ISR - EMERGENCY SYSTEM SHUTDOWN!\r\n");
    xSemaphoreGiveFromISR(xSemaphore_Task3, &xHigherPriorityTaskWoken);
}

// SW3 GPIOA interrupt handler
void SW3_GPIOA_IRQHANDLER(void)
{
    GPIO_PortClearInterruptFlags(GPIOA, 1U << 4U);
    PRINTF("SW3 ISR - REGULAR system operation is resuming...\r\n\n");
    delay();
    PIT_StopTimer(PIT_PERIPHERAL, PIT_CHANNEL_0);
    LED_RED_OFF();
    LED_GREEN_ON();
    GPIO_PinWrite(GPIOC, 16U, 0U);
    xTimerStartFromISR(SwTimerHandle, &xHigherPriorityTaskWoken);
}

// Main function
int main(void)
{
    // Create binary semaphores
    xSemaphore_Task1 = xSemaphoreCreateBinary();
    xSemaphore_Task2 = xSemaphoreCreateBinary();
    xSemaphore_Task3 = xSemaphoreCreateBinary();

    // Initialize hardware
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitDebugConsole();
    SystemCoreClockUpdate();

    // Create software timer
    SwTimerHandle = xTimerCreate("SwTimer", SW_TIMER_PERIOD_MS, pdTRUE, 0, SwTimerCallback);

    // Create Task1
    if (xTaskCreate(Task1, "TASK1", configMINIMAL_STACK_SIZE + 128, NULL, Task1_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }

    // Create Task2
    if (xTaskCreate(Task2, "TASK2", configMINIMAL_STACK_SIZE + 128, NULL, Task2_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }

    // Create Task3 with higher priority
    if (xTaskCreate(Task3, "TASK3", configMINIMAL_STACK_SIZE + 128, NULL, Task3_PRIORITY, NULL) != pdPASS)
    {
        PRINTF("Task creation failed!.\r\n");
        while (1)
            ;
    }

    // Initialize LEDs and print initial messages
    LED_GREEN_ON();
    GPIO_PinWrite(GPIOC, 16U, 0U);
    PRINTF("MCO556_FinalProject_Lastname1_Lastname2\r\n");
    PRINTF("Regular Operation\r\n");
    PRINTF("Alarm Sound is OFF\r\n\n");

    // Start the software timer
    xTimerStart(SwTimerHandle, 0);

    // Start FreeRTOS scheduler
    vTaskStartScheduler();

    // Infinite loop
    for (;;)
        ;
}

// Software Timer callback function
static void SwTimerCallback(TimerHandle_t xTimer)
{
    PRINTF("Software Timer ISR - sensors are read\r\n");
    xSemaphoreGiveFromISR(xSemaphore_Task1, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Task1 function
static void Task1(void *pvParameters)
{
    while (1)
    {
        xSemaphoreTake(xSemaphore_Task1, portMAX_DELAY);
        PRINTF("Task1 - sensor data is processed\r\n");
        xSemaphoreGive(xSemaphore_Task2);
    }
}

// Task2 function
static void Task2(void *pvParameters)
{
    while (1)
    {
        xSemaphoreTake(xSemaphore_Task2, portMAX_DELAY);
        PRINTF("Task2 - display is updated\r\n\n");
    }
}

// Task3 function
static void Task3(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        xSemaphoreTake(xSemaphore_Task3, portMAX_DELAY);
        PRINTF("Task3 - EMERGENCY protocol is being executed...\r\n");
        PRINTF("The system is halted\r\n");
        PRINTF("RED LIGHT is flashing and SOUND ALARM is ON\r\n");
        PRINTF("Press button SW3 to clear the system and reboot...\r\n\n");

        // Turn OFF GREEN LED, turn ON external LED connected to PTC16
        LED_GREEN_OFF();
        GPIO_PinWrite(GPIOC, 16U, 1U);

        // Start PIT timer channel 0 for blinking RED LED
        PIT_StartTimer(PIT_PERIPHERAL, PIT_CHANNEL_0);

        // Blink the RED LED rapidly at 4Hz
        vTaskDelayUntil(&xLastWakeTime, TASK3_BLINK_PERIOD_MS / portTICK_PERIOD_MS);
        LED_RED_TOGGLE();
        vTaskDelayUntil(&xLastWakeTime, TASK3_BLINK_PERIOD_MS / portTICK_PERIOD_MS);
    }
}
