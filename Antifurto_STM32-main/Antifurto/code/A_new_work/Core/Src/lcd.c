/*
 * lcd.c
 *
 *  Created on: Jun 27, 2025
 *      Author: christianmorelli
 */

#include "lcd.h"
#include "stm32f3xx_hal.h"
#include "main.h"
#include "stdint.h"
#include "string.h"

// Definizione dei pin
#define RS_Pin GPIO_PIN_0
#define RS_Port GPIOA
#define EN_Pin GPIO_PIN_1
#define EN_Port GPIOA
#define D4_Pin GPIO_PIN_2
#define D5_Pin GPIO_PIN_3
#define D6_Pin GPIO_PIN_4
#define D7_Pin GPIO_PIN_5
#define D_Port GPIOA

void LCD_Enable_Pulse() {
    HAL_GPIO_WritePin(EN_Port, EN_Pin, GPIO_PIN_SET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(EN_Port, EN_Pin, GPIO_PIN_RESET);
    HAL_Delay(2);
}

void LCD_Send_4Bits(uint8_t data) {
    HAL_GPIO_WritePin(D_Port, D4_Pin, (data >> 0) & 0x01);
    HAL_GPIO_WritePin(D_Port, D5_Pin, (data >> 1) & 0x01);
    HAL_GPIO_WritePin(D_Port, D6_Pin, (data >> 2) & 0x01);
    HAL_GPIO_WritePin(D_Port, D7_Pin, (data >> 3) & 0x01);
}

void LCD_Send_Command(uint8_t cmd) {
    HAL_GPIO_WritePin(RS_Port, RS_Pin, GPIO_PIN_RESET);
    LCD_Send_4Bits(cmd >> 4);
    LCD_Enable_Pulse();
    LCD_Send_4Bits(cmd & 0x0F);
    LCD_Enable_Pulse();
    HAL_Delay(5);
}

void LCD_Send_Data(uint8_t data) {
    HAL_GPIO_WritePin(RS_Port, RS_Pin, GPIO_PIN_SET);
    LCD_Send_4Bits(data >> 4);
    LCD_Enable_Pulse();
    LCD_Send_4Bits(data & 0x0F);
    LCD_Enable_Pulse();
    HAL_Delay(5);
}

void LCD_Send_String(char *str) {
    while (*str) {
        LCD_Send_Data((uint8_t)(*str));
        str++;
    }
}

void LCD_Set_Cursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x00 + col : 0x40 + col;
    LCD_Send_Command(0x80 | address);
}

void LCD_Init(void) {
    HAL_Delay(50);
    LCD_Send_4Bits(0x03); LCD_Enable_Pulse(); HAL_Delay(5);
    LCD_Send_4Bits(0x03); LCD_Enable_Pulse(); HAL_Delay(5);
    LCD_Send_4Bits(0x03); LCD_Enable_Pulse(); HAL_Delay(5);
    LCD_Send_4Bits(0x02); LCD_Enable_Pulse(); HAL_Delay(1);

    LCD_Send_Command(0x28); // 4-bit, 2 righe, 5x8 font
    LCD_Send_Command(0x0C); // Display ON, cursore OFF
    LCD_Send_Command(0x01); // Clear display
    HAL_Delay(5);
    LCD_Send_Command(0x06); // Entry mode set: incrementa cursore
    LCD_Set_Cursor(1, 0);
    LCD_Send_String("DISARMATO");


}

	char codice[5] = "";
	uint8_t ind = 0;

void gestisci_tasto(char tasto){

    if (ind < 6) {
        codice[ind] = tasto; // tasto è '0'–'9'
        ind++;
        codice[ind] = '\0';


        // Aggiorna LCD
        LCD_Set_Cursor(0, 0);
        for (int i = 0; i < 4; i++) {
            if (i < ind){
            	if (codice[i]=='*') {
            	            // Reset per nuovo inserimento
            	            ind = 0;
            	            codice[0] = '\0';
            	            LCD_Set_Cursor(0, 0);
            	            LCD_Send_String("____");
            	            break;
            	        }
                LCD_Send_Data(codice[i]);
            }
            else
                LCD_Send_Data('_');
        }
    }
}

