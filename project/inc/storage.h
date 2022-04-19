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

