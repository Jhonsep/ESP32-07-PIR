#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c.h"
#include "string.h"
// Macros y definiciones
#define ds3231 0x68
#define SecZ 0x00
#define MinZ 0x55
#define HorZ 0x62 //2 pm
#define DiaZ 0x02
#define FecZ 0x27
#define MesZ 0x10
#define AniZ 0x20

static void enviar_ds3231(){
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (ds3231<<1)|I2C_MASTER_WRITE,1);
	i2c_master_write_byte(cmd, 0x0,1);
	i2c_master_write_byte(cmd, SecZ,1);
	i2c_master_write_byte(cmd, MinZ,1);
	i2c_master_write_byte(cmd, HorZ,1);
	i2c_master_write_byte(cmd, DiaZ,1);
	i2c_master_write_byte(cmd, FecZ,1);
	i2c_master_write_byte(cmd, MesZ,1);
	i2c_master_write_byte(cmd, AniZ,1);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
}

char* leer_ds3231(uint8_t* seg, uint8_t* min, uint8_t* hor, uint8_t* fec, uint8_t* mes, uint8_t* ani, uint8_t* ampm){
	char* diax[] = {"domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes" "Sabado"};
	uint8_t segundos, minutos, horas, dia, fecha, meses, anios;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (ds3231<<1)|I2C_MASTER_WRITE,1);
	i2c_master_write_byte(cmd, 0x0,1);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (ds3231<<1)|I2C_MASTER_READ,1);
	i2c_master_read(cmd, &segundos,1,0);
	i2c_master_read(cmd, &minutos,1,0);
	i2c_master_read(cmd, &horas,1,0);
	i2c_master_read(cmd, &dia,1,0);
	i2c_master_read(cmd, &fecha,1,0);
	i2c_master_read(cmd, &meses,1,0);
	i2c_master_read(cmd, &anios,1,1);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	*seg = segundos;
	*min = minutos;
	*hor = 0x1F & horas;
	*fec = fecha;
	*mes = meses;
	*ani = anios;
	if(horas & 0x20)*ampm = 1; else *ampm = 0;
	return diax[dia-1];
}
void TareaDS3231(void* P){
	uint8_t seg, min, hor, fec, mes, ani, ampm;
	char diax[10],pmam[2];
	enviar_ds3231();
	for(;;){
		strcpy(diax,leer_ds3231(&seg, &min, &hor, &fec, &mes, &ani, &ampm));
		if (ampm)
			memcpy(pmam,"PM", 2);
		else
			memcpy(pmam, "AM", 2);
		printf("Fecha %s %x-%x-20%x\n\r", diax, fec, mes, ani);
		printf("Hora: %x:%x:%x %C%c\n\r", hor, min, seg, pmam[0], pmam[1]);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}
