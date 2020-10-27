//Archivos Espressif API
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//Archivos propios
#include "configurar_i2c.h"
#include "pir.h"
#include "ds3231.h"
//Macros y definiciones
#define Pila 1024
//variables globales
void app_main (){
	iniciar_i2c();
	xTaskCreatePinnedToCore(TareaPir, "Sensor_PIR", Pila*2, NULL, 2, NULL, 0);
	xTaskCreatePinnedToCore(Pulsador, "Pulsador", Pila*2, NULL, 2, NULL, 1);
}
