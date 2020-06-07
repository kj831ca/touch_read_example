#include <stdio.h>
#include <string.h>
#include "tp_pad.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "TOUCH_PAD";
static QueueHandle_t xTPQueueHandle;
static tTouchPad_msg tp_msg[5];

esp_err_t InitTPSensors(Touch_Sensor_t * sensors, int len,uint32_t filterPeriod)
{
    touch_pad_init();
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
    for(int i=0; i<len;i++)
    {
        touch_pad_config(sensors[i].num,sensors[i].threshold);
    }
    touch_pad_filter_start(filterPeriod);
    return ESP_OK;
}

 esp_err_t ReadTPSensors(Touch_Sensor_t * sensor, int len)
 {
     for(int i=0;i<len;i++)
     {
         if(sensor[i].enable)
         {
             touch_pad_read_raw_data(sensor[i].num, &sensor[i].raw_value);
             touch_pad_read_filtered(sensor[i].num, &sensor[i].filtered_value);
         }
     }
     return ESP_OK;
 }
 QueueHandle_t InitTouchPadTask(tTouch_Sensor_Obj *touch_obj)
 {
     xTPQueueHandle = xQueueCreate(5, sizeof(tTouchPad_msg *));
     xTaskCreate(vTouchReadTask,"TouchReadTask",2048,(void*)touch_obj,6,NULL);
     return xTPQueueHandle;
 }
 int16_t ScaleSensor(Touch_Sensor_t *sensor)
 {
    int32_t tmpValue;

    if(sensor->calTable == NULL)
        return sensor->raw_value;

    tCalibration_Table *tbl = sensor->calTable;
    int index ;
    //search table if sensor reading is between value
    if(sensor->raw_value >= sensor->threshold) return 0;
    if(sensor->raw_value >=tbl->sensReading[0])
    {
        tmpValue = 1 * tbl->xNum;
        tmpValue >>= tbl->qBaseNum;
        return tmpValue;
    }
    for(index = 0;index<tbl->paramCount-1;index++)
    {
        if(tbl->sensReading[index]>tbl->sensReading[index+1])
        {
            if((sensor->raw_value <= tbl->sensReading[index]) && (sensor->raw_value >= tbl->sensReading[index+1]))
            {
                break;
            }

            if(tbl->sensReading[index]<tbl->sensReading[index+1])
            {
                if((sensor->raw_value >= tbl->sensReading[index]) && (sensor->raw_value <= tbl->sensReading[index+1]))
                break;
            }
        }
        if(index == tbl->paramCount)
        {
            tmpValue = tbl->qActualRead[index] * tbl->xNum;
            tmpValue >>= tbl->qBaseNum;
            return tmpValue;
        }
    }

    int32_t xa, xb, x ;
    int16_t ya, yb;

    //Convert number into Q-Base format
    xa = tbl->sensReading[index] << tbl->qBaseNum; 
    xb = tbl->sensReading[index+1] << tbl->qBaseNum;
    x  = sensor->raw_value << tbl->qBaseNum; 
        
    ya = tbl->qActualRead[index]; //This number already in Q-Base format
    yb = tbl->qActualRead[index+1];
    
    tmpValue = ((yb - ya)*(x -xa))/(xb-xa);
    tmpValue += ya ; //Calculate linear interpolation
    tmpValue *= tbl->xNum; //Multiply by magnify number
    tmpValue >>= tbl->qBaseNum; //Covert back to real number r x magnify.

    return (int16_t)tmpValue ;
} 
void EnableScale(Touch_Sensor_t *sensor, tCalibration_Table *tbl, uint16_t threshold)
{
    sensor->calTable = tbl;
    sensor->threshold = threshold;
    sensor->useScale = true;
}
 void vTouchReadTask(void *pvParameter)
 {
     tTouch_Sensor_Obj *sensor_obj =(tTouch_Sensor_Obj *)pvParameter;
     int idx=0;
     Touch_Sensor_t *sensor = sensor_obj->sensor;
     int len = sensor_obj->length;
     tTouchPad_msg *q_msg;
     char msg[64];
     int sensor_num;
     int16_t val,frac,digit;
     while(1)
     {
        //ReadTPSensors(sensor_obj->sensor,sensor_obj->length);
        tp_msg[idx].message[0] = '\0';
        sensor_num = 0;
        for(int i=0;i<len;i++)
        {
            if(sensor[i].enable)
            {
                touch_pad_read_raw_data(sensor[i].num, &sensor[i].raw_value);
                touch_pad_read_filtered(sensor[i].num, &sensor[i].filtered_value);
                if(sensor[i].useScale && sensor[i].calTable != NULL)
                {
                    val = ScaleSensor(&sensor[i]);
                    //Print value with fraction
                    frac = val % sensor[i].calTable->xNum;
                    digit = val / sensor[i].calTable->xNum;
                    //ESP_LOGI(TAG,"Sensor name %s",sensor[i].name); //Not sure why esp idf put " " on my channel name..
                    if(sensor_num == 0)
                    {
                        sprintf(msg,"[%s,%d.%02d",sensor[i].name,digit,frac);
                    }else 
                    {
                        sprintf(msg,"|%s,%d.%02d",sensor[i].name,digit,frac);
                    }

                } else 
                {
                    val = sensor[i].raw_value;
                
                    if(sensor_num == 0)
                    {
                        sprintf(msg,"[%s,%d",sensor[i].name,val);
                    }else 
                    {
                        sprintf(msg,"|%s,%d",sensor[i].name,val);
                    }
                }
                sensor_num++;
                strcat(tp_msg[idx].message,msg);
            }
        }
        strcat(tp_msg[idx].message,"]");

        //printf("%s \n",tp_msg[idx].message);
        //ESP_LOGI(TAG,"Q:: %s",tp_msg[idx].message);
        if(uxQueueMessagesWaiting(xTPQueueHandle)>4)
        {
            xQueueReceive(xTPQueueHandle,&q_msg,(TickType_t)10);
            ESP_LOGI(TAG,"QUEUE IS FULL");
        }
        q_msg = &tp_msg[idx];
        xQueueSendToBack(xTPQueueHandle,(void*)&q_msg,( TickType_t ) 0);
        idx++;
        if(idx > 3) idx = 0;
        vTaskDelay(250 / portTICK_PERIOD_MS);  
     }   
 }