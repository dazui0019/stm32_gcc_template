#include "app.h"
#include "main.h"
#include "usart.h"
#include "printf_tiny.h"

uint8_t* hell_str = (uint8_t*)"Hello, World!\r\n";

int setup(void)
{
    return 0;
}

void loop(void)
{
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    printf_("hello, world!\r\n");
    HAL_Delay(500);
}

void _putchar(char character){
    uint8_t ch = character;
    HAL_UART_Transmit(&huart2, &ch, 1, 100);
}