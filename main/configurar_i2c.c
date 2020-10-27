#include "driver/i2c.h"
#include "driver/gpio.h"
//Macros y definiciones
#define PinSDA 19
#define PinSCL 18
#define i2c_clock 100000

void iniciar_i2c(){
	i2c_config_t configuracion;
	configuracion.mode = I2C_MODE_MASTER;
	configuracion.sda_io_num = PinSDA;
	configuracion.scl_io_num = PinSCL;
	configuracion.sda_pullup_en = GPIO_PULLUP_ENABLE;
	configuracion.scl_pullup_en = GPIO_PULLUP_ENABLE;
	configuracion.master.clk_speed = i2c_clock;
	i2c_param_config(I2C_NUM_0, &configuracion);
	i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0,0,0);
}
