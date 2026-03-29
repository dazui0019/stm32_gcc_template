#include "app.h"
#include "main.h"

int setup(void)
{
    return 0;
}

void loop(void)
{
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    HAL_Delay(100);
}