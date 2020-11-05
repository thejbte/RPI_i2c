
#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "MAX17048.h"
#include "sht30.h"
#include <unistd.h>


 
 //gcc i2c_test.c MAX17048.c sht30.c -lbcm2835 -D _DRIVER=1 -o test
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

    //bcm2835_i2c_setSlaveAddress(slave_address);
    //bcm2835_i2c_setClockDivider(clk_div);

    #if _DRIVER == 2 
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

#if _DRIVER == 1
    SHT30_Init(&SHT30 ,Write_I2C,Read_I2C,SHT30_ADDR_SLAVE_7BITS);
while(1){
    SHT30_ReadData(&SHT30);
    printf("Relative Humidity : %.2f RH \n", SHT30.humidity);
	printf("Temperature in Celsius : %.2f C \n", SHT30.temperature);
    sleep(1);
}
        bcm2835_close();
    printf("... don e!\n");
bcm2835_i2c_end();
        return 0;
#endif

  #if _DRIVER == 3
    MAX17048_Init(&MAX17048 ,Write_I2C,Read_I2C,MAX17048_ADDR_SLAVE); //MAX17048_ADDR_SLAVE
    uint8_t buffer[2];
   	char config[2] = {0};
	config[0] = MAX17048_VERSION;

    
    bcm2835_i2c_write(config,1);
    sleep(1);
    char data[6] = {0};
	bcm2835_i2c_read(data, 2);
    	int temp = (data[0] <<8  | data[1]);

		// Output data to screen
		printf("version %02X \n", temp);


    bcm2835_i2c_end();   
     bcm2835_close();
    printf("... don e!\n");
	return 0;
  #endif
#if _DRIVER == 0   	
    uint8_t buffer[2];
   	char config[2] = {0};
	config[0] = 0x2C;
	config[1] = 0x06;

    bcm2835_i2c_write(config,2);
    sleep(1);
    char data[6] = {0};
	bcm2835_i2c_read(data, 6);
    	int temp = (data[0] * 256 + data[1]);
		float cTemp = -45 + (175 * temp / 65535.0);
		float fTemp = -49 + (315 * temp / 65535.0);
		float humidity = 100 * (data[3] * 256 + data[4]) / 65535.0;

		// Output data to screen
		printf("Relative Humidity : %.2f RH \n", humidity);
		printf("Temperature in Celsius : %.2f C \n", cTemp);
		printf("Temperature in Fahrenheit : %.2f F \n", fTemp);

    bcm2835_i2c_end();   
     bcm2835_close();
    printf("... don e!\n");
	return 0;
#endif

}
 

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