/*!
 * ********************************************************************************
 * @file i2c_test.c
 * @author Esp.Ing Julian Bustamante N
 * @email thejbte@gmail.com
 * @date 5 Nov, 2020
 * @brief Read fuel gauge and temperature/humidity  sensors 
 **********************************************************************************/


#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "MAX17048.h"
#include "sht30.h"
#include <unistd.h>
/*
#GPIOs location
GPIOS=/sys/class/gpio

#Buzzer
echo 19 > $GPIOS/export
echo out > $GPIOS/gpio19/direction

#RGB
echo 4 > $GPIOS/export
echo out > $GPIOS/gpio4/direction
echo 5 > $GPIOS/export
echo out > $GPIOS/gpio5/direction
echo 6 > $GPIOS/export
echo out > $GPIOS/gpio6/direction
*/
 
 // download bcm2835 
 //gcc i2c_test.c MAX17048.c sht30.c -lbcm2835 -D _DRIVER=1 -o test

 //i2cdetec -y 1
 //#undef _DRIVER 

 
uint16_t clk_div = BCM2835_I2C_CLOCK_DIVIDER_148;
uint8_t slave_address =0x44;// MAX17048_ADDR_SLAVE;

 
void Write_I2C(uint8_t Address, void *data, uint8_t amount);
void Read_I2C(uint8_t Address, void *Data, uint8_t amount, uint8_t Sizereg) ;


 
 MAX17048_t MAX17048;
 SHT30_t SHT30;
int main(int argc, char **argv) {
    

    printf("Running.. ... \n");
    if (!bcm2835_init())
    {
      printf("bcm2835_init failed. Are you running as root??\n");
      return 1;
    }
      
    // I2C begin if specified    
      if (!bcm2835_i2c_begin())
      {
        printf("bcm2835_i2c_begin failed. Are you running as root??\n");
        return 1;
      }

/*compilar con flags*/
  #if _DRIVER == 1 
    MAX17048_Init(&MAX17048 ,Write_I2C,Read_I2C,0x36); //MAX17048_ADDR_SLAVE
    MAX17048_Compensation(&MAX17048, MAX17048_RCOMP0);
    MAX17048_SleepEnable(&MAX17048);
    MAX17048_Sleep(&MAX17048, 0);
    MAX17048_QStart(&MAX17048);

    while(1){
      printf("Voltage : %d mV \n", MAX17048_Voltage(&MAX17048));
      printf("Soc in %% : %d %% \n",   MAX1708_SOC_Int(&MAX17048));
      sleep(1);
    }
    bcm2835_close();
    printf("... done!\n");
    bcm2835_i2c_end();
    return 0;
#endif


}
 
/*===============WRAPPERS==========================================*/
 void Write_I2C(uint8_t Address, void *data, uint8_t amount) {
	  uint8_t *DatatoSend = (uint8_t *)data;
	  uint16_t Bytes;
    bcm2835_i2c_setSlaveAddress(Address);
    bcm2835_i2c_write(DatatoSend,amount);
  }

  void Read_I2C(uint8_t Address, void *Data, uint8_t amount, uint8_t Sizereg) {
	  uint8_t *DatatoSend = (uint8_t *)Data;
	  uint16_t Bytes;
    bcm2835_i2c_setSlaveAddress(Address);
    bcm2835_i2c_write(DatatoSend,Sizereg);
    usleep(0.1*1E6);/*without this it doesn't work*/
    bcm2835_i2c_read(DatatoSend,amount);
  }
