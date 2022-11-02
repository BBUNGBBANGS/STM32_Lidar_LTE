#include "stdint.h"
#include "main.h"

extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _estack;

extern void __libc_init_array();

void Reset_Handler()
{
    uint32_t *dataInit = &_sidata;
    uint32_t *data = &_sdata;
    while(data < &_edata)
    {
        *data++ = *dataInit++;
    }

    uint32_t *bss = &_sbss;
    while(bss < &_ebss)
    {
        *bss++ = 0;
    }

    SystemInit(); 
    __libc_init_array();
    main();

    while (1);
}

void Default_Handler(void)
{
    while (1);
}

void SystemTick_Handler(void)
{
    HAL_IncTick();
}

__weak void NMI_Handler(void)                          {Default_Handler();}
__weak void HardFault_Handler(void)                    {Default_Handler();}
__weak void MemManage_Handler(void)                    {Default_Handler();}  
__weak void BusFault_Handler(void)                     {Default_Handler();}  
__weak void UsageFault_Handler(void)                   {Default_Handler();} 
__weak void SVC_Handler(void)                          {Default_Handler();}
__weak void DebugMon_Handler(void)                     {Default_Handler();}
__weak void PendSV_Handler(void)                       {Default_Handler();}
__weak void SysTick_Handler(void)                      {SystemTick_Handler();}
__weak void WWDG_IRQHandler(void)                      {Default_Handler();}
__weak void PVD_IRQHandler(void)                       {Default_Handler();}
__weak void TAMP_STAMP_IRQHandler(void)                {Default_Handler();}
__weak void RTC_WKUP_IRQHandler(void)                  {Default_Handler();}
__weak void FLASH_IRQHandler(void)                     {Default_Handler();}
__weak void RCC_IRQHandler(void)                       {Default_Handler();}
__weak void EXTI0_IRQHandler(void)                     {Default_Handler();}
__weak void EXTI1_IRQHandler(void)                     {Default_Handler();}
__weak void EXTI2_TSC_IRQHandler(void)                 {Default_Handler();}
__weak void EXTI3_IRQHandler(void)                     {Default_Handler();}
__weak void EXTI4_IRQHandler(void)                     {Default_Handler();}
__weak void DMA1_Channel1_IRQHandler(void)             {Default_Handler();}
__weak void DMA1_Channel2_IRQHandler(void)             {Default_Handler();}
__weak void DMA1_Channel3_IRQHandler(void)             {Default_Handler();}
__weak void DMA1_Channel4_IRQHandler(void)             {Default_Handler();}
__weak void DMA1_Channel5_IRQHandler(void)             {Default_Handler();}
__weak void DMA1_Channel6_IRQHandler(void)             {Default_Handler();}
__weak void DMA1_Channel7_IRQHandler(void)             {Default_Handler();}
__weak void ADC1_2_IRQHandler(void)                    {Default_Handler();}
__weak void USB_HP_CAN_TX_IRQHandler(void)             {Default_Handler();}
__weak void USB_LP_CAN_RX0_IRQHandler(void)            {Default_Handler();}
__weak void CAN_RX1_IRQHandler(void)                   {Default_Handler();}
__weak void CAN_SCE_IRQHandler(void)                   {Default_Handler();}
__weak void EXTI9_5_IRQHandler(void)                   {Default_Handler();}
__weak void TIM1_BRK_TIM15_IRQHandler(void)            {Default_Handler();}
__weak void TIM1_UP_TIM16_IRQHandler(void)             {Default_Handler();}
__weak void TIM1_TRG_COM_TIM17_IRQHandler(void)        {Default_Handler();}
__weak void TIM1_CC_IRQHandler(void)                   {Default_Handler();}
__weak void TIM2_IRQHandler(void)                      {Default_Handler();}
__weak void TIM3_IRQHandler(void)                      {Default_Handler();}
__weak void TIM4_IRQHandler(void)                      {Default_Handler();}
__weak void I2C1_EV_IRQHandler(void)                   {Default_Handler();}
__weak void I2C1_ER_IRQHandler(void)                   {Default_Handler();}
__weak void I2C2_EV_IRQHandler(void)                   {Default_Handler();}
__weak void I2C2_ER_IRQHandler(void)                   {Default_Handler();}
__weak void SPI1_IRQHandler(void)                      {Default_Handler();}
__weak void SPI2_IRQHandler(void)                      {Default_Handler();}
__weak void USART1_IRQHandler(void)                    {Default_Handler();}
__weak void USART2_IRQHandler(void)                    {Default_Handler();}
__weak void USART3_IRQHandler(void)                    {Default_Handler();}
__weak void EXTI15_10_IRQHandler(void)                 {Default_Handler();}
__weak void RTC_Alarm_IRQHandler(void)                 {Default_Handler();}
//__weak void USBWakeUp_IRQHandler(void)                 {Default_Handler();}
__weak void TIM8_BRK_IRQHandler(void)                  {Default_Handler();}
__weak void TIM8_UP_IRQHandler(void)                   {Default_Handler();}
__weak void TIM8_TRG_COM_IRQHandler(void)              {Default_Handler();}
__weak void TIM8_CC_IRQHandler(void)                   {Default_Handler();}
__weak void ADC3_IRQHandler(void)                      {Default_Handler();}
__weak void SPI3_IRQHandler(void)                      {Default_Handler();}
__weak void UART4_IRQHandler(void)                     {Default_Handler();}
__weak void UART5_IRQHandler(void)                     {Default_Handler();}
__weak void TIM6_DAC_IRQHandler(void)                  {Default_Handler();}
__weak void TIM7_IRQHandler(void)                      {Default_Handler();}                   
__weak void DMA2_Channel1_IRQHandler(void)             {Default_Handler();}
__weak void DMA2_Channel2_IRQHandler(void)             {Default_Handler();}
__weak void DMA2_Channel3_IRQHandler(void)             {Default_Handler();}
__weak void DMA2_Channel4_IRQHandler(void)             {Default_Handler();}
__weak void DMA2_Channel5_IRQHandler(void)             {Default_Handler();}
__weak void ADC4_IRQHandler(void)                      {Default_Handler();}
__weak void COMP1_2_3_IRQHandler(void)                 {Default_Handler();}
__weak void COMP4_5_6_IRQHandler(void)                 {Default_Handler();} 
__weak void COMP7_IRQHandler(void)                     {Default_Handler();}
__weak void USB_HP_IRQHandler(void)                    {Default_Handler();}
__weak void USB_LP_IRQHandler(void)                    {Default_Handler();}
__weak void USBWakeUp_RMP_IRQHandler(void)             {Default_Handler();}
__weak void FPU_IRQHandler(void)                       {Default_Handler();}

__attribute__((section(".isr_vector")))

const void (*VectorTable[])(void) = 
{
    (const void (*)(void))&_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0,
	0,
	0,
	0,
	SVC_Handler,
	DebugMon_Handler,
	0,
	PendSV_Handler,
	SysTick_Handler,
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMP_STAMP_IRQHandler,
	RTC_WKUP_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_TSC_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Channel1_IRQHandler,
	DMA1_Channel2_IRQHandler,
	DMA1_Channel3_IRQHandler,
	DMA1_Channel4_IRQHandler,
	DMA1_Channel5_IRQHandler,
	DMA1_Channel6_IRQHandler,
	DMA1_Channel7_IRQHandler,
	ADC1_2_IRQHandler,
	USB_HP_CAN_TX_IRQHandler,
	USB_LP_CAN_RX0_IRQHandler,
	CAN_RX1_IRQHandler,
	CAN_SCE_IRQHandler,
	EXTI9_5_IRQHandler,
	TIM1_BRK_TIM15_IRQHandler,
	TIM1_UP_TIM16_IRQHandler,
	TIM1_TRG_COM_TIM17_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
	USART3_IRQHandler,
	EXTI15_10_IRQHandler,
	RTC_Alarm_IRQHandler,
	USBWakeUp_IRQHandler,
	TIM8_BRK_IRQHandler,
	TIM8_UP_IRQHandler,
	TIM8_TRG_COM_IRQHandler,
	TIM8_CC_IRQHandler,
	ADC3_IRQHandler,
	0,
	0,
	0,
	SPI3_IRQHandler,
	UART4_IRQHandler,
	UART5_IRQHandler,
	TIM6_DAC_IRQHandler,
	TIM7_IRQHandler,
	DMA2_Channel1_IRQHandler,
	DMA2_Channel2_IRQHandler,
	DMA2_Channel3_IRQHandler,
	DMA2_Channel4_IRQHandler,
	DMA2_Channel5_IRQHandler,
	ADC4_IRQHandler,
	0,
	0,
	COMP1_2_3_IRQHandler,
	COMP4_5_6_IRQHandler,
	COMP7_IRQHandler,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	USB_HP_IRQHandler,
	USB_LP_IRQHandler,
	USBWakeUp_RMP_IRQHandler,
	0,
	0,
	0,
	0,
	FPU_IRQHandler
};
