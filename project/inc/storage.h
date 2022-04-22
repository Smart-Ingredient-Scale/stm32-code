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
#define EEPROM_ADDR                 0xa0
#define DATABASE_ADDR               0x013f
#define DATABASE_MAGIC              0x2f
#define ENTRY_SIZE                  4
#define NUM_ENTRIES_PER_BUF         (32 / ENTRY_SIZE)


// Database = big endian
// Page size is 32-bytes, so must start (DATABASE_ADDR + 1) on a 32 byte boundry for page_write to work
// | Magic |            Entry 1            |            Entry 2            | ..
// |-------|-------------------------------|-------------------------------|
// | 0x2b  | high8                   low8  | high8                   low8  |


void write_default_to_eeprom();
void update_entry_eeprom(int idx);
uint32_t get_entry_eeprom(int idx);
void load_ingredients_from_eeprom();



// void write_eeprom_byte();
// void write_eeprom_page(const uint8_t *src, uint16_t destAddress, uint8_t numWrites);
// void read_i2c_byte(void);


// uint8_t write_eeprom_page(uint16_t eepromAddr, const uint8_t *src, uint8_t numBytes);
// uint8_t read_eeprom_page(uint16_t eepromAddr, uint8_t *dest, uint8_t numBytes);



// EEPROM
// SCL - PB6
// SDA - PB7


// Slave Address: BR24S64-W
// 1 0 1 0 A2 A1 A0 :: A2 = 0, A1 = 0, A0 = 0
// 1010 000

// Byte Write Cycle:
