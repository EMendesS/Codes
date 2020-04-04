#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "string.h" //adicionado
#include <stdlib.h> //adicionado

// other files that are included in the project but not shown here
// stm32f10x_rcc.h
// stm32f10x_gpio.h
// stm32f10x_usart.h
// stm32f10x_spi.h

#define CMD_SET_READ_BIT 0x80
#define REG_WHO_AM_I 0x0F
#define VAL_WHO_AM_I 0xD7

static __IO uint32_t TimingDelay; // __IO so no undesired optimization
char palavra[4];  //adicionado

// Delay for ms
void delay_ms(uint32_t nTime){
    TimingDelay = nTime;
    while(TimingDelay != 0);
}

void SysTick_Handler() {
    if(TimingDelay != 0x00){
        TimingDelay--;
    }

}

// setup SPI1 pins A4/SS, A5/SCK, A6/MISO, A7/MOSI on port A
void PeripheralInit_SPI1_Slave(){

    GPIO_InitTypeDef GPIO_InitDef;
    SPI_InitTypeDef SPI_InitDef;

    // initialize init structs
    GPIO_StructInit(&GPIO_InitDef);
    SPI_StructInit(&SPI_InitDef);

    // initialize clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);

    // initialize A4/SS alternate function open-drain (50 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    // initialize A5/SCK alternate function open-drain (50 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    // initialize A6/MISO alternate function push-pull (50 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    // initialize A7/MOSI alternate function open-drain (50 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    // initialize LED output open-drain (2 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitDef);

    //  initialize SPI slave
    // for slave, no need to define SPI_BaudRatePrescaler
    SPI_InitDef.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitDef.SPI_Mode = SPI_Mode_Slave;
    SPI_InitDef.SPI_DataSize = SPI_DataSize_16b; // 8-bit transactions
    SPI_InitDef.SPI_FirstBit = SPI_FirstBit_MSB; // MSB first
    SPI_InitDef.SPI_CPOL = SPI_CPOL_Low; // CPOL = 0, clock idle low
    SPI_InitDef.SPI_CPHA = SPI_CPHA_2Edge; // CPHA = 1
    SPI_InitDef.SPI_NSS = SPI_NSS_Hard; // use hardware SS
    SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // APB2 72/64 = 1.125 MHz
    // SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // APB2 72/256 = 0.28 MHz
    // SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // APB2 72/16 = 4.5 MHz
    SPI_InitDef.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitDef);

    SPI_Cmd(SPI1, ENABLE);

}

// transfer a byte over SPI1  A4/SS, A5/SCK, A6/MISO, A7/MOSI
uint16_t transfer_8b_SPI1_Slave(uint16_t outByte){

    // Approach 1, from Brown's book
    // SPI_I2S_SendData(SPI1, outByte); // send
    // while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    // return SPI_I2S_ReceiveData(SPI1); // read received

    // Approach 2,
    // from http://www.lxtronic.com/index.php/basic-spi-simple-read-write
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, outByte);                                                        // send
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
    return SPI_I2S_ReceiveData(SPI1);                                                   // read recieived
}

// setup SPI2 pins PB12/SS, PB13/SCK, PB14/MISO, PB15/MOSI
void PeripheralInit_SPI2_Slave(){

    GPIO_InitTypeDef GPIO_InitDef;
    SPI_InitTypeDef SPI_InitDef;

    // initialize init structs
    GPIO_StructInit(&GPIO_InitDef);
    SPI_StructInit(&SPI_InitDef);

    // initialize clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    // initialize PB12/SS alternate function open-drain (10 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitDef);

    // initialize PB13/SCK alternate function open-drain (10 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitDef);

    // initialize PB14/MISO alternate function push-pull (10 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitDef);

    // initialize PB15/MOSI alternate function open-drain (10 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitDef);

    // initialize SPI slave
    // for slave, no need to define SPI_BaudRatePrescaler
    SPI_InitDef.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitDef.SPI_Mode = SPI_Mode_Slave;
    SPI_InitDef.SPI_DataSize = SPI_DataSize_8b; // 8-bit transactions
    SPI_InitDef.SPI_FirstBit = SPI_FirstBit_MSB; // MSB first
    SPI_InitDef.SPI_CPOL = SPI_CPOL_Low; // CPOL = 0, clock idle low
    SPI_InitDef.SPI_CPHA = SPI_CPHA_2Edge; // CPHA = 1
    SPI_InitDef.SPI_NSS = SPI_NSS_Hard; // use hardware SS
    SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32; // APB1 36/32 = 1.125 MHz
    // SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; // APB1 36/128 = 0.28 MHz
    // SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // APB1 36/8 = 4.5 MHz
    SPI_InitDef.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitDef);

    SPI_Cmd(SPI2, ENABLE);

}

// transfer a byte over SPI2 PB12/SS, PB13/SCK, PB14/MISO, PB15/MOSI
uint8_t transfer_8b_SPI2_Slave(uint8_t outByte){

    // Approach 1, from Brown's book
    // SPI_I2S_SendData(SPI2, outByte); // send
    // while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    // return SPI_I2S_ReceiveData(SPI2); // read received

    // Approach 2,
    // from http://www.lxtronic.com/index.php/basic-spi-simple-read-write
    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI2, outByte);                                                        // send
    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE));
    return SPI_I2S_ReceiveData(SPI2);                                                   // read recieived
}

// setup USART1 pins A9/TX, A10/RX on port A
void PeripheralInit_USART1(){

    GPIO_InitTypeDef GPIO_InitDef;
    USART_InitTypeDef USART_InitDef;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_StructInit(&GPIO_InitDef);

    // initialize A9/TX
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    // initialize A10/RX
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    USART_StructInit(&USART_InitDef);
    // modify for non-default values
    USART_InitDef.USART_BaudRate = 115200;
    USART_InitDef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitDef.USART_WordLength = USART_WordLength_8b;
    USART_InitDef.USART_StopBits = USART_StopBits_1;
    USART_InitDef.USART_Parity = USART_Parity_No;
    USART_InitDef.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitDef);
    USART_Cmd(USART1, ENABLE);

}

// transmit a character over USART1 A9/TX
int put_char_usart1(int c) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART1->DR = (c & 0xFF);
    return 0;
}

// receive a character over USART1 A10/RX
int get_char_usart1(){

    // this while loop makes the call blocking
    // will wait until byte received
    // while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    // return USART1->DR & 0xFF;

    if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET){
        return USART1->DR & 0xFF;
    } else {
        return -1;
    }
}

// transmit a string over USART1 A9/TX
int serial_print_usart1(char chars[]){
    int i=0;
    // print until null char or until too many characters counted
    while((chars[i] != 0x00) && (i < 512)){
        put_char_usart1(chars[i]);
        i++;
    }
    if(i < 512){ return 0; } else { return 1; }
}

// code for slave
int main(){

//    volatile uint32_t count = 0;
    uint16_t numRead = 0x00;

    SysTick_Config(SystemCoreClock/1000); // configure system clock for 1000 ticks/s or 1 ms
    PeripheralInit_USART1(); // initialize USART1 at A9/TX, A10/RX
    PeripheralInit_SPI1_Slave(); // initialize SPI1 slave at A4/SS, A5/SCK, A6/MISO, A7/MOSI
    // PeripheralInit_SPI2_Slave(); // initialize SPI2 slave at B12/SS, B13/SCK, B14/MISO, B15/MOSI

    // announce start of code
    serial_print_usart1("Quick example of SPI communication\n");
    serial_print_usart1("SPI slave, emulating L3GD20H gyro\n");

    while(1){

        numRead = transfer_8b_SPI1_Slave(0xD7); // receive a request command
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        // Print received
        serial_print_usart1("Received....");
        itoa(numRead, palavra, 10);
        serial_print_usart1(palavra);
        serial_print_usart1("\n");
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
    }

}
