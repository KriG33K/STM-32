/*
 * lcd.h
 *
 *  Created on: Jun 27, 2025
 *      Author: christianmorelli
 */

#ifndef __LCD_H__
#define __LCD_H__
#include <stdint.h>

void LCD_Init(void);
void LCD_Send_Command(uint8_t cmd);
void LCD_Send_Data(uint8_t data);
void LCD_Send_String(char *str);
void LCD_Set_Cursor(uint8_t row, uint8_t col);
void gestisci_tasto(char tasto);
#endif
