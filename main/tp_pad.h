#ifndef TP_TOUCH_PAD_DOT_H
#define TP_TOUCH_PAD_DOT_H

#include "driver/touch_pad.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#define MAX_TOUCH_SENSOR 10

typedef struct
{
    int32_t sensReading[20];
    short qActualRead[20];
    uint8_t qBaseNum;
    short xNum;
    int paramCount;

}tCalibration_Table;

typedef struct
{
    /* data */
    char name[10];
    uint8_t num;
    uint16_t offset;
    uint16_t gain;
    uint16_t raw_value;
    uint16_t filtered_value;
    uint16_t threshold;
    bool enable;
    bool useScale;
    tCalibration_Table *calTable;
}Touch_Sensor_t;


#define CREATE_TOUCH_SENSOR(name,tp_num) {#name,tp_num,0,1,0,0,0,true,false,0}
#define CREATE_TOUCH_SENSOR_CAL(name,tp_num,table) {#name,tp_num,0,1,0,0,500,true,true,#table}

typedef struct
{
    Touch_Sensor_t *sensor;
    int length;
}tTouch_Sensor_Obj;

typedef struct 
{
    char message[64];
    uint8_t length;
}tTouchPad_msg;

//Enable touch pad sensor 
 //esp_err_t InitTPSensors(Touch_Sensor_t * sensor, int len, uint16_t masking,uint32_t filterPeriod);
 esp_err_t InitTPSensors(Touch_Sensor_t * sensor, int len, uint32_t filterPeriod);
 esp_err_t ReadTPSensors(Touch_Sensor_t * sensor, int len);
 int16_t ScaleSensor(Touch_Sensor_t *sensor);
 void EnableScale(Touch_Sensor_t *sensor, tCalibration_Table *tbl, uint16_t threshold);
 void vTouchReadTask(void *pvParameter);
 QueueHandle_t InitTouchPadTask(tTouch_Sensor_Obj * touch_obj);   

#endif