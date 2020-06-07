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
static tCalibration_Table ch0Cal = {{373,290,197,152,123,102,84,71,65,60,55,50,46}, //<== Your TP reading from calibrate round up
        {128,256,512,768,1024,1280,1536,1792,2048,2304,2560,2816,3072} //<==Your actual dept when calibrate x 2^Qbase
        ,8 //<==Q Base in this case I use Q8 which has resultion of 1/256
        ,100 //<== decimal point 1/100 (x.xx)
        ,13}; //<== Number of calibrated points.

static tCalibration_Table ch3Cal = {{477,388,319,265,225,198,176,159,144,129,118,111,104,89,80},
        {128,256,512,768,1024,1280,1536,1792,2048,2304,2560,2816,3072,3584,4096},8,100,15};


static Touch_Sensor_t g_tp_sensor[] ={CREATE_TOUCH_SENSOR(CH0,0),CREATE_TOUCH_SENSOR(CH3,3)}; 
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
    //for(int i=0;i<)
    EnableScale(&g_tp_sensor[0],&ch0Cal,500);
    EnableScale(&g_tp_sensor[1],&ch3Cal,500);
    InitTPSensors(tp_obj.sensor,tp_obj.length,10);

    //Testing sending touch pad reads between tasks.
    xTaskCreate(&tp_example_read_task, "touch_pad_read_task", 2048, NULL, 5, NULL);
    tp_Queue = InitTouchPadTask(&tp_obj);

}