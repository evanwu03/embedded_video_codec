
#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <msp432p401r.h>


/* =========================================== */
// UART_Config_t configuration parameter types
/* =========================================== */

typedef enum {
    UART_PARITY_NONE = 0,
    UART_PARITY_ODD = EUSCI_A_CTLW0_PEN,
    UART_PARITY_EVEN = EUSCI_A_CTLW0_PAR | EUSCI_A_CTLW0_PEN
} UART_Parity_t;

typedef enum {
    UART_LSB_FIRST = 0,
    UART_MSB_FIRST = EUSCI_A_CTLW0_MSB
} UART_DataOrder_t;

typedef enum {
    UART_DATA_8BIT = 0,
    UART_DATA_7BIT = EUSCI_A_CTLW0_SEVENBIT
} UART_Character_Len_t;

typedef enum {
    UART_MODE = EUSCI_A_CTLW0_MODE_0,
    UART_IDLE_LINE_MULTI = EUSCI_A_CTLW0_MODE_1,
    UART_ADDRESS_BIT_MULTI = EUSCI_A_CTLW0_MODE_2,
    UART_AUTO_BAUD_DETECT = EUSCI_A_CTLW0_MODE_3
} UART_Mode_t;

typedef enum {
    UART_UCLK = EUSCI_A_CTLW0_SSEL__UCLK,
    UART_ACLK = EUSCI_A_CTLW0_SSEL__ACLK,
    UART_SMCLK = EUSCI_A_CTLW0_SSEL__SMCLK,
} UART_Clock_Source_t;

// UCAxMCTLW Register configs

typedef enum {
    UART_OVERSAMPLING_OFF = 0,
    UART_OVERSAMPLING_ON = EUSCI_A_MCTLW_OS16
} UART_Oversampling_t;

// End of UART parameter definitions



/* =========================================== */
// UART_Config_t Definition
/* =========================================== */
typedef struct {
    UART_Parity_t parity;             // Parity Enable
    UART_DataOrder_t order;           // MSB mode select
    UART_Character_Len_t data_length; // Character length
    UART_Mode_t mode;                 // eUSCI_A mode select
    UART_Clock_Source_t clock_sel;    // eUSCI_A clock source select
    uint32_t baud_rate;               // Desired baud rate
    UART_Oversampling_t oversampling; // Enables oversmapling mode;      UCAxMCTLW:  UCOS16 bit
    uint16_t baud_prescaler;          // baud rate prescaler select;     UCAxBRW:    UCBRx field
    uint8_t firstMod;                 // first modualtion stage select;  UCAxMCTLW:  UCBRFx field
    uint8_t secondMod;                // second modulation stage select; UCAxMCTLW:  UCBRSx field

} UART_config_t;
// End of UART_Config_t definition



// Driver functions 
void uart_initModule(EUSCI_A_Type *uart, const UART_config_t *config);
void uart_enableModule(EUSCI_A_Type *uart);
void uart_disableModule(EUSCI_A_Type *uart);
void uart_enableInterrupts(EUSCI_A_Type *uart, uint8_t mask);
void uart_disableInterrupts(EUSCI_A_Type *uart, uint8_t mask);


#endif // UART_H 