#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "string.h" //adicionado
#include <stdlib.h> //adicionado

// other files that are incuded in the project but not shown here
// stm32f10x_rcc.h
// stm32f10x_gpio.h
// stm32f10x_usart.h
// stm32f10x_spi.h

#define CMD_SET_READ_BIT 0x80
#define REG_WHO_AM_I 0x0F
#define VAL_WHO_AM_I 0xD7

char palavra[4];  //adicionado

static __IO uint32_t TimingDelay; // __IO so no undesired optimization

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

// Setup slave select, output, A3 on port A
void PeripheralInit_GPIO() {

    GPIO_InitTypeDef GPIO_InitDef;  // GPIO init
    GPIO_StructInit(&GPIO_InitDef); // initialize init struct

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // initialize clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // initialize clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // initialize clock

    GPIO_InitDef.GPIO_Pin = GPIO_Pin_12;         // GPIO pin12
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_Out_PP;  // GPIO mode output push-pull
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz;  // GPIO port output speed, 2 MHz
    GPIO_Init(GPIOB, &GPIO_InitDef);    // initialize pin on GPIOA port

    GPIO_InitDef.GPIO_Pin = GPIO_Pin_13;         // GPIO pin 13
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_Out_OD;  // GPIO mode output push-pull
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_2MHz;  // GPIO port output speed, 2 MHz
    GPIO_Init(GPIOC, &GPIO_InitDef);    // initialize pin on GPIOA port

}

// Setup SPI1 pins A4/SS, A5/SCK, A6/MISO, A7/MOSI on port A
// Hardware slave selectis not used. That is indicated in the settings
void PeripheralInit_SPI1_Master(){

    GPIO_InitTypeDef GPIO_InitDef;
    SPI_InitTypeDef SPI_InitDef;

    // initialize init structs
    GPIO_StructInit(&GPIO_InitDef);
    SPI_StructInit(&SPI_InitDef);

    // initialize clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

    // do not initialize A4/SS because a software SS will be used

    // initialize A5/SCK alternate function push-pull (50 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    // initialize A6/MISO input pull-up (50MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    // initialize A7/MOSI alternate function push-pull (50 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    // initialize SPI master
    // for slave, no need to define SPI_BaudRatePrescaler
    SPI_InitDef.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitDef.SPI_Mode = SPI_Mode_Master;
    SPI_InitDef.SPI_DataSize = SPI_DataSize_16b;     // 8-bit transactions
    SPI_InitDef.SPI_FirstBit = SPI_FirstBit_MSB;    // MSB first
    SPI_InitDef.SPI_CPOL = SPI_CPOL_Low;            // CPOL = 0, clock idle low
    SPI_InitDef.SPI_CPHA = SPI_CPHA_2Edge;          // CPHA = 1
    SPI_InitDef.SPI_NSS = SPI_NSS_Soft;             // use software SS
    SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64; // APB2 72/64 = 1.125 MHz
    // SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256; // APB2 72/256 = 0.28 MHz
    // SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16; // APB2 72/16 = 4.5 MHz
    SPI_InitDef.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitDef);

    SPI_Cmd(SPI1, ENABLE);

}

// Transfer a byte over SPI1  A4/SS, A5/SCK, A6/MISO, A7/MOSI
uint8_t transfer_8b_SPI1_Master(uint8_t outByte){

    // Approach 1, from Brown's book
    // SPI_I2S_SendData(SPI1, outByte); // send
    // while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    // return SPI_I2S_ReceiveData(SPI1); // read received

    // Approach 2,
    // from http://www.lxtronic.com/index.php/basic-spi-simple-read-write
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI1, outByte); // send
    while(!SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
    return SPI_I2S_ReceiveData(SPI1); // read received
}

// Setup SPI2 pins B12/SS, B13/SCK, B14/MISO, B15/MOSI on port B
void PeripheralInit_SPI2_Master(){

    GPIO_InitTypeDef GPIO_InitDef;
    SPI_InitTypeDef SPI_InitDef;

    // initialize init structs
    GPIO_StructInit(&GPIO_InitDef);
    SPI_StructInit(&SPI_InitDef);

    // initialize clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); // this is on a different bus

    // do not initialize B12/SS because a software SS will be used

    // initialize B13/SCK alternate function push-pull (10 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitDef);

    // initialize B14/MISO input pull-up (10 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitDef);

    // initialize B15/MOSI alternate function push-pull (10 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOB, &GPIO_InitDef);

    //  initialize SPI master
    // for slave, no need to define SPI_BaudRatePrescaler
    SPI_InitDef.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitDef.SPI_Mode = SPI_Mode_Master;
    SPI_InitDef.SPI_DataSize = SPI_DataSize_16b;         // 16-bit transactions
    SPI_InitDef.SPI_FirstBit = SPI_FirstBit_MSB;        // MSB first
    SPI_InitDef.SPI_CPOL = SPI_CPOL_Low;                // CPOL = 0, clock idle low
    SPI_InitDef.SPI_CPHA = SPI_CPHA_2Edge;              // CPHA = 1
    SPI_InitDef.SPI_NSS = SPI_NSS_Soft;                 // use software SS
    SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // APB1 36/32 = 1.125 MHz
    // SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; // APB1 36/128 = 0.28 MHz
    // SPI_InitDef.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; // APB1 36/8 = 4.5 MHz
    SPI_InitDef.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI_InitDef);

    SPI_Cmd(SPI2, ENABLE);
}

// Transfer a byte over SPI2 B12/SS, B13/SCK, B14/MISO, B15/MOSI
uint16_t transfer_8b_SPI2_Master(uint16_t outByte){

    // Approach 1, from Brown's book
    // SPI_I2S_SendData(SPI2, outByte); // send
    // while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    // return SPI_I2S_ReceiveData(SPI2); // read received

    // Approach 2,
    // from http://www.lxtronic.com/index.php/basic-spi-simple-read-write
    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE));
    SPI_I2S_SendData(SPI2, outByte); // send
    while(!SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE));
    return SPI_I2S_ReceiveData(SPI2); // read recieived
}

// Setup USART1 pins A9/TX, A10/RX on port A
void PeripheralInit_USART1(){

    GPIO_InitTypeDef GPIO_InitDef;
    USART_InitTypeDef USART_InitDef;

    // initialize clocks
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

    // initialize init struct
    GPIO_StructInit(&GPIO_InitDef);
    USART_StructInit(&USART_InitDef);

    // initialize USART1 TX alternate function push-pull (50 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    // initialize USART1 RX input floating (50 MHz)
    GPIO_InitDef.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitDef.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitDef);

    USART_InitDef.USART_BaudRate = 115200;
    USART_InitDef.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitDef.USART_WordLength = USART_WordLength_8b;
    USART_InitDef.USART_StopBits = USART_StopBits_1;
    USART_InitDef.USART_Parity = USART_Parity_No;
    USART_InitDef.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitDef);
    USART_Cmd(USART1, ENABLE);

}

// Transmit a character over USART1 A9/TX
int put_char_usart1(int c) {
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART1->DR = (c & 0xFF);
    return 0;
}

// Receive a character over USART1 A10/RX
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

// Transmit a string over USART1 A9/TX
int serial_print_usart1(char chars[]){
    int i=0;
    // print until null char or until too many characters counted
    while((chars[i] != 0x00) && (i < 512)){
        put_char_usart1(chars[i]);
        i++;
    }
    if(i < 512){ return 0; } else { return 1; }
}

int main(){

//    volatile uint32_t count = 0;
    uint16_t numRead = 0;

    SysTick_Config(SystemCoreClock/1000); // configure system clock for 1000 ticks/s or 1 ms
    PeripheralInit_GPIO(); // initialize the SS pin
    PeripheralInit_USART1(); // initialize USART1 at A9/TX, A10/RX
    //PeripheralInit_SPI1_Master(); // initialize SPI1 at A5/SCK, A6/MISO, A7/MOSI
    PeripheralInit_SPI2_Master(); // initialize SPI2 at B13/SCK, B14/MISO, B15/MOSI

    GPIO_SetBits(GPIOB, GPIO_Pin_12); // set bit/pin, slave not selected
    // announce start of code over USART1
    serial_print_usart1("Quick example of SPI communication with the L3GD20H gyro.\n");

    //Liga o chip
    GPIO_SetBits(GPIOC, GPIO_Pin_13);

    while(1){
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
        delay_ms(200);
        GPIO_ResetBits(GPIOB, GPIO_Pin_12); // slave select (low)

        // 1st bit to 1 indicate read 0x80. 2nd bit can be zero, don't care
        //transfer_8b_SPI2_Master(0x01); // address WHO_AM_I
        numRead = transfer_8b_SPI2_Master(0xFFFF); // value WHO_AM_I
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
        GPIO_SetBits(GPIOB, GPIO_Pin_12); // slave deselect (high)
        delay_ms(200);

        // if correct value is received, then the L3GD20H is detected
        if(numRead == VAL_WHO_AM_I) {
            serial_print_usart1("Master Received Value....");
            itoa(numRead, palavra, 10);
            serial_print_usart1(palavra);
            serial_print_usart1("\n");
        } else {
            serial_print_usart1("Master Received Wrong....");
            itoa(numRead, palavra, 10);
            serial_print_usart1(palavra);
            serial_print_usart1("\n");
        }

        numRead = 0x00;

    }

}
