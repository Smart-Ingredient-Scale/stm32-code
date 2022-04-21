#include "stm32f4xx.h"


struct IngredientInfo
{
	const char name[13];
	int32_t density; // fixed point decimal with 2 decimal places
                     // density is in units
};



void storage_init();

// For menu traversal
void storage_get_names(char** name1, char **name2, char **name3);
void storage_inc_index();
void storage_dec_index();

struct IngredientInfo storage_get_ingred_from_menu(int offset);
struct IngredientInfo storage_get_ingred(int idx);
int storage_get_ingred_idx();
void storage_update_ingred(int idx, int32_t density);


// EEPROM
#define EEPROM_ADDR             0xa0
// 0x50

void eeprom_sw_reset();
void eeprom_byte_write(uint16_t addr, uint8_t data);
uint8_t eeprom_byte_read(uint16_t addr);
// bool eeprom_byte_safewrite(uint16_t addr, uint8_t data);

// void eeprom_page_write(uint16_t addr, uint8_t *data, uint8_t size);
// bool eeprom_page_safewrite(uint16_t addr, uint8_t *data, uint8_t size);
// void eeprom_page_read(uint16_t addr, uint8_t *data, uint8_t size);





// EEPROM
// SCL - PB6
// SDA - PB7


// Slave Address: BR24S64-W
// 1 0 1 0 A2 A1 A0 :: A2 = 0, A1 = 0, A0 = 0
// 1010 000

// Byte Write Cycle:
