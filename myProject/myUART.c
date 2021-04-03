#include "MKL25Z4.h"
#include "myUART.h"

volatile uint8_t userSignal = 0;

void setupUART2(uint32_t baud_rate) {
	
	uint32_t divisor, bus_clock;
	
	// Enable clocking to the two different peripheral block
  SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	//(Alt 4 GPIO config to receive) data
	PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
	
	UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
	
	bus_clock = DEFAULT_SYSTEM_CLOCK/2;
	divisor = bus_clock / (baud_rate*16);
	UART2->BDH = UART_BDH_SBR(divisor >> 8);
	UART2->BDL = UART_BDL_SBR(divisor);
	
	UART2->C1 = 0;
	UART2->S2 = 0;
	UART2->C3 = 0;
	
	//Turn on receive data
	UART2->C2 |= (UART_C2_RE_MASK);
	
	NVIC_SetPriority(UART2_IRQn, 128);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
	
	UART2->C2 |= UART_C2_RIE_MASK;
}


void UART2_IRQHandler() {
	
	//NVIC_ClearPendingIRQ(UART2_IRQn);
	
	if (UART2->S1 & UART_S1_RDRF_MASK) {
	// received a character
		userSignal = UART2->D;
	} 
	/*
	if (UART2->S1 & (UART_S1_OR_MASK | UART_S1_NF_MASK | UART_S1_FE_MASK |
					UART_S1_PF_MASK)) {
										// handle the error
										// clear the flag
		NVIC_ClearPendingIRQ(PORTD_IRQn);
	} 
	*/
}