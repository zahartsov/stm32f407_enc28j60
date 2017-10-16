/*##################################### GPIO ###################################
������� �������������:
1. ��������� ������������ � �������� RCC->AHB1ENR ���������� ���� RCC_AHB1ENR_GPIO�EN, ��� � - ����� �����.
2. ��������� � ���������� ������������ � �������� RCC->AHB1RSTR ���������� ���� RCC_AHB1RSTR_GPIO�RST, ��� �  - ����� �����.
3. ����� ������ ������ ����� ����� � �������� GPIO�->MODER:
  00 - ���� (~GPIO_MODER_MODER�);
  01 - ����� (GPIO_MODER_MODER�_0);
  10 - �������������� ������� (GPIO_MODER_MODER�_1);
  11 - ��������� ����� (GPIO_MODER_MODER�);
      ��� � - ����� ����.
4. ��������� ������ ������ (GPIO�->OTYPER):
  0 - ����������� ����� (~GPIO_OTYPER_OT_�);
  1 - ����� ��������� ����� (������ ���������� ������� �������) (GPIO_OTYPER_OT_�);
      ��� � - ����� ����.
5. ��������� �������������� ��������� (GPIO�->PUPDR):
  00 - ��� �������� (~GPIO_PUPDR_PUPDR�);
  01 - �������� � �������� ������ (GPIO_PUPDR_PUPDR�_0);
  10 - �������� � ������� ������ (GPIO_PUPDR_PUPDR�_1);
      ��� � - ����� ����
6. ��������� �������� ���� (GPIO�->OSPEEDR), ������ �� �������� ������� ������������ �����:
  00 - ������ ��������, 2 MHz (~GPIO_OSPEEDER_OSPEEDR�);
  01 - ������� ��������, 25 MHz (GPIO_OSPEEDER_OSPEEDR�_0);
  10 - ������� ��������, 50 MHz (GPIO_OSPEEDER_OSPEEDR�_1);
  11 - ����� ������� ��������, 100 MHz (GPIO_OSPEEDER_OSPEEDR�);
      ��� � - ����� ����
7. ��������� �������������� ������� (GPIO�->AFR[0]; GPIO�->FR[1]). GPIO�->AFR[0] - ��� ����� 0-7; GPIO�->AFR[1] - ��� ����� 8-15:
  0000 - AF0 (system)
  0001 - AF1 (TIM1/TIM2)
  0010 - AF2 (TIM3..5)
  0011 - AF3 (TIM8..11)
  0100 - AF4 (I2C1..3)
  0101 - AF5 (SPI1/SPI2)
  0110 - AF6 (SPI3)
  0111 - AF7 (USART1..3)
  1000 - AF8 (USART4..6)
  1001 - AF9 (CAN1/CAN2, TIM12..14)
  1010 - AF10 (OTG_FS, OTG_HS)
  1011 - AF11 (ETH)
  1100 - AF12 (FSMC, SDIO, OTG_HS)
  1101 - AF13 (DCMI)
  1110 - AF14
  1111 - AF15 (EVENTOUT)

��� ���������� �� ����� ������������ ������� GPIO�->IDR (� - ����� �����) � ��������� GPIO_IDR_IDR_� (� - ����� ����).
��� ������ � ���� ������������ ������� GPIO�->ODR (� - ����� �����) � ��������� GPIO�->ODR_ODR_� (� - ����� ����).
��� ��������� ������ ������������ �������� GPIO�->BSRRL � GPIO�->BSRRH (� - ����� �����) � ��������� GPIO_BSRR_BS_� (� - ����� ����).
������ � ������� GPIO�->BSRRL - ��������� 1; � ������� GPIO�->BSRRH - ��������� 0.
################################################################################*/

#include "gpio.h"

void GPIO_Init()
{
  //�������� ������������
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;

  //������������ �����
  RCC->AHB1RSTR |= RCC_AHB1RSTR_GPIODRST/* | RCC_AHB1RSTR_GPIOARST*/;
  RCC->AHB1RSTR &= ~RCC_AHB1RSTR_GPIODRST/* | RCC_AHB1RSTR_GPIOARST)*/;

  //����������� ����� ������
  GPIOD->MODER &= ~(GPIO_MODER_MODER12 | GPIO_MODER_MODER13 | GPIO_MODER_MODER14 | GPIO_MODER_MODER15);
  GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0;

  GPIOA->MODER &= ~GPIO_MODER_MODER0;
  GPIOA->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1 | GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;

  GPIOE->MODER &= ~GPIO_MODER_MODER3;
  GPIOE->MODER |= GPIO_MODER_MODER3_0;

  GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1;

  GPIOC->MODER &= ~GPIO_MODER_MODER13;
  GPIOC->MODER |= GPIO_MODER_MODER13_0;

  //��������� ������ ������
  GPIOD->OTYPER &= ~GPIO_OTYPER_OT_12 & ~GPIO_OTYPER_OT_13 & ~GPIO_OTYPER_OT_14 & ~GPIO_OTYPER_OT_15;
  GPIOA->OTYPER &= ~GPIO_OTYPER_OT_0 & ~GPIO_OTYPER_OT_2 & ~GPIO_OTYPER_OT_3 & ~GPIO_OTYPER_OT_5 & ~GPIO_OTYPER_OT_6 & ~GPIO_OTYPER_OT_7;
  GPIOE->OTYPER &= ~GPIO_OTYPER_OT_3;

  GPIOB->OTYPER = 0;
  GPIOC->OTYPER &= ~GPIO_OTYPER_OT_13;

  //����������� ������������� ��������
  GPIOD->PUPDR |= GPIO_PUPDR_PUPDR12_1 | GPIO_PUPDR_PUPDR13_1 | GPIO_PUPDR_PUPDR14_1 | GPIO_PUPDR_PUPDR15_1;
  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR0 & ~GPIO_PUPDR_PUPDR5 & ~GPIO_PUPDR_PUPDR6 & ~GPIO_PUPDR_PUPDR7;
  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR2_1 | GPIO_PUPDR_PUPDR3_1 /*| GPIO_PUPDR_PUPDR6_1 | GPIO_PUPDR_PUPDR7_1*/;
  GPIOE->PUPDR |= GPIO_PUPDR_PUPDR3_0;
  GPIOC->PUPDR |= GPIO_PUPDR_PUPDR13_0;

  //����������� �������� �����
  GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR12 & ~GPIO_OSPEEDER_OSPEEDR13 & ~GPIO_OSPEEDER_OSPEEDR14 & ~GPIO_OSPEEDER_OSPEEDR15;
  GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR0;
  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2_1 | GPIO_OSPEEDER_OSPEEDR3_1 | GPIO_OSPEEDER_OSPEEDR5 | GPIO_OSPEEDER_OSPEEDR6 | GPIO_OSPEEDER_OSPEEDR7;

  //����������� �������������� �������
  GPIOA->AFR[0] |= 0x7700;              //AF7 �� ���� 2 � 3 ����� � - USART2;
  GPIOA->AFR[0] |= 0x55500000;           //AF5 �� ���� 5, 6 � 7 ����� � - SPI1;
}

void userGPIO_shortInit(GPIO_TypeDef* GPIO, uint8_t pin, uint8_t mode, uint8_t pull, uint8_t AF)
{
  if(GPIO == (GPIO_TypeDef*)GPIOA) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  else if(GPIO == (GPIO_TypeDef*)GPIOB) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  else if(GPIO == (GPIO_TypeDef*)GPIOC) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
  else if(GPIO == (GPIO_TypeDef*)GPIOD) RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
  else if(GPIO == (GPIO_TypeDef*)GPIOE) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  else if(GPIO == (GPIO_TypeDef*)GPIOF) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;

  if(mode == GPIO_MODE_INPUT) GPIO->MODER &= ~(GPIO_MODER_MODER0 << pin*2);
  else GPIO->MODER |= mode << pin*2;
  GPIO->OTYPER &= ~GPIO_OTYPER_OT_0 << pin*2;
  if(pull == GPIO_PULL_NOPULL) GPIO->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << pin*2);
  else GPIO->PUPDR |= pull << pin*2;
  GPIO->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR0_1 << pin*2;

  if(mode == GPIO_MODE_AF)
  {
    if(pin < 8) GPIO->AFR[0] |= AF << pin*4;
    else GPIO->AFR[1] |= AF << pin*4;
  }
}

void userGPIO_Init(GPIO* GPIO)
{
  if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOA) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  else if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOB) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
  else if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOC) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
  else if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOD) RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
  else if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOE) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
  else if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOF) RCC->AHB1ENR |= RCC_AHB1ENR_GPIOFEN;

  if(GPIO->mode == GPIO_MODE_INPUT) GPIO->baseGPIO->MODER &= ~(GPIO_MODER_MODER0 << GPIO->pin*2);
  else GPIO->baseGPIO->MODER |= GPIO->mode << GPIO->pin*2;
  if(GPIO->type == GPIO_TYPE_PUSHPULL )GPIO->baseGPIO->OTYPER &= ~(GPIO_OTYPER_OT_0 << GPIO->pin*2);
  else GPIO->baseGPIO->OTYPER |= GPIO_OTYPER_OT_0 << GPIO->pin*2;
  if(GPIO->pull == GPIO_PULL_NOPULL) GPIO->baseGPIO->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << GPIO->pin*2);
  else GPIO->baseGPIO->PUPDR |= GPIO->pull << GPIO->pin*2;
  if(GPIO->speed == GPIO_SPEED_LOW) GPIO->baseGPIO->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << GPIO->pin*2);
  else GPIO->baseGPIO->OSPEEDR |= GPIO->speed << GPIO->pin*2;

  if(GPIO->mode == GPIO_MODE_AF)
  {
    if(GPIO->pin < 8) GPIO->baseGPIO->AFR[0] |= GPIO->AF << GPIO->pin*4;
    else GPIO->baseGPIO->AFR[1] |= GPIO->AF << GPIO->pin*4;
  }
}

void userEXTI_Init(GPIO* GPIO)
{
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  uint16_t tmp = 0;
  if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOA) tmp |= SYSCFG_EXTICR1_EXTI0_PA << GPIO->pin*4;
  else if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOB) tmp |= SYSCFG_EXTICR1_EXTI0_PB << GPIO->pin*4;
  else if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOC) tmp |= SYSCFG_EXTICR1_EXTI0_PC << GPIO->pin*4;
  else if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOD) tmp |= SYSCFG_EXTICR1_EXTI0_PD << GPIO->pin*4;
  else if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOE) tmp |= SYSCFG_EXTICR1_EXTI0_PE << GPIO->pin*4;
  else if(GPIO->baseGPIO == (GPIO_TypeDef*)GPIOF) tmp |= SYSCFG_EXTICR1_EXTI0_PF << GPIO->pin*4;
  else return;

  if(GPIO->pin < 4) SYSCFG->EXTICR[0] |= tmp;
  else if(GPIO->pin < 8) SYSCFG->EXTICR[1] |= tmp;
  else if(GPIO->pin < 12) SYSCFG->EXTICR[2] |= tmp;
  else if(GPIO->pin < 16) SYSCFG->EXTICR[3] |= tmp;
  else return;

  EXTI->IMR |= EXTI_IMR_MR0 << GPIO->pin;
  //EXTI->EMR |= EXTI_EMR_MR0 << GPIO->pin;

  if(GPIO->interruptRising == EXTI_RISING_DISABLE) EXTI->RTSR &= ~(EXTI_RTSR_TR0 << GPIO->pin);
  else EXTI->RTSR |= EXTI_RTSR_TR0 << GPIO->pin;

  if(GPIO->interruptFalling == EXTI_FALLING_DISABLE) EXTI->FTSR &= ~(EXTI_FTSR_TR0 << GPIO->pin);
  else EXTI->FTSR |= EXTI_FTSR_TR0 << GPIO->pin;

  if(GPIO->pin == 0) NVIC_EnableIRQ(EXTI0_IRQn);
  else if(GPIO->pin == 1) NVIC_EnableIRQ(EXTI1_IRQn);
  else if(GPIO->pin == 2) NVIC_EnableIRQ(EXTI1_IRQn);
  else if(GPIO->pin == 3) NVIC_EnableIRQ(EXTI1_IRQn);
  else if(GPIO->pin == 4) NVIC_EnableIRQ(EXTI1_IRQn);
  else if(GPIO->pin < 10) NVIC_EnableIRQ(EXTI9_5_IRQn);
  else if(GPIO->pin < 16) NVIC_EnableIRQ(EXTI15_10_IRQn);
}