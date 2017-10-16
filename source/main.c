#include "stm32f4xx.h"
#include "configuration.h"

int main()
{
  configureIt();

  vTaskStartScheduler();
  while(1)
  {
    __NOP();
    __NOP();
    __NOP();
    __NOP();
  }
}


/*-----------------------------------------------------------*/
void vApplicationTickHook( void )
{

}
/*-----------------------------------------------------------*/
void vApplicationIdleHook( void )
{

}
/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    for( ;; );
}