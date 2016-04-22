/* 
 * File:   BME280.h
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
 
#ifndef BME280_H
#define BME280_H
 
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <string.h>
#include <math.h>

#define SCK LATCbits.LC3 // SPI Clock 
#define SDO LATCbits.LC5 // Serial Data Out
#define SDI LATCbits.LC4 // Serial Data In
#define CSB LATAbits.LA5 // Chip Select

#define CTRL_HUM 0xF2  // BME280 humidity register settings
#define CONTROL 0xF4 //BME280 control register
#define TEMP_REG 0xFA // BME280 temperature reg
#define HUM_REG 0xFD // BME280 humidity reg
#define PRESS_REG 0xF7 // BME280 pressure reg
#define dig_T1_REG 0x88 // BME280 temp calibration coefficients...
#define dig_T2_REG 0x8A
#define dig_T3_REG 0x8C
#define dig_H1_REG 0xA1 // BME280 humidity calibration coefficients...
#define dig_H2_REG 0xE1
#define dig_H3_REG 0xE3
#define dig_H4_REG 0xE4
#define dig_H5_REG 0xE5
#define dig_H6_REG 0xE7
#define dig_P1_REG 0x8E // BME280 pressure calibration coefficients...
#define dig_P2_REG 0x90
#define dig_P3_REG 0x92
#define dig_P4_REG 0x94
#define dig_P5_REG 0x96
#define dig_P6_REG 0x98
#define dig_P7_REG 0x9A
#define dig_P8_REG 0x9C
#define dig_P9_REG 0x9E

signed long int t_fine; // global variable 
//Forces a sample of the BME280. Also sets oversampling for humidity, temp and press = 1.
//Consult the BME280 Datasheet to change these options. 
void BME280_init();

//Write a byte to a register via SPI 
void writeSPI(char, char);

// return a unsigned 16-bit value 
unsigned int readSPI16bit(char);

// return a unsigned 8-bit value
unsigned char readSPI8bit(char);
// returns a unsigned 16-bit (little endian) 
unsigned int readSPI16bit_u_LE(char);

// returns a unsigned 16-bit (little endian)
signed int readSPI16bit_s_LE(char); 

// get temperature and returns it in Celsius
float readTemp(); 

// gets RH humidity and returns it as a percentage
float readHumidity();

// gets pressure and returns it in kPa.
float readPressure(); 

#endif // BME280_H