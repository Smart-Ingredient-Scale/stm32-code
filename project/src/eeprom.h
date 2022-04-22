#ifndef EEPROM_H 
#define EEPROM_H

#include <stdint.h>
#include "stm32f4xx.h"

/* make sure start on a 32-byte page boundary on reads AND writes!! */


enum ack_polarity_t {LOW=0, HIGH=1};
enum sda_status_t   {OUTPUT, INPUT};
enum rw_bit_t {READ=0, WRITE=1};
enum ack_status_t {ACK=0, NACK=1};

#define MAX_PAGE_SIZE_BYTES 32  /* max number of bytes in EEPROM page */
#define JANK_MAX_PAGE_SIZE  8
#define BAUD_WIDTH_US       3   /* maximum data rate of 333kHx */
#define MIN_WAIT_US         1
#define GPIO_RECONFIG_US    20
#define SCL_PIN             6
#define SDA_PIN             7
#define PAGE_WRITE_DELAY_US 5000 /* 5ms according to datasheet */

/* Low level-macro to drive the SCK and SDA lines */
void send_data_bit(uint8_t bit);
uint8_t read_i2c_bit(void);

/* Function declarations to bit bang the generic i2c interface
 * contains relevant flags for checking what the ACK status should be */
void send_start_condition(void);
void send_stop_condition(void);
void send(enum ack_polarity_t ackPol);
uint8_t read_i2c_byte(void);

/* Contains the higher-level EEPROM-specific driver code */
void send_slave_address(enum rw_bit_t);
void write_eeprom_byte();
void write_eeprom_page(const uint8_t *src, uint16_t destAddress, uint8_t numWrites);
void write_byte_eeprom(uint16_t eepromAddress, uint8_t byte);
uint8_t read_byte_eeprom(uint16_t eepromAddress);
uint8_t read_page_eeprom(uint16_t eepromAddress, uint8_t *dest, uint8_t numBytes);
uint8_t write_page_eeprom(uint16_t eepromAddress, const uint8_t *src, uint8_t numBytes);

/* GPIO configuration for EEPROM */
void config_pb7_sda_gpio(void);
void config_pb6_scl_gpio(void);
void reconfig_sda_gpio(enum sda_status_t outStat);

#endif /* EEPROM_H */
