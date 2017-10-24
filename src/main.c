/* Copyright 2014, ChaN
 * Copyright 2016, Matias Marando
 * Copyright 2016, Eric Pernia
 * Copyright 2017, Agustin Bassi
 * All rights reserved.
 *
 * This file is part of Workspace.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==================[inlcusiones]============================================*/

#include "main.h"   		// <= su propio archivo de cabecera

#include "sapi.h"     		// <= Biblioteca sAPI

#include "ff.h"       		// <= Biblioteca FAT FS
#include "fatfs_stubs.h"	// En este archivo se setea si sacar datos por SD o UART

/*==================[definiciones y macros]==================================*/

#define FILENAME "datalog.txt"

/*==================[definiciones de datos internos]=========================*/

static FATFS FileSystem;  	// <-- FatFs work area needed for each volume
static FIL   File;          // <-- File object needed for each open file

/*==================[definiciones de datos externos]=========================*/

/*==================[declaraciones de funciones internas]====================*/

/*==================[declaraciones de funciones externas]====================*/

// FUNCION que se ejecuta cada vezque ocurre un Tick
bool_t diskTickHook( void *ptr );

/*==================[funcion principal]======================================*/

// FUNCION PRINCIPAL, PUNTO DE ENTRADA AL PROGRAMA LUEGO DE ENCENDIDO O RESET.
int main( void ){
uint32_t bytesWritten;
uint16_t adcValue = 0;

	// Configura la placa de manera generica
	boardConfig();
	adcConfig( ADC_ENABLE );
	spiConfig( SPI0 );
	tickConfig( 10, diskTickHook );
   
    rtc.year  = 1970;
    rtc.month = 1;
    rtc.mday  = 1;
    rtc.wday  = 1;
    rtc.hour  = 0;
    rtc.min   = 0;
    rtc.sec   = 0;
    rtcConfig( &rtc );

	// habilitar la SD
	if( f_mount_( &FileSystem, "", 0 ) != FR_OK ){
		// ERROR
	}

	while( TRUE ){

		// delay de 1 seg

		// lectura de los datos de los sensores
		adcValue = adcRead( CH1 );

		// lectura del RTC
		rtcRead( &rtc );

		// procesar los datos
		formatearLaInformacion( adcValue ); // = fecha;valor;
		/*
		 * fecha1;valor1;
		 * fecha2;valor2;
		 */

		// secuencia de escritura
		if( f_open_( &File, FILENAME, FA_WRITE | FA_OPEN_APPEND ) == FR_OK ){


			f_write_( &File, "Hola mundo\r\n", 50, &bytesWritten );

			f_close_(&File);
		} else{
			// ERROR
		}
	}


	return 0;
}

/*==================[definiciones de funciones internas]=====================*/
void        formatearLaInformacion ( int valorAdc ){
	// valor_fecha ; valor_variable ;
    infoToSave[0]  = (rtc.mday/10) + '0';
    infoToSave[1]  = (rtc.mday%10) + '0';
    infoToSave[2]  = '/';
    infoToSave[3]  = (rtc.month/10) + '0';
    infoToSave[4]  = (rtc.month%10) + '0';
    infoToSave[5]  = '/';
    infoToSave[6]  = (rtc.year/1000) + '0';
    infoToSave[7]  = ((rtc.year%1000)/100) + '0';
    infoToSave[8]  = ((rtc.year%100)/10) + '0';
    infoToSave[9]  = (rtc.year%10) + '0';
    infoToSave[10] = '_';
    infoToSave[11] = (rtc.hour/10) + '0';
    infoToSave[12] = (rtc.hour%10) + '0';
    infoToSave[13] = ':';
    infoToSave[14] = (rtc.min/10) + '0';
    infoToSave[15] = (rtc.min%10) + '0';
    infoToSave[16] = ':';
    infoToSave[17] = (rtc.sec/10) + '0';
    infoToSave[18] = (rtc.sec%10) + '0';
    infoToSave[19] = ';';
    infoToSave[20] = valorAdc/1000 		+ '0';
    infoToSave[21] = (valorAdc%1000)/100 	+ '0';
    infoToSave[22] = (valorAdc%100)/10 	+ '0';
    infoToSave[23] = (valorAdc%10) 		+ '0';
    infoToSave[24] = ';';
    infoToSave[25] = '\n';
    infoToSave[26] = '\r';
    infoToSave[27] = '\0';
}

/*==================[definiciones de funciones externas]=====================*/

// FUNCION que se ejecuta cada vezque ocurre un Tick
bool_t diskTickHook( void *ptr ){
	// Disk timer process
	disk_timerproc();
	return 1;
}

/*==================[fin del archivo]========================================*/
