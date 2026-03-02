
#include "../include/uart.h"


/// @brief HAL function for configuring EUSCI_Ax module for UART mode
/// @param uart THe UART module to be configured (A0, A1, or A2)
/// @param config A structure of type uart_config_t containing configuration parameters
void uart_initModule(EUSCI_A_Type *uart, const UART_config_t *config) {

    // Resets any previous UART configurations
    uart->CTLW0 = EUSCI_A_CTLW0_SWRST;

    // Select UART clock
    uart->CTLW0 |= config->clock_sel;

    uart->CTLW0 |= (config->parity | config->order | config->data_length | config->mode);

    // Set the baud rate generator prescale value
    uart->BRW = config->baud_prescaler;

    // Select first and second modulation stage values
    uart->MCTLW = 0;
    uart->MCTLW |= (config->firstMod << EUSCI_A_MCTLW_BRF_OFS) | (config->secondMod << EUSCI_A_MCTLW_BRS_OFS);

    // Set oversampling mode if enabled
    if (config->oversampling)
    {
        uart->MCTLW |= config->oversampling;
    }
}

/// @brief Enables the UART module by clearing the UCSWRST bit
/// @param uart reference to UART module
void uart_enableModule(EUSCI_A_Type *uart) {
    // Maybe check if UART is valid
    uart->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;
}

/// @brief Disables the UART module by setting the UCSWRST bit
/// @param uart reference to UART module
void uart_disableModule(EUSCI_A_Type *uart) {
    uart->CTLW0 |= EUSCI_A_CTLW0_SWRST;
}

/// @brief Enables interrupt for selected UART module
/// @param uart reference to UART module
/// @param mask bit mask of interrupt types that should be enabled
void uart_enableInterrupts(EUSCI_A_Type *uart, uint8_t mask) {

    uint8_t locMask;

    // Check for valid bits only
    locMask = (mask & (EUSCI_A_IE_RXIE | EUSCI_A_IE_TXIE | EUSCI_A_IE_STTIE | EUSCI_A_IE_TXCPTIE));

    uart->IE |= locMask;
}

/// @brief Disables interrupt for selected UART module
/// @param uart
void uart_disableInterrupts(EUSCI_A_Type *uart, uint8_t mask) {

    uint8_t locMask;

    // Check for valid bits only
    locMask = (mask & (EUSCI_A_IE_RXIE | EUSCI_A_IE_TXIE | EUSCI_A_IE_STTIE | EUSCI_A_IE_TXCPTIE));

    uart->IE &= ~locMask;
}

