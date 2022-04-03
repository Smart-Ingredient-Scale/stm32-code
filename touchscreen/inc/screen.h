#include "stm32f4xx.h"

#define COLOR_THEME_ATHLETIC_GOLD			0xcdb1
#define COLOR_THEME_BLACK					0x0000
#define COLOR_THEME_GRAY					0x9cb1
#define COLOR_THEME_DARK_GRAY				0x39c7
#define COLOR_THEME_CAMPUS_GOLD				0xc461

#define LCD_THEME_PRIMARY_COLOR				COLOR_THEME_BLACK
#define LCD_THEME_SECONDARY_COLOR			COLOR_THEME_CAMPUS_GOLD
#define LCD_THEME_COLOR3					COLOR_THEME_GRAY
#define LCD_THEME_COLOR4					COLOR_THEME_DARK_GRAY
#define LCD_THEME_COLOR5					COLOR_THEME_CAMPUS_GOLD


#define LINE_WIDTH							6
#define LINE_OFFSET							(LINE_WIDTH/2)


#define MAX_BUTTONS_PER_SCREEN				7

struct Button
{
	uint8_t id;
	uint16_t x0;
	uint16_t x1;
	uint16_t y0;
	uint16_t y1;
	uint8_t process_id; //store an enum, giving instructions to a generic "process" function on what tasks to perform
};


struct Screen
{
	uint8_t id;
	struct Button *buttons;
	uint8_t num_buttons;
	//struct Active *active_zones;
	uint8_t is_displayed;
};

// IDs
typedef enum {
	// Home Screen
	ITEM1_HOME_PROCESS_ID,
	ITEM2_HOME_PROCESS_ID,
	ITEM3_HOME_PROCESS_ID,
	UP_HOME_PROCESS_ID,
	DOWN_HOME_PROCESS_ID,

	// Item Information Screen
	HOME_ITEM_PROCESS_ID,
	UNIT1_ITEM_PROCESS_ID,
	UNIT2_ITEM_PROCESS_ID,
	UNIT3_ITEM_PROCESS_ID,
	UNIT4_ITEM_PROCESS_ID,
	CALIBRATE_ITEM_PROCESS_ID,

	// Volume Calibration Screen
	HOME_VOL_CAL_PROCESS_ID,
	UP_VOL_CAL_PROCESS_ID,
	DOWN_VOL_CAL_PROCESS_ID,
	LEFT_VOL_CAL_PROCESS_ID,
	RIGHT_VOL_CAL_PROCESS_ID,
	SUBMIT_VOL_CAL_PROCESS_ID,

	// Mass Calibration Screen
	HOME_MASS_CAL_PROCESS_ID,
	SUBMIT_MASS_CAL_PROCESS_ID
} process_id_t;

typedef enum {
	// Screens
	HOME_SCREEN_ID,
	INFORMATION_SCREEN_ID,
	VOLUME_CALIBRATION_SCREEN_ID,
	MASS_CALIBRATION_SCREEN_ID,
	MAX_SCREEN_ID, // meta-variable

	// Buttons
	ITEM1_HOME_BUTTON_ID,
	ITEM2_HOME_BUTTON_ID,
	ITEM3_HOME_BUTTON_ID,
	UP_HOME_BUTTON_ID,
	DOWN_HOME_BUTTON_ID,

	HOME_ITEM_BUTTON_ID,
	UNIT1_ITEM_BUTTON_ID,
	UNIT2_ITEM_BUTTON_ID,
	UNIT3_ITEM_BUTTON_ID,
	UNIT4_ITEM_BUTTON_ID,
	CALIBRATE_ITEM_BUTTON_ID,

	HOME_VOL_CAL_BUTTON_ID,
	UP_VOL_CAL_BUTTON_ID,
	DOWN_VOL_CAL_BUTTON_ID,
	LEFT_VOL_CAL_BUTTON_ID,
	RIGHT_VOL_CAL_BUTTON_ID,
	SUBMIT_VOL_CAL_BUTTON_ID,

	HOME_MASS_CAL_BUTTON_ID,
	SUBMIT_MASS_CAL_BUTTON_ID
} obj_id_t;

// Units to be displayed on the 7-segment display
typedef enum {
	UNITS_GRAMS,
	UNITS_POUNDS,
	UNITS_OUNCES,
	UNITS_MILLILITERS
} units_t;


#define VOL_CAL_NUM_CHARS 4
#define VOL_CAL_PERIOD_IDX 20
#define VOL_CAL_CHARS "0\0001\0002\0003\0004\0005\0006\0007\0008\0009\000.\000"
#define VOL_CAL_UNITS_LENGTH 5
// ^ length of the units string + null byte
#define VOL_CAL_UNITS "cups\000mL  \000unit\000"


// Units just for volume selection
typedef enum {
	VOL_CAL_UNITS_CUPS,
	VOL_CAL_UNITS_ML,
	VOL_CAL_UNITS_UNIT,
	VOL_CAL_UNITS_NUM_UNITS
} vol_cal_units_t;

struct VolumeSelection
{
	uint8_t char_idx1;
	uint8_t char_idx2;
	uint8_t char_idx3;
	uint8_t char_idx4;
	vol_cal_units_t units;
	uint8_t cur_selection;
};


void draw_home_screen(char *item1, char *item2, char *item3);
void update_home_screen(char *item1, char *item2, char *item3);

void draw_information_screen(char *ingedient_name, units_t unit, char *density);
void update_information_screen(char *ingedient_name, units_t unit, char *density);

void draw_vol_cal_screen(struct VolumeSelection *vol_sel);
void update_vol_cal_screen(struct VolumeSelection *vol_sel);
void update_volume_selection(struct VolumeSelection *vol_sel, process_id_t process_id);
int32_t convert_vol_cal(struct VolumeSelection *vol_sel);

void draw_mass_cal_screen(char* volume);

void panic(char* panic_message);

void process_button(struct Screen **cur_screen, process_id_t process_id);