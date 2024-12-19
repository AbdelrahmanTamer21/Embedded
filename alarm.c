#include "alarm.h"

void alarm_init()
{
    gpio_init(ALARM_PIN);
    gpio_set_dir(ALARM_PIN, GPIO_OUT);
}

void alarm_on(){
    gpio_put(ALARM_PIN, 1);
}

void alarm_off(){
    gpio_put(ALARM_PIN, 0);
}