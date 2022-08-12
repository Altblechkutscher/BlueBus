/*
 * File:   eeprom.h
 * Author: Ted Salmon <tass2001@gmail.com>
 * Description:
 *     EEPROM mechanisms
 */
#ifndef EEPROM_H
#define EEPROM_H
#include <xc.h>
#include "utils.h"
#include "../mappings.h"

/* 16000000 / (2 * (0 + 1)) = 8,000,000 or 8Mhz */
#define EEPROM_BRG 0
// 25LC1024 EEPROM instructions
#define EEPROM_COMMAND_WREN 0x06 // Write enable
#define EEPROM_COMMAND_WRDI 0x04 // Write disable
#define EEPROM_COMMAND_WRITE 0x02 // Initialize start of write sequence
#define EEPROM_COMMAND_READ 0x03 // Initialize start of read sequence
#define EEPROM_COMMAND_RDSR 0x05 // Read the status register
#define EEPROM_COMMAND_GET 0x00 // Dummy byte used to retrieve data
#define EEPROM_STATUS_BUSY 0x01 // EEPROM Busy status response

void EEPROMInit();
void EEPROMDestroy();
void EEPROMIsReady();
unsigned char EEPROMReadByte(uint32_t);
void EEPROMWriteByte(uint32_t, unsigned char);
#endif /* EEPROM_H */
