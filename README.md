# Touch Pad Read Example
Based on Touch Pad Read Example from ESP32 IDF 
Change the number of sensors to be only 2 sensor. Create the structure of Touch Read sensor.

## This code is used for reading on my DIY Water Level sensor project 
## You can find information how to build the DIY Capacitive Water Level sensor her -> Link:

Read and display raw values or IIR filtered values from capacitive touch pad sensors. 

Once configured, ESP32 is continuously measuring capacitance of touch pad sensors. Measurement is reflected as numeric value inversely related to sensor's capacitance. The capacitance is bigger when sensor is touched with a finger and the measured value smaller. In opposite situation, when finger is released, capacitance is smaller and the measured value bigger.

To detect when a sensor is touched and when not, each particular design should be calibrated by obtaining both measurements for each individual sensor. Then a threshold between both values should be established. Using specific threshold, API is then able to distinguish whether specific sensor is touched or released.

ESP32 supports reading up to ten capacitive touch pad sensors T0 - T9, connected to specific GPIO pins. For information on available pins please refer to [Technical Reference Manual](https://espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf). Application initializes all ten sensor pads. Then in a loop reads sensors T0 - T9 and displays obtained values (after a colon) on a serial terminal:

```
Touch Sensor filter mode read, the output format is: 
Touchpad num:[CHxx, raw_data]

[CH0,dddd|CH3,dddd|...|CHn,dddd]

If you use enable scale the output will be
[CHxx,scaleddata]

If you want to modify the code to read another touch channel please modify this line locate on /main/esp32/main.c
line 20 --> static Touch_Sensor_t g_tp_sensor[] ={CREATE_TOUCH_SENSOR("CH0",0),CREATE_TOUCH_SENSOR("CH3",3)};

for example if you want to enable CH6 and CH7  touch sensor
static Touch_Sensor_t g_tp_sensor[] ={CREATE_TOUCH_SENSOR("CH0",0),CREATE_TOUCH_SENSOR("CH3",3),CREATE_TOUCH_SENSOR("CH6",6),CREATE_TOUCH_SENSOR("CH7",7))};

## Reference Information

For hardware and firmware design guidelines on ESP32 touch sensor system, please refer to [Touch Sensor Application Note](https://github.com/espressif/esp-iot-solution/blob/master/documents/touch_pad_solution/touch_sensor_design_en.md), where you may find comprehensive information on how to design and implement touch sensing applications, such as linear slider, wheel slider, matrix buttons and spring buttons.

There is another similar example that demonstrates how to perform simple calibration and trigger an interrupt when a pat is touched - see [touch_pad_interrupt](../touch_pad_interrupt).

See the README.md file in the upper level 'examples' directory for more information about examples.
