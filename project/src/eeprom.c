#include "eeprom.h"
#include "clock.h"

/* make sure start on a 32-byte page boundary on reads AND writes!! */


enum sda_status_t sdaStat = OUTPUT;

/* Function declarations to bit bang the generic i2c interface
 * contains relevant flags for checking what the ACK status should be */
static inline void set_scl_low(void) {
    GPIOB->BSRR = GPIO_BSRR_BR_6;
}

static inline void set_scl_high(void) {
    GPIOB->BSRR = GPIO_BSRR_BS_6;
}

static inline void set_sda_low(void) {
    GPIOB->BSRR = GPIO_BSRR_BR_7;
}

static inline void set_sda_high(void) {
    GPIOB->BSRR = GPIO_BSRR_BS_7;
}

void config_pb7_sda_gpio(void) {
    /* configure pb7 for general purpose output */
    GPIOB->MODER &= ~GPIO_MODER_MODER7;
    GPIOB->MODER |= GPIO_MODER_MODER7_0;
    sdaStat = OUTPUT;

    /* enable a pull-up resistor on the SDA line */
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR7;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR7_0;

    set_sda_low();
}

/* change the sda GPIO pin for output (push-pull) or input mode */
void reconfig_sda_gpio(enum sda_status_t outStat) {
    
    if(outStat == OUTPUT) {
        /* configure pb7 for general purpose output */
        GPIOB->MODER &= ~GPIO_MODER_MODER7;
        GPIOB->MODER |= GPIO_MODER_MODER7_0;
        sdaStat = OUTPUT;
    }

    else {
        /* configure pb7 for general purpose input */
        GPIOB->MODER &= ~GPIO_MODER_MODER7;
        sdaStat = INPUT;
    }

    micro_wait(GPIO_RECONFIG_US);

}

/* configure the SCL gpio pin on PB6 */
void config_pb6_scl_gpio(void) {
    /* configure pb6 for general purpose output */
    GPIOB->MODER &= ~GPIO_MODER_MODER6;
    GPIOB->MODER |= GPIO_MODER_MODER6_0;

    /* enable a pull-up resistor on the SDA line */
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR7;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR7_0;

    set_scl_low(); /* idle SCL value */
}

/* send the start condition on the I2C data line */
void send_start_condition(void) {
    set_scl_low();

    /* reconfigure the SDA line for output after reading the ACK */
    if(sdaStat == INPUT) {
        reconfig_sda_gpio(OUTPUT);
    }

    else {
        micro_wait(MIN_WAIT_US);
    }

    set_sda_high();
    micro_wait(MIN_WAIT_US);
    set_scl_high();

    /* make sure the lines default to high */
    micro_wait(MIN_WAIT_US);

    /* send the start condition */
    set_sda_low();
    micro_wait(MIN_WAIT_US);

    set_scl_low(); /* idle state */
}

/* STOP condition, assume SDA is low */
void send_stop_condition(void) {
    if(sdaStat == INPUT) {
        reconfig_sda_gpio(OUTPUT);
        set_sda_low();
    }

	set_scl_high();

    /* send the stop condition */
    micro_wait(MIN_WAIT_US);
    set_sda_high();
    micro_wait(MIN_WAIT_US);
}

/* send i2c data bit assuming the SCL line defaults low */
void send_i2c_bit(uint8_t bit) {
    if(sdaStat == INPUT) {
        reconfig_sda_gpio(OUTPUT);
    }

    else {
        micro_wait(MIN_WAIT_US);
    }
    
    if(bit == 0) {
        set_sda_low();
    }

    else {
        set_sda_high();
    }

    micro_wait(MIN_WAIT_US); /* let data line settle (setup time) */

    /* implement posedge scl data latch + hold time */
    set_scl_high();
    micro_wait(MIN_WAIT_US);

    set_scl_low();
}

/* Send a generic byte of data MSb-first */
enum ack_status_t send_i2c_byte(uint8_t byte) {
    for(int8_t i = 7; i >= 0; i--) {
        send_i2c_bit((byte>>i) & 0x1);
    }

    /* verify the ACK was received from the EEPROM */
    if(read_i2c_bit() != LOW) {
        return NACK;
    }
    
    else {
        return ACK;
    }
}

/* reconfigure the SDA pin for input and read the SDA line */
uint8_t read_i2c_bit(void) {
    if(sdaStat != INPUT) {
        reconfig_sda_gpio(INPUT);
    }

    set_scl_high();
    micro_wait(BAUD_WIDTH_US);

    uint8_t bit = ((GPIOB->IDR >> SDA_PIN) & 0x1);
    
    set_scl_low(); /* leave in default state */

    micro_wait(BAUD_WIDTH_US);

    return bit;
}

/* read 8 bits in a row driven by the I2C slave device */
uint8_t read_i2c_byte(void) {
    uint8_t byte = 0x0;

    for(uint8_t i = 0; i < 8; i++) {
        byte |= read_i2c_bit();
        if(i != 7) {
        	byte <<= 1;
        }
    }

    /* drive the ACK line low (to ACK the transaction) */
    send_i2c_bit(0);

    return byte;
}

/* Contains the higher-level EEPROM-specific driver code */
void send_slave_address(enum rw_bit_t rwBit) {
    /* send the slave address 7'b101000 and the R/~W bit */
    uint8_t slaveAddress = (rwBit == READ)? 0xA1 : 0xA0;

    // while(send_i2c_byte(slaveAddress) != ACK);
    if(send_i2c_byte(slaveAddress) != ACK) {
        while(1);
    }

}

/* Send the EEPROM the 13-bit memory adress for its internal pointer to
* its non-volatile memory */
void send_eeprom_rw_address(uint16_t address) {
    uint8_t topAddress = address >> 8;
    uint8_t botAddress = address & 0xFF;

    /* send the upper bits first */
    // while(send_i2c_byte(topAddress) != ACK);
    if(send_i2c_byte(topAddress) != ACK) {
        while(1);
    }


    /* send the lower bits second */
    // while(send_i2c_byte(botAddress) != ACK);
    if(send_i2c_byte(botAddress) != ACK) {
        while(1);
    }

}

/* Takes care of the handshaking required to send only one byte */
void write_byte_eeprom(uint16_t eepromAddress, uint8_t byte) {
    send_start_condition();

    send_slave_address(WRITE);

    send_eeprom_rw_address(eepromAddress);

    /* send only one byte, can replace with for loop later */
    send_i2c_byte(byte);

    /* stop */
    send_stop_condition();

    micro_wait(PAGE_WRITE_DELAY_US);
}

/* Takes care of the setup and sending of a stream of up to 32 bytes
 * during a page write to the EEPROM */
uint8_t write_page_eeprom(uint16_t eepromAddress, const uint8_t *src, uint8_t numBytes) {
    if(numBytes > MAX_PAGE_SIZE_BYTES) {
        numBytes = MAX_PAGE_SIZE_BYTES;
    }
    
    send_start_condition();

    send_slave_address(WRITE);

    send_eeprom_rw_address(eepromAddress);

    /* send up to 32 bytes consecutively starting at eepromAddress */
    for(uint8_t i = 0; i < numBytes; i++) {
        send_i2c_byte(*src++);
    }

    /* stop */
    send_stop_condition();

    micro_wait(PAGE_WRITE_DELAY_US); /* wait for write cycle completioni */

    return numBytes;
}

/* Takes care of the handshaking required to read only one byte */
uint8_t read_byte_eeprom(uint16_t eepromAddress) {
    send_start_condition();

    send_slave_address(WRITE); /* set the pointer to where we want to
                                * begin reading from */

    send_eeprom_rw_address(eepromAddress);

    send_start_condition(); /* begin the read transaction */

    send_slave_address(READ);

    uint8_t byteRead = read_i2c_byte();

    /* end the I2C transaction */
    send_stop_condition();

    return byteRead;
}

/* Reads in numBytes from EEPROM into a uint8_t destination buffer and
 * returns the number of bytes able to be read from the EEPROM */
uint8_t read_page_eeprom(uint16_t eepromAddress, uint8_t *dest, uint8_t numBytes) {
    if(numBytes > MAX_PAGE_SIZE_BYTES) {
        numBytes = 32;
    }

    send_start_condition();

    send_slave_address(WRITE); /* set the pointer to where we want to
                                * begin reading from */

    send_eeprom_rw_address(eepromAddress);

    send_start_condition(); /* begin the read transaction(s) */

    send_slave_address(READ);

    /* read the bytes in consecutive fashion (make sure to end w/ ACK) */
    for(uint8_t i = 0; i < numBytes; i++) {
        *dest++ = read_i2c_byte();
    }

    send_stop_condition();

    return numBytes; /* return the number of bytes read from EEPROM */
}
