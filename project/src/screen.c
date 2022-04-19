#include "stm32f4xx.h"

#include "screen.h"
#include "stm32_adafruit_lcd.h"
#include "ili9341.h"
#include "stdio.h"
#include "storage.h"

// Usage:
// Call BSP_LCD_Init() to init the display
// expose the following variables like so:

//// Use the following to switch between screens ////
// extern struct Screen home_screen;
// extern struct Screen information_screen;
// extern struct Screen vol_cal_screen;
// extern struct Screen mass_cal_screen;

//// Use the following to see what volume the user has input, and the current unit that the user has selected to display on the 8-seg ////
// extern struct VolumeSelection volume_selection;
// extern units_t cur_display_unit;

// Call process_button with the relevant button id to update the screen


char vol_cal_chars[] = VOL_CAL_CHARS;
char vol_cal_units[] = VOL_CAL_UNITS;

struct VolumeSelection volume_selection = (struct VolumeSelection){0};
units_t cur_display_unit = UNITS_GRAMS;
uint32_t cur_density = 1;

extern uint32_t curr_g_read; // from load-cell.c

// Screen and button variables

struct Button home_screen_buttons[MAX_BUTTONS_PER_SCREEN] = {
    {   .id = ITEM1_HOME_BUTTON_ID,
        .x0 = 160,
        .x1 = 240,
        .y0 = 0,
        .y1 = 240,
        .process_id = ITEM1_HOME_PROCESS_ID },

    {   .id = ITEM2_HOME_BUTTON_ID,
        .x0 = 80,
        .x1 = 160,
        .y0 = 0,
        .y1 = 240,
        .process_id = ITEM2_HOME_PROCESS_ID },
    
    {   .id = ITEM3_HOME_BUTTON_ID,
        .x0 = 0,
        .x1 = 80,
        .y0 = 0,
        .y1 = 240,
        .process_id = ITEM3_HOME_PROCESS_ID },

    {   .id = UP_HOME_BUTTON_ID,
        .x0 = 120,
        .x1 = 240,
        .y0 = 240,
        .y1 = 320,
        .process_id = UP_HOME_PROCESS_ID },

    {   .id = DOWN_HOME_BUTTON_ID,
        .x0 = 0,
        .x1 = 120,
        .y0 = 240,
        .y1 = 320,
        .process_id = DOWN_HOME_PROCESS_ID },
};


struct Button information_screen_buttons[MAX_BUTTONS_PER_SCREEN] = {
    {   .id = HOME_ITEM_BUTTON_ID,
        .x0 = 160,
        .x1 = 240,
        .y0 = 0,
        .y1 = 240,
        .process_id = HOME_ITEM_PROCESS_ID },

    {   .id = UNIT1_ITEM_BUTTON_ID,
        .x0 = 105,
        .x1 = 130,
        .y0 = 130,
        .y1 = 150,
        .process_id = UNIT1_ITEM_PROCESS_ID },

    {   .id = UNIT2_ITEM_BUTTON_ID,
        .x0 = 105,
        .x1 = 130,
        .y0 = 175,
        .y1 = 210,
        .process_id = UNIT2_ITEM_PROCESS_ID },

    {   .id = UNIT3_ITEM_BUTTON_ID,
        .x0 = 105,
        .x1 = 130,
        .y0 = 225,
        .y1 = 260,
        .process_id = UNIT3_ITEM_PROCESS_ID },

    {   .id = UNIT4_ITEM_BUTTON_ID,
        .x0 = 105,
        .x1 = 130,
        .y0 = 275,
        .y1 = 310,
        .process_id = UNIT4_ITEM_PROCESS_ID },

    {   .id = CALIBRATE_ITEM_BUTTON_ID,
        .x0 = 0,
        .x1 = 80,
        .y0 = 180,
        .y1 = 320,
        .process_id = CALIBRATE_ITEM_PROCESS_ID },

};


struct Button vol_cal_screen_buttons[MAX_BUTTONS_PER_SCREEN] = {
    {   .id = HOME_VOL_CAL_BUTTON_ID,
        .x0 = 160,
        .x1 = 240,
        .y0 = 0,
        .y1 = 80,
        .process_id = HOME_VOL_CAL_PROCESS_ID },  

    {   .id = UP_VOL_CAL_BUTTON_ID,
        .x0 = 160,
        .x1 = 240,
        .y0 = 240,
        .y1 = 320,
        .process_id = UP_VOL_CAL_PROCESS_ID },  

    {   .id = DOWN_VOL_CAL_BUTTON_ID,
        .x0 = 80,
        .x1 = 160,
        .y0 = 240,
        .y1 = 320,
        .process_id = DOWN_VOL_CAL_PROCESS_ID },  

    {   .id = LEFT_VOL_CAL_BUTTON_ID,
        .x0 = 0,
        .x1 = 80,
        .y0 = 0,
        .y1 = 120,
        .process_id = LEFT_VOL_CAL_PROCESS_ID },  

    {   .id = RIGHT_VOL_CAL_BUTTON_ID,
        .x0 = 0,
        .x1 = 80,
        .y0 = 120,
        .y1 = 240,
        .process_id = RIGHT_VOL_CAL_PROCESS_ID },  

    {   .id = SUBMIT_VOL_CAL_BUTTON_ID,
        .x0 = 0,
        .x1 = 80,
        .y0 = 240,
        .y1 = 320,
        .process_id = SUBMIT_VOL_CAL_PROCESS_ID },  
};


struct Button mass_cal_screen_buttons[MAX_BUTTONS_PER_SCREEN] = {
    {   .id = HOME_MASS_CAL_BUTTON_ID,
        .x0 = 160,
        .x1 = 240,
        .y0 = 0,
        .y1 = 80,
        .process_id = HOME_MASS_CAL_PROCESS_ID },  
    
    {   .id = SUBMIT_MASS_CAL_BUTTON_ID,
        .x0 = 0,
        .x1 = 80,
        .y0 = 60,
        .y1 = 260,
        .process_id = SUBMIT_MASS_CAL_PROCESS_ID },  
};


struct Screen home_screen = {
    .id = HOME_SCREEN_ID,
    .buttons = home_screen_buttons,
    .num_buttons = 5
};

struct Screen information_screen = {
    .id = INFORMATION_SCREEN_ID,
    .buttons = information_screen_buttons,
    .num_buttons = 6
};

struct Screen vol_cal_screen = {
    .id = VOLUME_CALIBRATION_SCREEN_ID,
    .buttons = vol_cal_screen_buttons,
    .num_buttons = 6
};

struct Screen mass_cal_screen = {
    .id = MASS_CALIBRATION_SCREEN_ID,
    .buttons = mass_cal_screen_buttons,
    .num_buttons = 2
};

// Include icons
extern uint16_t* home_icon;
extern uint16_t* check_icon;


// Home Screen
void draw_home_screen(char *item1, char *item2, char *item3) {
    BSP_LCD_Clear(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetBackColor(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetTextColor(LCD_THEME_PRIMARY_COLOR);
    BSP_LCD_SetFont(&Font24);

    // General selection slots
    BSP_LCD_FillRect(160-LINE_OFFSET, 0, LINE_WIDTH, 240); //top hori
    BSP_LCD_FillRect(80-LINE_OFFSET, 0, LINE_WIDTH, 240); //bottom hori

    // Right vertical section
    BSP_LCD_FillRect(0, 240-LINE_OFFSET, 240, LINE_WIDTH); //vertical
    BSP_LCD_FillRect(120-LINE_OFFSET, 240, LINE_WIDTH, 80); //small hori
    BSP_LCD_FillTriangle(150, 250, 210, 280, 150, 310); // up triangle
    BSP_LCD_FillTriangle(90, 250, 30, 280, 90, 310); // down triangle
    
    // Slot text
    BSP_LCD_DisplayStringAtSize(0, 110, (uint8_t *)item1, CENTER_MODE, 12);
    BSP_LCD_DisplayStringAtSize(0, 190, (uint8_t *)item2, CENTER_MODE, 12);
    BSP_LCD_DisplayStringAtSize(0, 270, (uint8_t *)item3, CENTER_MODE, 12);
}

void update_home_screen(char *item1, char *item2, char *item3) {
    BSP_LCD_SetBackColor(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetTextColor(LCD_THEME_PRIMARY_COLOR);
    BSP_LCD_SetFont(&Font24);  

    BSP_LCD_DisplayStringAtSize(0, 110, (uint8_t *)item1, CENTER_MODE, 12);
    BSP_LCD_DisplayStringAtSize(0, 190, (uint8_t *)item2, CENTER_MODE, 12);
    BSP_LCD_DisplayStringAtSize(0, 270, (uint8_t *)item3, CENTER_MODE, 12);
}

// Item Information Screen
void draw_information_screen(char *ingedient_name, char *density) {
    BSP_LCD_Clear(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetBackColor(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetTextColor(LCD_THEME_PRIMARY_COLOR);
    BSP_LCD_SetFont(&Font24);

    // General horizontal lines
    BSP_LCD_FillRect(160-LINE_OFFSET, 0, LINE_WIDTH, 320); //top hori
    BSP_LCD_FillRect(80-LINE_OFFSET, 0, LINE_WIDTH, 320); //bottom hori

    BSP_LCD_FillRect(0, 180-LINE_OFFSET, 80, LINE_WIDTH); //small vert

    // Top row
    BSP_LCD_FillRect(160, 80-LINE_OFFSET, 80, LINE_WIDTH); //small vert
    BSP_LCD_DrawRGB16Image(170, 8, 64, 64, &home_icon);

    BSP_LCD_DisplayStringAtSize(80, 110, (uint8_t *)ingedient_name, CENTER_MODE, 13);

    // Middle row
    BSP_LCD_DisplayStringAt(-60, 190, (uint8_t *)"Unit:", CENTER_MODE);
    
    DISPLAY_STRING_WITH_UNITS(UNITS_GRAMS, 20, 190, (uint8_t *)"g", CENTER_MODE);
    DISPLAY_STRING_WITH_UNITS(UNITS_POUNDS, 70, 190, (uint8_t *)"lb", CENTER_MODE);
    DISPLAY_STRING_WITH_UNITS(UNITS_OUNCES, 120, 190, (uint8_t *)"oz", CENTER_MODE);
    DISPLAY_STRING_WITH_UNITS(UNITS_MILLILITERS, 170, 190, (uint8_t *)"mL", CENTER_MODE);

    // Bottom Row
    // BSP_LCD_FillRect(0, 180-LINE_OFFSET, 80, LINE_WIDTH); //small vert
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(-30, 260, (uint8_t *)"Density:", CENTER_MODE);
    BSP_LCD_DisplayStringAtSize(-30, 290, (uint8_t *)density, CENTER_MODE, 11);
    BSP_LCD_DisplayStringAt(130, 275, (uint8_t *)"Calibrate", CENTER_MODE);
}

void update_information_screen() {
    BSP_LCD_SetBackColor(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetTextColor(LCD_THEME_PRIMARY_COLOR);
    BSP_LCD_SetFont(&Font24);

    // BSP_LCD_DisplayStringAtSize(80, 110, (uint8_t *)ingedient_name, CENTER_MODE, 13);

    DISPLAY_STRING_WITH_UNITS(UNITS_GRAMS, 20, 190, (uint8_t *)"g", CENTER_MODE);
    DISPLAY_STRING_WITH_UNITS(UNITS_POUNDS, 70, 190, (uint8_t *)"lb", CENTER_MODE);
    DISPLAY_STRING_WITH_UNITS(UNITS_OUNCES, 120, 190, (uint8_t *)"oz", CENTER_MODE);
    DISPLAY_STRING_WITH_UNITS(UNITS_MILLILITERS, 170, 190, (uint8_t *)"mL", CENTER_MODE);

    // BSP_LCD_SetFont(&Font20);
    // BSP_LCD_DisplayStringAtSize(-30, 290, (uint8_t *)density, CENTER_MODE, 11);
}

// Volume Calibration Screen
void draw_vol_cal_screen(struct VolumeSelection *vol_sel) {
    BSP_LCD_Clear(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetBackColor(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetTextColor(LCD_THEME_PRIMARY_COLOR);
    BSP_LCD_SetFont(&Font24);

    // Home icon
    BSP_LCD_FillRect(160, 80-LINE_OFFSET, 80, LINE_WIDTH); //small vert
    BSP_LCD_FillRect(160, 0, LINE_WIDTH, 80); //small hori
    BSP_LCD_DrawRGB16Image(170, 8, 64, 64, &home_icon);

    // Arrows and check
    BSP_LCD_FillRect(0, 240-LINE_OFFSET, 240, LINE_WIDTH); //vertical
    BSP_LCD_FillRect(80, 0, LINE_WIDTH, 320); //horizontal
    BSP_LCD_FillRect(160-LINE_OFFSET, 240, LINE_WIDTH, 80); //small hori
    BSP_LCD_FillRect(0, 120, 80, LINE_WIDTH); //small vert
    BSP_LCD_FillTriangle(175, 255, 225, 280, 175, 305); // up triangle
    BSP_LCD_FillTriangle(145, 255, 95, 280, 145, 305); // down triangle
    BSP_LCD_FillTriangle(15, 85, 40, 35, 65, 85); // left triangle
    BSP_LCD_FillTriangle(15, 155, 40, 205, 65, 155); // right triangle

    BSP_LCD_DrawRGB16Image(8, 250, 64, 64, &check_icon);

    // Boxes around numbers/units
    BSP_LCD_DrawRect(105, 63, 30, 20);
    BSP_LCD_DrawRect(105, 88, 30, 20);
    BSP_LCD_DrawRect(105, 113, 30, 20);
    BSP_LCD_DrawRect(105, 138, 30, 20);
    BSP_LCD_DrawRect(109, 165, 22, 65); // units
    
    // Text
    DISPLAY_STRING_WITH_SELECTION(0, -45, 190, (uint8_t *)&vol_cal_chars[vol_sel -> char_idx1], CENTER_MODE);
    DISPLAY_STRING_WITH_SELECTION(1, -20, 190, (uint8_t *)&vol_cal_chars[vol_sel -> char_idx2], CENTER_MODE);
    DISPLAY_STRING_WITH_SELECTION(2, 5, 190, (uint8_t *)&vol_cal_chars[vol_sel -> char_idx3], CENTER_MODE);
    DISPLAY_STRING_WITH_SELECTION(3, 30, 190, (uint8_t *)&vol_cal_chars[vol_sel -> char_idx4], CENTER_MODE);
 
    BSP_LCD_SetFont(&Font20);
    DISPLAY_STRING_WITH_SELECTION(4, 80, 190, (uint8_t *)&vol_cal_units[(vol_sel -> units) * VOL_CAL_UNITS_LENGTH], CENTER_MODE);
 
    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(15, 100, (uint8_t *)"Enter", CENTER_MODE);
    BSP_LCD_DisplayStringAt(40, 130, (uint8_t *)"Volume", CENTER_MODE);
}

void update_vol_cal_screen(struct VolumeSelection *vol_sel) {
    BSP_LCD_SetBackColor(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetTextColor(LCD_THEME_PRIMARY_COLOR);
    BSP_LCD_SetFont(&Font24);

    DISPLAY_STRING_WITH_SELECTION(0, -45, 190, (uint8_t *)&vol_cal_chars[vol_sel -> char_idx1], CENTER_MODE);
    DISPLAY_STRING_WITH_SELECTION(1, -20, 190, (uint8_t *)&vol_cal_chars[vol_sel -> char_idx2], CENTER_MODE);
    DISPLAY_STRING_WITH_SELECTION(2, 5, 190, (uint8_t *)&vol_cal_chars[vol_sel -> char_idx3], CENTER_MODE);
    DISPLAY_STRING_WITH_SELECTION(3, 30, 190, (uint8_t *)&vol_cal_chars[vol_sel -> char_idx4], CENTER_MODE);

    BSP_LCD_SetFont(&Font20);
    DISPLAY_STRING_WITH_SELECTION(4, 80, 190, (uint8_t *)&vol_cal_units[(vol_sel -> units) * VOL_CAL_UNITS_LENGTH], CENTER_MODE);
}


// Mass Calibration Screen
void draw_mass_cal_screen(char* volume_and_unit) {
    BSP_LCD_Clear(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetBackColor(LCD_THEME_SECONDARY_COLOR);
    BSP_LCD_SetTextColor(LCD_THEME_PRIMARY_COLOR);
    BSP_LCD_SetFont(&Font24);

    // Home icon
    BSP_LCD_FillRect(160, 80-LINE_OFFSET, 80, LINE_WIDTH); //small vert
    BSP_LCD_FillRect(160, 0, LINE_WIDTH, 80); //small hori
    BSP_LCD_DrawRGB16Image(170, 8, 64, 64, &home_icon);

    // Bottom button
    BSP_LCD_FillRect(80-LINE_OFFSET, 60-LINE_OFFSET, LINE_WIDTH, 200 + LINE_WIDTH); //hori
    BSP_LCD_FillRect(0, 60-LINE_OFFSET, 80, LINE_WIDTH); //small vert
    BSP_LCD_FillRect(0, 260-LINE_OFFSET, 80, LINE_WIDTH); //small vert
    BSP_LCD_DisplayStringAt(42, 270, (uint8_t *)"Record Mass", CENTER_MODE);

    BSP_LCD_SetFont(&Font20);
    BSP_LCD_DisplayStringAt(90, 100, (uint8_t *)"Place ", LEFT_MODE);
    BSP_LCD_DisplayStringAtSize(170, 100, (uint8_t *)volume_and_unit, LEFT_MODE, 10);
    BSP_LCD_DisplayStringAt(90, 120, (uint8_t *)"on the scale", LEFT_MODE);
}


void panic(char* panic_message) {
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_SetFont(&Font20);

    uint16_t message_length = 0;
    while (panic_message[message_length] != '\0') {
        message_length += 1;
    }

    BSP_LCD_DisplayStringAt(10, 80, (uint8_t *)"Panic - Message:", LEFT_MODE);

    int height = 120;
    for (uint8_t i = 0; i < message_length; i += 17) {
        BSP_LCD_DisplayStringAt(10, height, (uint8_t *)&panic_message[i], LEFT_MODE);
        height += 20;
    }

    for(;;); // never returns
}

int true_modulus(int a, int b) {
    return (a + b) % b;
}

void update_volume_selection(struct VolumeSelection *vol_sel, process_id_t process_id) {
    uint8_t *chars = (uint8_t*)vol_sel;
    if (process_id == UP_VOL_CAL_PROCESS_ID) {
        if (vol_sel -> cur_selection < VOL_CAL_INPUT_BOX_COUNT) {
            chars[vol_sel -> cur_selection] += 2;
            chars[vol_sel -> cur_selection] = chars[vol_sel -> cur_selection] % (VOL_CAL_PERIOD_IDX + 2);
        } else {
            chars[vol_sel -> cur_selection] += 1;
            chars[vol_sel -> cur_selection] = chars[vol_sel -> cur_selection] % VOL_CAL_UNITS_NUM_UNITS;
        }
    } else if (process_id == DOWN_VOL_CAL_PROCESS_ID) {
        if (vol_sel -> cur_selection < VOL_CAL_INPUT_BOX_COUNT) {
            chars[vol_sel -> cur_selection] = true_modulus(chars[vol_sel -> cur_selection] - 2, VOL_CAL_PERIOD_IDX + 2);
        } else {
            chars[vol_sel -> cur_selection] = true_modulus(chars[vol_sel -> cur_selection] - 1, VOL_CAL_UNITS_NUM_UNITS);
        }
    } else if (process_id == LEFT_VOL_CAL_PROCESS_ID) {
        vol_sel -> cur_selection = true_modulus(vol_sel -> cur_selection - 1, 5);
    } else if (process_id == RIGHT_VOL_CAL_PROCESS_ID) {
        vol_sel -> cur_selection = true_modulus(vol_sel -> cur_selection + 1, 5);
    }
}

// Returns as fixed point decimal with 2 decimal places,
// -1 if error
// Ex: 31.14 -> 3114
int32_t convert_vol_cal(struct VolumeSelection *vol_sel) {
    // Check to make sure just 1 period, find position
    uint8_t period_pos = VOL_CAL_INPUT_BOX_COUNT;
    uint8_t *chars = (uint8_t*)vol_sel;
    for(uint8_t i = 0; i < VOL_CAL_INPUT_BOX_COUNT; i++) {
        if (chars[i] == VOL_CAL_PERIOD_IDX) {
            if (period_pos != VOL_CAL_INPUT_BOX_COUNT) { return -1; }
            period_pos = i;
        }   
    }

    // Convert numbers into res
    int32_t res = 0;
    int32_t num;
    uint8_t power;
    for(uint8_t i = 0; i < VOL_CAL_INPUT_BOX_COUNT; i++) {
        if (chars[i] != VOL_CAL_PERIOD_IDX) {
            // For all numbers
            num = chars[i] / 2; //Divide by 2 due to array index with \0 bytes
            if (i < period_pos) {
                power = (period_pos - i - 1) + 2; // Add 2 for the 2 decimal places
            } else {
                power = (period_pos - i) + 2;
            }
            if (power < 0) { continue; } // Ignore smallest

            while (power > 0) {
                num *= 10;
                power -= 1;
            }
            res += num;
        }
    }

    return res;
}

void fxdpnt_to_str(int32_t density, char* s) {
    char str[13] = {'\0'};
    int pre = density / 100;
    int post = density % 100;

    sprintf(str, "%d.%.2d", pre, post);
    strncpy(s, str, 13);
}

void process_button(struct Screen **cur_screen, process_id_t process_id) {
    obj_id_t screen_id = (*cur_screen) -> id;
    if (screen_id >= MAX_SCREEN_ID) {
        char err[80];
        sprintf(err, "Attempt to run process %d with screen_id %d", process_id, screen_id);
        panic(err);
    }

    char *name1 = '\0'; // For storing name of items to display on menu
    char *name2 = '\0';
    char *name3 = '\0';
    char density_str[12];
    
    if (screen_id == HOME_SCREEN_ID) {
        if ((process_id == ITEM1_HOME_PROCESS_ID) |
            (process_id == ITEM2_HOME_PROCESS_ID) |
            (process_id == ITEM3_HOME_PROCESS_ID) ) {
            
            // Get current ingredient info
            struct IngredientInfo ingred = storage_get_ingred_from_menu(process_id - ITEM1_HOME_PROCESS_ID);
            if (ingred.name[0] == '\0') return; // Invalid storage_get_ingred
            fxdpnt_to_str(ingred.density, density_str); //convert density int32_t to string

            // Switch screen
            (*cur_screen) = &information_screen;
            draw_information_screen(ingred.name, density_str);
        } else if (process_id == UP_HOME_PROCESS_ID) {
            storage_dec_index();
            storage_get_names(&name1, &name2, &name3);
            update_home_screen(name1, name2, name3);
        } else if (process_id == DOWN_HOME_PROCESS_ID) {
            storage_inc_index();
            storage_get_names(&name1, &name2, &name3);
            update_home_screen(name1, name2, name3);
        }

    } else if (screen_id == INFORMATION_SCREEN_ID) {
        if (process_id == HOME_ITEM_PROCESS_ID) {
            // Switch screen
            (*cur_screen) = &home_screen;
            storage_get_names(&name1, &name2, &name3);
            draw_home_screen(name1, name2, name3);          
        } else if (process_id == UNIT1_ITEM_PROCESS_ID) {
            cur_display_unit = UNITS_GRAMS;
            update_information_screen();
        } else if (process_id == UNIT2_ITEM_PROCESS_ID) {
            cur_display_unit = UNITS_POUNDS;
            update_information_screen();
        } else if (process_id == UNIT3_ITEM_PROCESS_ID) {
            cur_display_unit = UNITS_OUNCES;
            update_information_screen();
        } else if (process_id == UNIT4_ITEM_PROCESS_ID) {
            cur_display_unit = UNITS_MILLILITERS;
            update_information_screen();
        } else if (process_id == CALIBRATE_ITEM_PROCESS_ID) {
            volume_selection = (struct VolumeSelection){0}; // Reset volume_selection

            // Switch screen
            (*cur_screen) = &vol_cal_screen;
            draw_vol_cal_screen(&volume_selection);
        }

    } else if (screen_id == VOLUME_CALIBRATION_SCREEN_ID) {
        if (process_id == HOME_VOL_CAL_PROCESS_ID) {
            // Switch Screen
            (*cur_screen) = &home_screen;
            storage_get_names(&name1, &name2, &name3);
            draw_home_screen(name1, name2, name3);  
        } else if ((process_id == UP_VOL_CAL_PROCESS_ID) |
                  (process_id == DOWN_VOL_CAL_PROCESS_ID) |
                  (process_id == LEFT_VOL_CAL_PROCESS_ID) |
                  (process_id == RIGHT_VOL_CAL_PROCESS_ID)) {
            update_volume_selection(&volume_selection, process_id);
            update_vol_cal_screen(&volume_selection);
        } else if (process_id == SUBMIT_VOL_CAL_PROCESS_ID) {
            if (convert_vol_cal(&volume_selection) != -1) { // check if valid volume selection
                // Switch Screen
                (*cur_screen) = &mass_cal_screen;
                draw_mass_cal_screen("curvol");
            } else {
                volume_selection = (struct VolumeSelection){0}; // Reset volume_selection
                update_vol_cal_screen(&volume_selection);
            }
        }

    } else if (screen_id == MASS_CALIBRATION_SCREEN_ID) {
        if (process_id == HOME_MASS_CAL_PROCESS_ID) {
            // Switch Screen
            (*cur_screen) = &home_screen;
            storage_get_names(&name1, &name2, &name3);
            draw_home_screen(name1, name2, name3);  
        } else if (process_id == SUBMIT_MASS_CAL_PROCESS_ID) {
            // Update ingredient info
            int idx = storage_get_ingred_idx();
            storage_update_ingred(idx, 500);

            // Get current ingredient info
            struct IngredientInfo ingred = storage_get_ingred(idx);
            fxdpnt_to_str(ingred.density, density_str); //convert density int32_t to string

            // Switch screen
            (*cur_screen) = &information_screen;
            draw_information_screen(ingred.name, density_str);
        }
    }
}

// This conversion function converts from cups/ml/TSP/TBSP to ml
int32_t vol_cal_convert(int32_t x)
{

    if(volume_selection.units == VOL_CAL_UNITS_CUPS)
    {
        x = x * 23659 / 100; // Cups to ml - mult by 236.588
    }
    else if(volume_selection.units == VOL_CAL_UNITS_TSP)
    {
        x = 4929 / 1000; // TSP to ml - mult by 4.92892
    }
    else if(volume_selection.units == VOL_CAL_UNITS_TBSP)
    {
        x = 14787 / 1000; // TSP to ml - mult by 14.7868
    }

    return x;
}

// calculate density from g reading (from load-cell.c) and the ml val (from vol_cal_convert)
int32_t calc_density(int32_t vol_ml)
{
    uint32_t density;

    density = curr_g_read / vol_ml;

    return density;
}


