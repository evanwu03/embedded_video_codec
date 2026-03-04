
#include "../include/spi.h"


/// @brief Initializes eUSCI B module for 3-Pin SPI usage
/// @param spi 
/// @param config 
void spi_initModule(EUSCI_B_Type* spi, const SPI_Config_t* config) {

    /*  // --- Configure the SPI pins based on module ---
    if (spi == EUSCI_B0 || spi == EUSCI_B0_SPI) {

        // Set P1.5, P1.6, and P1.7 as SPI pins (CLK, MOSI, MISO)
        P1->SEL0 |= BIT5 | BIT6 | BIT7;   // CLK, MOSI, MISO
        P1->SEL1 &= ~(BIT5 | BIT6 | BIT7);
    } 
    else if (spi == EUSCI_B1 || spi == EUSCI_B1_SPI) {

        P6->SEL0 |= BIT2 | BIT3 | BIT4;   // CLK, MOSI, MISO
        P6->SEL1 &= ~(BIT2 | BIT3 | BIT4);
    } 
    else if (spi == EUSCI_B2 || spi ==EUSCI_B2_SPI) {

        P3->SEL0 |= BIT5 | BIT6 | BIT7;   // CLK, MOSI, MISO
        P3->SEL1 &= ~(BIT5 | BIT6 | BIT7);
    }
    else if (spi == EUSCI_B3 || spi == EUSCI_B3_SPI) {
        
        P9->SEL0 |= BIT1 | BIT2 | BIT3;   // CLK, MOSI, MISO
        P9->SEL1 &= ~(BIT1 | BIT2 | BIT3);
    } */

    // Add the configuration code here..
    spi->CTLW0 =  EUSCI_B_CTLW0_SWRST; // Clear previous configurations 
    spi->CTLW0|= EUSCI_B_CTLW0_SYNC;
    spi->CTLW0 |= ( config->clock_phase_sel
                  | config->clock_polarity_sel
                  | config->data_length
                  | config->data_order);


    if(config->mode == SPI_MASTER_MODE) { 

        spi->CTLW0 |= (EUSCI_B_CTLW0_MST);
        spi->CTLW0 |= config->clock_sel;
        spi->BRW = config->clock_divider;
    }

    
}   


/// @brief Enables SPI module by clearing SWRST bit 
/// @param  spi SPI module to be enabled
void spi_enableModule(EUSCI_B_Type* spi)  {
    spi->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    // Release eUSCI state machine from reset
}


/// @brief Disables SPI module by setting SWRST bit 
/// @param  spi SPI module to be enabled
void spi_disableModule(EUSCI_B_Type* spi)  {
    spi->CTLW0 |= EUSCI_B_CTLW0_SWRST;    // Release eUSCI state machine from reset
}


/// @brief Sends a bytes from master to slave 
/// @param spi SPI module 
/// @param data byte to be sent
void spi_sendByte(EUSCI_B_Type* spi, const uint8_t data) { 
    while(!(spi->IFG & EUSCI_B_IFG_TXIFG)); // Use busy wait loop 
    spi->TXBUF = data; 

    //while(!(spi->IFG & EUSCI_B_IFG_RXIFG));  // wait until data is received
} 


/// @brief Receives a byte from master
/// @param spi SPI module
/// @return returns the byte transmitted
uint8_t spi_receiveByte(EUSCI_B_Type* spi) { 
    while(!(spi->IFG & EUSCI_B_IFG_RXIFG));  // wait until data is received
    return spi->RXBUF;
}

/// @brief Enables SPI interrrupts
/// @param spi SPI module to enable interrupts on 
/// @param mask bitmask for interrupt selection
void spi_enableInterrupts(EUSCI_B_Type* spi, const uint8_t mask) { 

    uint8_t locMask;

    // Check for valid bits only
    locMask = (mask & (EUSCI_B_IE_RXIE | EUSCI_B_IE_TXIE));

    spi->IE |= locMask;
}


/// @brief Disables SPI interrupts
/// @param spi SPI module to enable interrupts on
/// @param mask bitmask for interrupt selection
void spi_disableInterrupts(EUSCI_B_Type* spi, const uint8_t mask) { 
     
    uint8_t locMask;

    // Check for valid bits only
    locMask = (mask & (EUSCI_B_IE_RXIE | EUSCI_B_IE_TXIE));

    spi->IE &= ~locMask;
}


