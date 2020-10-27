#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "string.h"
//Marcos y definiciones
#define DATA_PIR GPIO_NUM_4
#define BUZZER GPIO_NUM_22
#define PULSADOR GPIO_NUM_0
// Estructura y variables
uint8_t cont_alarma = 0;
struct ALARMA{
	uint8_t anio;
	uint8_t mes;
	uint8_t fecha;
	uint8_t seg;
	uint8_t min;
	uint8_t hora;
	uint8_t formato_hora;
	char dia[10];
	char ampm[2];
}Registros_alarma[20];
extern char* leer_ds3231(uint8_t* seg, uint8_t* min, uint8_t* hor,
		uint8_t* fec, uint8_t* mes, uint8_t* ani, uint8_t* ampm);

void TareaPir(void* P){
	gpio_set_direction(DATA_PIR|PULSADOR, GPIO_MODE_INPUT);
	gpio_set_direction(BUZZER, GPIO_MODE_OUTPUT);
	leer_ds3231(&Registros_alarma[0].seg,
			&Registros_alarma[0].min,
			&Registros_alarma[0].hora,
			&Registros_alarma[0].fecha,
			&Registros_alarma[0].mes,
			&Registros_alarma[0].anio,
			&Registros_alarma[0].formato_hora);
	for(;;){
		if(gpio_get_level(DATA_PIR)){
			//OBTEMER LECTURA DEL RTC
			strcpy(Registros_alarma[cont_alarma].dia,
				leer_ds3231(&Registros_alarma[cont_alarma].seg,
						&Registros_alarma[cont_alarma].min,
						&Registros_alarma[cont_alarma].hora,
						&Registros_alarma[cont_alarma].fecha,
						&Registros_alarma[cont_alarma].mes,
						&Registros_alarma[cont_alarma].anio,
						&Registros_alarma[cont_alarma].formato_hora));
			//Obtener siglas formato hora
			if(Registros_alarma[cont_alarma].formato_hora)
				strcpy(Registros_alarma[cont_alarma].ampm, "PM");
			else
				strcpy(Registros_alarma[cont_alarma].ampm, "AM");
			//Encender buzzer por 2 seg
			gpio_set_level(BUZZER,1);
			vTaskDelay(2000/portTICK_PERIOD_MS);
			gpio_set_level(BUZZER,1);
			//*************************
			cont_alarma++;
			if (cont_alarma >= 20) cont_alarma = 0;
		}
		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}

void Pulsador (void* P){
	for(;;){
		if(gpio_get_level(PULSADOR) == 0){
			vTaskDelay(750/portTICK_PERIOD_MS);
			//REPORTAR ALARMAS
			for(int i=0; i<cont_alarma; i++){
			printf("********** Alarmas %d **********\n\r", i);
			printf("Fecha: %x-%x-20%x\n\r", Registros_alarma[i].fecha,
					Registros_alarma[i].mes, Registros_alarma[i].anio);
			printf("Hora: %x:%x:%x %c%c\n\r", Registros_alarma[i].hora,
					Registros_alarma[i].min, Registros_alarma[i].seg,
					Registros_alarma[i].ampm[0], Registros_alarma[i].ampm[1]);
			printf("*******************************************");
			}
		}
		vTaskDelay(15/portTICK_PERIOD_MS);
	}
}

