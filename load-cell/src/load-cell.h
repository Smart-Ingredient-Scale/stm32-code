#ifndef LOAD_CELL_H
#define LOAD_CELL_H

// TODO 1: test the load cell ADC values for linearity and
// determine maximum load it can sample

// TODO 2: make the load_cell_calibrate() function

/* constants for load cell configuration */
#define SD_CLK_FREQEUNCY        50000   /* 50 kHz HX711 SDCLK */
#define ABS_MIN_SD_CLK_FREQ     16667   /* 16,667 Hz clock or slower for power down mode */   
#define SD_OUT_PIN              8       /* PB8 for HX711 serial data out */
#define SD_CLK_PIN              9       /* PB9 for HX711 serial data clock (PWM output) */
#define TIM4_PSC_CLK            500000  /* 500 kHz prescaler clock */
#define NUM_BITS_PER_SAMPLE     24      /* 24-bit ADC resolution */

/* number samples stored in memory (make an even power of 2) */
#define NUM_STORED_SAMPLES         16      /* number of 24-bit samples stored in memory */
#define NUM_STORED_SAMPLES_BITSHIFT 4      /* another way to represent NUM_STORED_SAMPLE */

/* Contains the data structure for averaging ADC samples. The ADC samples
 * are stored in === 2's COMPLEMENT=== for a 24-bit number!!!! */
typedef struct _HX771_Data {
    int32_t samples[NUM_STORED_SAMPLES];
    uint8_t nextUpdatedInd;
    int32_t cumulativeSum;
    int32_t movingAverage;
} HX711_Data_t;

/*** data structures for load cell function / function calls ***/
enum HX711SamplingRate_t {HZ10=10, HZ80=80}; /* sampling rate (10Hz or 80Hz) */

/* (1) Channel A 128 gain = 25 clks : all posedge clocks sent to HX711
 * (2) Channel B 32 gain = 26 clks
 * (3) Channel A 64 gain = 27 clks */
enum HX711ChanConfig_t {CHA_128_GAIN=25, CHB_32_GAIN=26, CHA_64_GAIN=27};

enum HX711DataStatus_t {UNAVAILABLE, READY}; /* determines if data ready from HX711 */
enum Tim4IrqState_t {IDLE0, IDLE1, RECEIVING_DATA, CONFIGURING_CHANNEL};

/* function declarations */
void load_cell_init(enum HX711SamplingRate_t fs, enum HX711ChanConfig_t chanCfg);
void load_cell_enable(void);
void load_cell_disable(void);

#endif /* LOAD_CELL_H */