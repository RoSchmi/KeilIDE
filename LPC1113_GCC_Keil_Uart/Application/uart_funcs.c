#include "LPC11xx.h"

int  uart_status_rxd(void);
int  uart_status_txd(void);
uint32_t uart_get_intr_IIR(void);
char UART_putc(char ch);
char UART_getc(void);
void UART_echo(void);

// Initialize UART to simple polling mode (no interrupt)
void UART_config(void)
{
  // Enable clocks IO config block
  // Bit 16: IO config
  LPC_SYSCON->SYSAHBCLKCTRL |=  ((1<<16));

  __NOP(); // Short time delay
  __NOP();
  __NOP();

  // PIO1_6 IO output config
  //  bit[5]   - Hysteresis (0=disable, 1 =enable)
  //  bit[4:3] - MODE(0=inactive, 1 =pulldown, 2=pullup, 3=repeater)
  //  bit[2:0] - Function (0 = IO, 1=RXD, 2=CT32B0_MAT0)
  LPC_IOCON->PIO1_6 = (0<<5) | (0<<3) | (0x1);

  // PIO1_7 IO output config
  //  bit[5]   - Hysteresis (0=disable, 1 =enable)
  //  bit[4:3] - MODE(0=inactive, 1 =pulldown, 2=pullup, 3=repeater)
  //  bit[2:0] - Function (0 = IO, 1=TXD, 2=CT32B0_MAT1)
  LPC_IOCON->PIO1_7 = (0<<5) | (0<<3) | (0x1);
	
  // Enable clocks IO UART
  // UART is bit 12
  LPC_SYSCON->SYSAHBCLKCTRL |=  ((1<<12));
  // UART clock divider, divide by 1
  LPC_SYSCON->UARTCLKDIV = 1;

  // Enable access to divisor latch
  //  bit[7]    - DLAB (Divisor Latch Access Bit)
  //  bit[1:0]  - Word length (0= 5bits, 1= 6bits, 2= 7bits, 3= 8bits)
  LPC_UART->LCR = (1<<7) | 3;
	
  // Baud rate 38400, system clock 48MHz
  // PCLK / Baud Rate / 16 = 78.125 = (256 x DLM + DLL) x (1 + DivAddVal/MulVal)
  // ULM = 0
  // DLL = 67
  // MulVal = 6
  // DivAddVal = 1
  // 67 * (1 + 1/6) = 78.1666
  LPC_UART->DLM = 0;
  LPC_UART->DLL = 67;
  LPC_UART->FDR = (6 << 4) | (1 << 0);

  // FIFO Control Register
  //  bit[7:6]  - RX Trigger Level (0 = 1 character, 1 = 4ch, 2=8, 3=14)
  //  bit[2]    - TX FIFO Reset
  //  bit[1]    - RX FIFO Reset
  //  bit[0]    - FIFO Enable
  LPC_UART->FCR = (0<<6) | 	(0<<2) | (0<<1) | 1;

  // Line Control Register
  //  bit[7]    - DLAB (Divisor Latch Access Bit)
  //  bit[6]    - Break Control Enable
  //  bit[5:4]  - Parity select ( 0=odd, 1=even, 2=force 1 sticky, 3=force 0 stick)
  //  bit[3]    - Parity Enable
  //  bit[2]    - stop bit (0 = 1 stop bit, 1 = 2 stop bits)
  //  bit[1:0]  - Word length (0= 5bits, 1= 6bits, 2= 7bits, 3= 8bits)
  LPC_UART->LCR = 3;
  
  // Dummy read of LSR to clear error flags
  uart_status_rxd();
	
  // Interrupt Disable (IER can only be programmed when DLAB = 0)
  //  bit[0]    - RBR (Receive Data Available Enable)
  //  bit[1]    - THRE (Transmit Enable)
  //  bit[2]    - RX Line (Receive Line Interrupt Enable)
  //  bit[8]    - ABEOIntEn (auto band interrupt)
  //  bit[9]    - ABTOIntEn (auto band timeout interrupt)
  LPC_UART->IER = 0;
	
  // Wait until TX buffer is empty
  while (((LPC_UART->LSR >> 6) & 0x1) == 0);
  // Drain RX buffer
  while (uart_status_rxd()!=0) UART_getc();
	
  // Optional: Turn off clock to I/O Config block to save power
  LPC_SYSCON->SYSAHBCLKCTRL &=  ~(1<<16);
  return;
}
int  uart_status_rxd(void)
{ // Bit 0 is RDR (Receive Data Ready)
  return (LPC_UART->LSR & 0x1);
}
int  uart_status_txd(void)
{
  // Bit 5 is THRE (Transmit Holding Register Empty)
  return ((LPC_UART->LSR >> 5) & 0x1);
}

uint32_t uart_get_intr_IIR(void)
{
  return (LPC_UART->IIR);
}	

// Output a character to UART0
char UART_putc(char ch)
{
  while (uart_status_txd()==0);
  LPC_UART->THR = (uint32_t)ch;
  return ch;
}
// Read a character from UART0. If no data received yet, wait
char UART_getc(void)
{ /* Wait if Receive Data Register Full flag is 0 */
  while (uart_status_rxd()==0);
  return LPC_UART->RBR;
}
// Echo UART input if there is any
void UART_echo(void)
{
  if (uart_status_rxd()) {
    UART_putc(LPC_UART->RBR);}
  return;
}

// Uart string output
void UART_puts(char * mytext)
{
  char CurrChar;
  CurrChar = *mytext;
  while (CurrChar != (char) 0x0){
    UART_putc(CurrChar);  // Normal data
    mytext++;
    CurrChar = *mytext;
    }
  return;
}
