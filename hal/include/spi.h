
#ifndef SPI_H
#define SPI_H


#include <msp432p401r.h>
#include <stdint.h>



/// @brief SPI clock selection (ACLK, SMCLK)
typedef enum { 
    SPI_ACLK = EUSCI_B_CTLW0_SSEL__ACLK, 
    SPI_SMCLK = EUSCI_B_CTLW0_SSEL__SMCLK
} SPI_Clock_t; 


/// @brief Configure SPI for slave or master operation
typedef enum {
    SPI_SLAVE_MODE = 0,
    SPI_MASTER_MODE,
} SPI_Mode_t; 


/// @brief Data order for SPI bytes: MSB or LSB first
typedef enum { 
    SPI_LSB_FIRST = 0,
    SPI_MSB_FIRST = EUSCI_B_CTLW0_MSB
} SPI_DataOrder_t; 


/// @brief SPI character length: 7 or 8 bits
typedef enum { 
    SPI_DATA_8BIT = 0,
    SPI_DATA_7BIT = EUSCI_B_CTLW0_SEVENBIT
} SPI_Character_length_t; 



/// @brief SPI Clock phase determines when data is sampled (received)
typedef enum {
    SPI_PHASE_DATA_CHANGED_ONFIRST_CAPTURE_ON_NEXT  = 0,
    SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT = EUSCI_B_CTLW0_CKPH
} SPI_Clock_Phase_t;


/// @brief SPI Clock polarity 
typedef enum { 
    SPI_INACTIVE_LOW = 0,
    SPI_INACTIVE_HIGH = EUSCI_B_CTLW0_CKPL
} SPI_Clock_Polarity_t; 



/// @brief SPI User configuration used in SPI_initModule()
typedef struct { 
    SPI_Clock_t clock_sel; 
    SPI_Mode_t mode; 
    SPI_DataOrder_t data_order; 
    SPI_Character_length_t data_length;
    SPI_Clock_Phase_t clock_phase_sel;
    SPI_Clock_Polarity_t clock_polarity_sel;
    uint16_t clock_divider; 
} SPI_Config_t;

// SPI driver functions
void spi_initModule(EUSCI_B_Type* spi, const SPI_Config_t* config); 
void spi_enableModule(EUSCI_B_Type* spi); 
void spi_disableModule(EUSCI_B_Type* spi);
void spi_sendByte(EUSCI_B_Type* spi, const uint8_t data);
uint8_t spi_receiveByte(EUSCI_B_Type* spi);
void spi_enableInterrupts(EUSCI_B_Type* spi, const uint8_t mask);
void spi_disableInterrupts(EUSCI_B_Type* spi, const uint8_t mask);


#endif //SPI_H