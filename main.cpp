// Started from http://elinux.org/Interfacing_with_I2C_Devices
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
using namespace std;
enum Qt1070Register {
  CHIP_ID = 0,
  FIRMWARE = 1,
  KEY_STATUS = 3,
  RESET = 57
};

enum TouchBerryPiShieldKey {
 // UP = 0x28,
 // DOWN = 0x31,
 // LEFT = 0x22,
 // RIGHT = 0x26,
 // A = 0x14,
 // B = 0x10,
 // X = 0x40

 UP = 0x04,
 DOWN = 0x08,
 LEFT = 0x01,
 RIGHT = 0x02,
 A = 0x20,
 B = 0x10,
 X = 0x40
};


bool write_register_address(int file, char registerAddress) {
  char writeBuffer[] = { registerAddress };

  // First write address
  if (write(file, writeBuffer, sizeof(writeBuffer)) != sizeof(writeBuffer)) {
      printf("Failed to write to the i2c bus.\r\n");
      return false;
  }

  return true;
}


bool read_register(int file, char registerAddress, char * value) {

    if (!write_register_address(file, registerAddress)) {
      return false;
    }

   // sleep(1);
    this_thread::sleep_for(std::chrono::milliseconds(250));
    char readBuffer[1];
    if (read(file, readBuffer, sizeof(readBuffer)) != sizeof(readBuffer)) {
        printf("Failed to read from the i2c bus.\r\n");
        return false;
    } else {
        *value = readBuffer[0];
    }
    return true;
}

bool reset_device(int file) {
    char writeBuffer[] = { RESET };

    // First write address
    if (write(file, writeBuffer, sizeof(writeBuffer)) != sizeof(writeBuffer)) {
        printf("Failed to write to the i2c bus.\r\n");
        return false;
    }
    return true;
}


int main(void) {
    int file;
    char filename[40];
    int slaveAddress = 0x1B;        // The 7-bit I2C address of the qt1070 emulator device

    sprintf(filename, "/dev/i2c-1");
    if ((file = open(filename, O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    } else {
      printf("Succesfully openend i2c-1 bus\r\n");
    }

    if (ioctl(file, I2C_SLAVE, slaveAddress) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    } else {
      printf("Succesfully aquired i2c-1 bus\r\n");
    }

    sleep(1);

    printf("Writing register address\r\n");
    write_register_address(file, FIRMWARE);

    sleep(1);


    char chipid = 0x00;
    if (!read_register(file, CHIP_ID, &chipid)) {
      printf("Failed to read chip id\r\n");
      return -1;
    }
    printf("Chip id = 0x%x\r\n", chipid);

    //return 0;
    // BREAK ///////////////////////////////////////////////////////////////////

    sleep(1);

    char firmware = 0x00;
    if (!read_register(file, FIRMWARE, &firmware)) {
      printf("Failed to read firmware version\r\n");
      return -1;
    }
    printf("Firmware version = 0x%x\r\n", firmware);

    sleep(1);

    if (!reset_device(file)) {
      printf("Failed to reset device\r\n");
      return -1;
    }
    printf("Device is resetting\r\n");
    sleep(1);
    
    bool keepReading = true;
    while (keepReading) {
     string line = "";
     ofstream myfile ("/tmp/touch/thumper");
      char keys = 0x00;
      if (!read_register(file, KEY_STATUS, &keys)) {
        printf("Failed to read firmware version\r\n");
      }
      // printf("Key status = 0x%x\r\n", keys);
      // int i = 0; 
      // bool kleur = 0;
      // bool ambulance = true;	
      switch (keys) {
        case UP:
          printf("UP \r\n");
	  line =  "forward";
          break;
        case LEFT:
          printf("LEFT \r\n");
 	 line =  "left";
          break;
        case DOWN:
          printf("DOWN \r\n");
	  line =  "reverse";
          break;
        case RIGHT:
          printf("RIGHT \r\n");
	  line =  "right";
          break;
        case A:
          printf("A \r\n");
 	  line =  "shift";
          break;
        case B:
          printf("B \r\n");
 	  line =  "dim";
	  //ambulance = false;
          break;
        case X:
          printf("X \r\n");
	 // kleur = true;
	 line = "alarm";
          break;
      }
    /*
     if(kleur){
	line = "strobe_rood";
	kleur != kleur;
     }else{
	line = "strobe_blauw";
	kleur != kleur;
     }
    */
      fflush(stdout);
	if (myfile.is_open())
	{
  	 myfile << line;
  	// myfile << " ";
	 printf("Geschreven naar file \r\n");
  	 myfile.close();
	}
	 else cout << "Unable to write to mbed \r\n";
     // sleep(1);
	this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    printf("Done\r\n");

    return 0;
}
