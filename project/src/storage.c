#include "stm32f4xx.h"
#include "storage.h"
#include "clock.h"
#include "eeprom.h"

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


void storage_init() {
    config_pb6_scl_gpio();
    config_pb7_sda_gpio();

    menu_idx = 0;
    cur_ingred_idx = 0;

    load_ingredients_from_eeprom();
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

    // Update in EEPROM as well
    update_entry_eeprom(idx);
}

// Hardware storage
void write_default_to_eeprom() {
    // Write magic
    write_byte_eeprom(DATABASE_ADDR, DATABASE_MAGIC);

    // Write all defaults
    for (int z = 0; z < num_ingredients; z++) {
        update_entry_eeprom(z);
    }
}

void update_entry_eeprom(int idx) {
    uint32_t density = ingredients[idx].density;

    uint8_t buf[4];
    buf[0] = (density >> 24) & 0xff;
    buf[1] = (density >> 16) & 0xff;
    buf[2] = (density >>  8) & 0xff;
    buf[3] = (density >>  0) & 0xff;

    write_page_eeprom(DATABASE_ADDR + (idx * ENTRY_SIZE) + 1, buf, ENTRY_SIZE);

    // uint32_t density = ingredients[idx].density;
    // for (int i = ENTRY_SIZE - 1; i >= 0; i--) {
    //     write_i2c_byte(DATABASE_ADDR + (idx * ENTRY_SIZE) + i + 1, density & 0xFF);
    //     density >> 8;
    // }
}

uint32_t get_entry_eeprom(int idx) {
    uint32_t density = 0;

    uint8_t buf[4];
    read_page_eeprom(DATABASE_ADDR + (idx * ENTRY_SIZE) + 1, buf, ENTRY_SIZE);

    for (int i = 0; i < 4; i++) {
        density |= buf[i] << ((3-i) * 8);
    }

    return density;
}

void load_ingredients_from_eeprom() {
    uint32_t density;
    uint8_t byte;

    // Check for magic
    byte = read_byte_eeprom(DATABASE_ADDR);
    if (byte != DATABASE_MAGIC) {
        write_default_to_eeprom();
        return;
    }

    // Load into table
    int ingredients_idx = 0;
    uint8_t buf[8];
    uint8_t num_bytes;
    for (int z = 0; z < num_ingredients; z += NUM_ENTRIES_PER_BUF) {
        if (num_ingredients - z < NUM_ENTRIES_PER_BUF) {
            num_bytes = (num_ingredients - z) * ENTRY_SIZE;
        } else {
            num_bytes = NUM_ENTRIES_PER_BUF * ENTRY_SIZE;
        }
        read_page_eeprom(DATABASE_ADDR + (z * ENTRY_SIZE) + 1, buf, num_bytes);

        for (int i = 0; i < num_bytes; i += ENTRY_SIZE) {
            // For each entry
            density = 0; 
            for (int j = 0; j < ENTRY_SIZE; j++) {
                density |= buf[i + j];
                if (j + 1 < ENTRY_SIZE) density = density << 8;
            }

            ingredients[ingredients_idx].density = density;
            ingredients_idx += 1;
        }
    }



    // // Load into table
    // for (int z = 0; z < num_ingredients; z++) {
    //     density = 0;

    //     // Read density for zth entry
    //     for (int i = 0; i < ENTRY_SIZE; i++) {
    //         byte = read_i2c_byte(DATABASE_ADDR + (z * ENTRY_SIZE) + i + 1); //Original addr + z entries + ith byte + 1 for magic
    //         density |= byte;
    //         if (i + 1 < ENTRY_SIZE) density = density << 8;
    //     }

    //     ingredients[z].density = density;
    // }
}
