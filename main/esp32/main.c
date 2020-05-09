/* Touch Pad Read Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "tp_pad.h"
#include "esp_log.h"

#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

//static Touch_Sensor_t g_tp_sensor[MAX_TOUCH_SENSOR];

static Touch_Sensor_t g_tp_sensor[] ={CREATE_TOUCH_SENSOR("CH0",0),CREATE_TOUCH_SENSOR("CH3",3)};
QueueHandle_t tp_Queue; 
static tTouch_Sensor_Obj  tp_obj= 
{
    .sensor = g_tp_sensor,
    .length = (sizeof g_tp_sensor / sizeof g_tp_sensor[0])
};
/*
  Read values sensed at all available touch pads.
 Print out values in a loop on a serial monitor.
 */
static void tp_example_read_task(void *pvParameter)
{
    tTouchPad_msg *msg;
    while (1) 
    {
       if(tp_Queue != 0)
       {
           if(xQueueReceive(tp_Queue,&msg,(TickType_t)10))
           {
               printf("Receive: %s \n",msg->message);
           }
       }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    // Initialize the touch pad hardware
    InitTPSensors(tp_obj.sensor,tp_obj.length,10);
    //Testing sending touch pad reads between tasks.
    xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
    tp_Queue = InitTouchPadTask(&tp_obj);

}