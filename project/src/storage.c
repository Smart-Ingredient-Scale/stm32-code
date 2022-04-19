#include "stm32f4xx.h"
#include "storage.h"

int menu_idx; // For menu traversal
int cur_ingred_idx; 
const int num_ingredients = 8; 


struct IngredientInfo ingredients[] = {
    {   .name = "Flour",
        .density = 122  },
    {   .name = "Sugar",
        .density = 123  },
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
    // TODO: eeprom

    menu_idx = 0;
    cur_ingred_idx = 0;
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
