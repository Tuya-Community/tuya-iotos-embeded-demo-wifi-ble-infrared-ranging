#include "uni_log.h"
#include "tuya_gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "uni_thread.h"

#include "app_adc.h"
#include "tuya_infrared_ranging.h"
#include "tuya_dp_process.h"

UINT16_T adc_value;

STATIC VOID_T ad_2_distance(UINT16_T adc_value)
{
    FLOAT_T distances, voltage;
    
    voltage = ((adc_value*1.0)/4096) * 3.3;
    
    if (voltage > 2.33) {
        sensor_state.distances_value = 0;
        return;
    }

    if (voltage > 1.625) {
        sensor_state.distances_value = (21.36/(voltage-0.194));
    } else if (voltage > 1.33) {
        sensor_state.distances_value = (17.35/(voltage-0.463));
    } else {
        sensor_state.distances_value = (24.8/(voltage-0.09));
    }
}

VOID_T get_distance_task(VOID_T)
{
    while(1) {
        get_adc_value(&adc_value);
        PR_NOTICE("alcohol_adc_value:%d", adc_value);
        //sensor_state.distances_value = adc_value;
        ad_2_distance(adc_value);
        hw_report_all_dp_status();
        tuya_hal_system_sleep(1000);
    }
}

VOID_T infrared_ranging_init(VOID_T)
{
    adc_init();
    tuya_hal_thread_create(NULL, "get value", 512 * 4, TRD_PRIO_2, get_distance_task, NULL);
}
