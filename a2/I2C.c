#include <stdbool.h>
#include "HistoryArrayAlloc.h"
#include "I2C.h"
#include "Utility.h"
#include "Global.h"
#include "periodTimer.h"
#include <pthread.h>


static int displayLeft = 0;
static int displayRight = 0;

int initI2cBus(char* bus, int address)
{
    int i2cFileDesc = open(bus, O_RDWR);
    int result = ioctl(i2cFileDesc, I2C_SLAVE, address);
    if (result < 0) {
        perror("I2C: Unable to set I2C device to slave address.");
        exit(1);
    }
    return i2cFileDesc;
}

void writeI2cReg(int i2cFileDesc, unsigned char regAddr,
                        unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

unsigned char readI2cReg(int i2cFileDesc, unsigned char regAddr)
{
// To read a register, must first write the address
    int res = write(i2cFileDesc, &regAddr, sizeof(regAddr));
    if (res != sizeof(regAddr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }
// Now read the value and return it
    char value = 0;
    res = read(i2cFileDesc, &value, sizeof(value));
    if (res != sizeof(value)) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }
    return value;
}

void WriteNumberToI2C(int i2cFileDesc, int number){
        if (number == 0){
            writeI2cReg(i2cFileDesc, REG_OUTA, 0xA1);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x86);
            return;
        }
        else if(number == 1){
            writeI2cReg(i2cFileDesc, REG_OUTA, 0x80);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x02);
            return;
        }
        else if(number == 2){
            writeI2cReg(i2cFileDesc, REG_OUTA, 0x31);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x0E);
            return;
        }
        else if(number == 3){
            writeI2cReg(i2cFileDesc, REG_OUTA, 0xB0);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x0E);
            return;
        }
        else if(number == 4){
            writeI2cReg(i2cFileDesc, REG_OUTA, 0x90);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x8A);
            return;
        }
        else if(number == 5){
            writeI2cReg(i2cFileDesc, REG_OUTA, 0xB0);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x8C);
            return;
        }
        else if(number == 6){
            writeI2cReg(i2cFileDesc, REG_OUTA, 0xB1);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x8C);
            return;
        }
        else if(number == 7){
            writeI2cReg(i2cFileDesc, REG_OUTA, 0x80);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x06);
            return;
        }

        else if(number == 8){
            writeI2cReg(i2cFileDesc, REG_OUTA, 0xB1);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x8E);
            return;
        }
        else if(number == 9){
            writeI2cReg(i2cFileDesc, REG_OUTA, 0x90);
            writeI2cReg(i2cFileDesc, REG_OUTB, 0x8E);
            return;
        }
}


void* setLED(){
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);


    int rightGPIO = open("/sys/class/gpio/gpio61/value",O_RDWR);

    int leftGPIO = open("/sys/class/gpio/gpio44/value",O_RDWR);

    while(isRunning){

        write(leftGPIO,"0", sizeof("0"));
        write(rightGPIO,"0", sizeof("0"));

        WriteNumberToI2C(i2cFileDesc, displayLeft);
        write(leftGPIO,"1", sizeof("1"));
        sleepForMs(5);

        write(leftGPIO,"0", sizeof("0"));
        write(rightGPIO,"0", sizeof("0"));

        WriteNumberToI2C(i2cFileDesc, displayRight);
        write(rightGPIO,"1", sizeof("1"));
        sleepForMs(5);

    }
    return NULL;
}

void* RunI2C()
{
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
    while(isRunning){
        writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
        writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
        int displayNumber = GetNumOfDips();

        if(displayNumber > 99){
            displayNumber = 99;
        }

        displayLeft = displayNumber%10;
        displayRight = (displayNumber/10) %10;

        sleepForMs(100);
    }
    close(i2cFileDesc);
    return 0;
}

