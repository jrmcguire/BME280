/* 
 * File:   BME280.c
 * Author: Jason McGuire (j.mcguire.2015@ieee.org)
 * Description: A library for a BME280 Weather Sensor for PIC18F using SPI
 * See example.c for implementation of the library
 * Sample Product: http://www.adafruit.com/products/2652
 * Created on October 13, 2015, 9:08 AM
 
License Information:
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

 */
 
 #include "BME280.h"

// Available Functions:
void BME280_init(); // Starts the BME280. Must use each time you want a new sample. 
void writeSPI(char, char); // Write a byte to a register via SPI
unsigned int readSPI16bit(char); // return a unsigned 16-bit value 
unsigned char readSPI8bit(char); // return a unsigned 8-bit value
unsigned int readSPI16bit_u_LE(char); // returns a unsigned 16-bit (little end)
signed int readSPI16bit_s_LE(char); // returns a unsigned 16-bit (little end)
float readTemp(); // get temperature and returns it in Celsius
float readHumidity(); // gets RH humidity and returns it as a percentage
float readPressure(); // gets pressure and returns it in kPa.


// Starts the BME280 sensor and waits for first sample. MUST be ran each time
// you wish to get a reading. 
void BME280_init(){
    writeSPI(CTRL_HUM, 0x01); // Humidity oversampling = 1
    writeSPI(CONTROL, 0x25); // Forced mode, Temp/Press oversampling = 1
}

unsigned char readSPI8bit(char reg){
    unsigned char trash;
    CSB = 0;
    SSPBUF = reg;
    SSPOV = 0;
    while ((SSPSTAT & 0x01) == 0);  // wait for transmission
    trash = SSPBUF; // clear BF flag
    SSPBUF = 0; // Send garbage
    while ((SSPSTAT & 0x01) == 0);  // wait for byte
    CSB = 1;
    return SSPBUF;       // clear flag by reading buffer
}

unsigned int readSPI16bit_u_LE(char reg){ // read 16-bits unsigned little endian
    unsigned int val;
    val = readSPI16bit(reg); 
    return (val >> 8) | (val << 8); // swap upper and lower regs
}

signed int readSPI16bit_s_LE(char reg){ // read 16-bit signed little endian
    return (signed int)readSPI16bit_u_LE(reg);
}



unsigned int readSPI16bit(char reg){
    unsigned int val;
    val = readSPI8bit(reg); // shift in MSB
    val = val << 8 | readSPI8bit(reg+1); // shift in LSB
    return val;
}

void writeSPI(char reg, char data){
    signed char trash;
    CSB = 0;
    SSPBUF = (reg & ~0x80); // write cmd bit 7 = 0
    SSPOV = 0;
    while ((SSPSTAT & 0x01) == 0);  // wait for transmission
    trash = SSPBUF; // clear BF flag
    SSPBUF = data; 
    SSPOV = 0;
    while ((SSPSTAT & 0x01) == 0);  // wait for transmission
    CSB = 1;
    trash = SSPBUF; // clear BF flag 
    return;
}

float readTemp(void){
    // Calibration Coefficients:
    unsigned long int dig_T1 = readSPI16bit_u_LE(dig_T1_REG); 
    signed long int dig_T2 = readSPI16bit_s_LE(dig_T2_REG);
    signed long int dig_T3 = readSPI16bit_s_LE(dig_T3_REG);
    
    // Temperature Raw ADC:
    unsigned long int adc_T = readSPI16bit(TEMP_REG);
    adc_T <<= 8; // move in XLSB register
    adc_T |= readSPI8bit(TEMP_REG + 2);
    adc_T >>= 4; // Only uses top 4 bits of XLSB register 

    // From BME280 data sheet: 
    signed long int var1  = ((((adc_T>>3) - (dig_T1 <<1))) *
	   (dig_T2)) >> 11;
  
    signed long int var2  = (((((adc_T>>4) - (dig_T1)) *
	     ((adc_T>>4) - (dig_T1))) >> 12) *
	     (dig_T3)) >> 14;

    t_fine = var1 + var2;
 
    float T = (t_fine * 5 + 128) >> 8;
    return T/100;
}

float readHumidity(void){
    // Calibration Coefficients:
    unsigned int dig_H1 = readSPI8bit(dig_H1_REG);
    signed long int dig_H2 = readSPI16bit_s_LE(dig_H2_REG);
    unsigned int dig_H3 = readSPI8bit(dig_H3_REG);
    signed long int dig_H4 = (readSPI8bit(dig_H4_REG) << 4) | (readSPI8bit(dig_H4_REG+1) & 0xF);
    signed long int dig_H5 = (readSPI8bit(dig_H5_REG+1) << 4) | (readSPI8bit(dig_H5_REG) >> 4);
    signed int dig_H6 = (signed int) readSPI8bit(dig_H6_REG);
    
    // Humidity raw ADC:
    unsigned long int adc_H = readSPI16bit(HUM_REG);
    
    // from BME280 data sheet:
    unsigned long int v_x1_u32r;
    
    v_x1_u32r = t_fine - 76800;
    
    v_x1_u32r = (((((adc_H << 14) - ((dig_H4) << 20) - ((dig_H5) * v_x1_u32r))
            + (16384)) >> 15) * (((((((v_x1_u32r * (dig_H6)) >> 10) *
		    (((v_x1_u32r * (dig_H3)) >> 11) + (32768))) >> 10) +
		  (2097152)) * (dig_H2) + 8192) >> 14));
    
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
			     (dig_H1)) >> 4));
    
    v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
    v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
    float humidity = (v_x1_u32r>>12);
    return  humidity / 1024.0;
}

float readPressure(void){
    // Calibration Coefficients
    unsigned long int dig_P1 = readSPI16bit_u_LE(dig_P1_REG);
    signed long int dig_P2 = readSPI16bit_s_LE(dig_P2_REG);
    signed long int dig_P3 = readSPI16bit_s_LE(dig_P3_REG);
    signed long int dig_P4 = readSPI16bit_s_LE(dig_P4_REG);
    signed long int dig_P5 = readSPI16bit_s_LE(dig_P5_REG);
    signed long int dig_P6 = readSPI16bit_s_LE(dig_P6_REG);
    signed long int dig_P7 = readSPI16bit_s_LE(dig_P7_REG);
    signed long int dig_P8 = readSPI16bit_s_LE(dig_P8_REG);
    signed long int dig_P9 = readSPI16bit_s_LE(dig_P9_REG);

    // Pressure Raw ADC:
    unsigned long int adc_P = readSPI16bit(PRESS_REG);
    adc_P <<= 8; // move in XLSB register
    adc_P |= readSPI8bit(PRESS_REG + 2);
    adc_P >>= 4; // Only uses top 4 bits of XLSB register 
    
    // from BME280 datasheet 
    signed long int var1 = ((signed long int)t_fine >> 1);
    var1 = var1 - (signed long int)64000;
    signed long int var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((signed long int)dig_P6);
    var2 = var2 + ((var1*((signed long int)dig_P5))<<1);
    var2 = (var2>>2)+(((signed long int)dig_P4)<<16);
    var1 = (((dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((signed long int)dig_P2) * var1)>>1))>>18;
    var1 =((((32768+var1))*((signed long int)dig_P1))>>15);
    if (var1 == 0){
        return 0; // avoid exception caused by division by zero
    }
    unsigned long int p = (((unsigned long int)(((signed long int)1048576)-adc_P)-(var2>>12)))*3125;
    if (p < 0x80000000){
        p = (p << 1) / ((unsigned long int)var1);
    }
    else
    {
        p = (p / (unsigned long int)var1) * 2;
    }
    var1 = (((signed long int)dig_P9) * ((signed long int)(((p>>3) * (p>>3))>>13)))>>12;
    var2 = (((signed long int)(p>>2)) * ((signed long int)dig_P8))>>13;
    p = (unsigned long int)((signed long int)p + ((var1 + var2 + dig_P7) >> 4));
    float pressure = p/1000;
    return pressure;
}