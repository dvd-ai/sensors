/*
 * Copyright (c) 2016-2020, Texas Instruments Incorporated
 * All rights reserved.
/*
 *    ======== i2cbme280.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* POSIX Header files */
#include <unistd.h>

/* Driver Header files */
#include <ti/drivers/I2C.h>
#include <ti/display/Display.h>

/* Module Header */
#include <ti/sail/bme280/bme280.h>

/* Driver configuration */
#include "ti_drivers_config.h"

s32    g_s32ActualTemp   = 0;
u32    g_u32ActualPress  = 0;
u32    g_u32ActualHumity = 0;

I2C_Handle      i2c;
I2C_Params      i2cParams;

static Display_Handle display;

extern s32 bme280_data_readout_template(I2C_Handle i2cHndl);

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    /* Call driver init functions */
    I2C_init();

    /* Open the HOST display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL) {
        while (1);
    }
    Display_print0(display, 0, 0, "Starting the i2cbme280 sensor example...\n\n");

    I2C_Params_init(&i2cParams);
    i2cParams.bitRate = I2C_400kHz;
    i2cParams.transferMode = I2C_MODE_BLOCKING;
    i2cParams.transferCallbackFxn = NULL;
    i2c = I2C_open(CONFIG_I2C_BME, &i2cParams);
    if (i2c == NULL) {
        Display_print0(display, 0, 0, "Error Initializing I2C\n");
    }
    else {
        Display_print0(display, 0, 0, "I2C Initialized!\n");
    }

    /* Initialize the BME Sensor */
    if(BME280_INIT_VALUE != bme280_data_readout_template(i2c))
    {
        Display_print0(display, 0, 0, "Error Initializing bme280\n");
    }
    bme280_set_power_mode(BME280_NORMAL_MODE);

    while(1)
    {
        if(BME280_INIT_VALUE == 
                bme280_read_pressure_temperature_humidity(&g_u32ActualPress,
                    &g_s32ActualTemp, &g_u32ActualHumity))
        {
            Display_print3(display, 0, 0, "%u KPa(Pressure), %u DegC(Temp), %u %%RH(Humidity)\n",
                        g_u32ActualPress/1000, g_s32ActualTemp/100,
                        g_u32ActualHumity/1000);
        }
        else
        {
            Display_print0(display, 0, 0, "Error reading from the bme280 sensor\n");
        } 

        sleep(10);
    }
}
