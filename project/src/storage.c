#include "stm32f4xx.h"
#include "storage.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_gpio.h"
#include "clock.h"

int menu_idx; // For menu traversal
int cur_ingred_idx; 
const int num_ingredients = 8; 


struct IngredientInfo ingredients[] = {
    {   .name = "Flour",
        .density = 100  },
    {   .name = "Sugar",
        .density = 10000  },
    {   .name = "Salt",
        .density = 124  },
    {   .name = "Baking soda",
        .density = 125  },
    {   .name = "Butter",
        .density = 126  },
    {   .name = "Milk",
        .density = 127  },
    {   .name = "VegetableOil",
        .density = 128  },
    {   .name = "Vanilla Extr",
        .density = 129  },
    // {   .name = "Cinnamon",
    //     .density = 130  },
};


void eeprom_init_pins();
void eeprom_init_i2c();

void storage_init() {
    // TODO: eeprom

    menu_idx = 0;
    cur_ingred_idx = 0;

    eeprom_init_pins();
    eeprom_init_i2c();
    eeprom_sw_reset();
}


void storage_get_names(char** name1, char **name2, char **name3) {
    if (menu_idx >= num_ingredients || menu_idx < 0) {
        menu_idx = 0; //reset
    }

    if (menu_idx < num_ingredients) {
        *name1 = ingredients[menu_idx].name;
    }

    if (menu_idx + 1 < num_ingredients) {
        *name2 = ingredients[menu_idx + 1].name;
    }

    if (menu_idx + 2 < num_ingredients) {
        *name3 = ingredients[menu_idx + 2].name;
    }

}


void storage_inc_index() {
    menu_idx += 3;

    if (menu_idx >= num_ingredients) {
        menu_idx = 0; //reset
    }
}

void storage_dec_index() {
    menu_idx -= 3;

    if (menu_idx >= num_ingredients || menu_idx < 0) {
        menu_idx = num_ingredients - (num_ingredients % 3); //circle back
        if  (menu_idx == num_ingredients) {
            menu_idx = num_ingredients - 3;
        }
    }
}

struct IngredientInfo storage_get_ingred_from_menu(int offset) {
    int idx = menu_idx + offset;
    return storage_get_ingred(idx);
}

struct IngredientInfo storage_get_ingred(int idx) {
    if (idx >= num_ingredients || idx < 0) {
        struct IngredientInfo empty = {.name = '\0'};
        return empty;
    }

    cur_ingred_idx = idx;
    return ingredients[idx];
}

int storage_get_ingred_idx() {
    return cur_ingred_idx;
}

void storage_update_ingred(int idx, int32_t density) {
    if (idx >= num_ingredients || idx < 0) {
        return;
    }

    ingredients[idx].density = density;
}





// EEPROM
void eeprom_init_pins() {
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void eeprom_init_i2c() {
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    I2C_InitTypeDef I2C_InitStructure;

	I2C_InitStructure.I2C_ClockSpeed = 10000; //400,000
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x30; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Init(I2C1, &I2C_InitStructure);

    I2C_Cmd(I2C1, ENABLE);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
}


void eeprom_sw_reset() {
    for (int i = 0; i < 9; i++) {
        I2C_GenerateSTART(I2C1, ENABLE);
        micro_wait(5000);
    }
}


void eeprom_byte_write(uint16_t addr, uint8_t data) {
    // Send start event and slave address
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); 
    I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Send addr
    uint8_t upper_addr = addr >> 8;
    uint8_t lower_addr = addr & 0xFF;
	I2C_SendData(I2C1, upper_addr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1, lower_addr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Send data
	I2C_SendData(I2C1, data);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));  

    // Send stop
    I2C_GenerateSTOP(I2C1, ENABLE);
}

uint8_t eeprom_byte_read(uint16_t addr) {
    // Send start event and slave address
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); 
    I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    // Send addr
    uint8_t upper_addr = addr >> 8;
    uint8_t lower_addr = addr & 0xFF;
	I2C_SendData(I2C1, upper_addr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	I2C_SendData(I2C1, lower_addr);
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));




    // Actual read
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); 
    I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    uint8_t data = I2C_ReceiveData(I2C1);

    // Send stop
    I2C_GenerateSTOP(I2C1, ENABLE);

    return data;
}