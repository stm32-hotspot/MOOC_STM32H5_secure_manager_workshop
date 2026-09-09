/**
  ******************************************************************************
  * @file    loader.c
  * @author  MCD Application Team
  * @brief   Test Protections module.
  *          This file provides set of firmware functions to manage Test Protections
  *          functionalities.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32h5xx_hal.h"
#include "loader.h"

#define BOOTLOADER_BASE_NS              (0x0BF97000)
#define CODE_SECONDARY_OFFSET           (0x001E0000)
#define CODE_SECONDARY_SIZE             (0x00020000)
#ifdef DATA_IMAGE_EN
#define DATA_SECONDARY_OFFSET           (0x001DE000)
#define DATA_SECONDARY_SIZE             (0x00002000)
#endif

/* Exported types ------------------------------------------------------------*/
struct sau_cfg_t
{
  uint32_t RNR;
  uint32_t RBAR;
  uint32_t RLAR;
};


/* Exported macros -----------------------------------------------------------*/
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/* Exported functions ------------------------------------------------------- */
void mpu_loader_cfg(void);
void gtzc_loader_cfg(void);
void gpio_loader_cfg(void);
void sau_loader_cfg(void);
void nvic_loader_cfg(void);

#define BOOTLOADER_BASE_NS                  (0x0BF97000)
#define BOOTLOADER_SIZE                     (0x9400)
/* Engi bits */
#define ENGI_START                          (0x08FFF800UL)
#define ENGI_SIZE                           (0x40UL)

extern uint32_t __ICFEDIT_region_RAM_start__;
extern uint32_t __ICFEDIT_region_RAM_end__;
void clear_all_sram(void);
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief This function is called to clear all SRAM area before jumping
  * in the non secure bootloader.
  * @note
  * @retval void
  */
void clear_all_sram(void)
{
  __IO uint32_t *pt = (uint32_t *)__ICFEDIT_region_RAM_start__;
  uint32_t index;
  uint32_t size = (__ICFEDIT_region_RAM_end__-__ICFEDIT_region_RAM_start__+1)/4;

  for (index = 0; index < size; index++)
  {
    pt[index] = 0;
  }
}

/**
  * @brief  Display the TEST Main Menu choices on HyperTerminal
  * @param  Testnumber giving the test that generate a reset
  * @retval None.
  */
void LOADER_Run(void)
{
  /* configure GTZC to allow non secure / privileged loader execution */
  gtzc_loader_cfg();

  /* configure GPIO to allow non secure / privileged loader execution */
  gpio_loader_cfg();

  /* reconfigure MPU to allow non secure / privileged loader execution */
  mpu_loader_cfg();

  /* configure SAU to allow non secure / privileged loader execution */
  sau_loader_cfg();

  /* Configure NVIC */
  nvic_loader_cfg();

  uint32_t boot_address = cmse_nsfptr_create(*(uint32_t *)(BOOTLOADER_BASE_NS + 4U));

  /*Increment HDPL to HDPL=3*/
  SET_BIT(SBS->HDPLCR,  SBS_HDPLCR_INCR_HDPL);

  __TZ_set_MSP_NS((*(uint32_t *)BOOTLOADER_BASE_NS));
  SCB_NS->VTOR = BOOTLOADER_BASE_NS;


  asm volatile("movs r0, %0\n"
               "movs r1, #0\n" /*clear registers before jumping to non-secure*/
               "movs r2, #0\n"
               "movs r3, #0\n"
               "movs r4, #0\n"
               "movs r5, #0\n"
               "movs r6, #0\n"
               "movs r7, #0\n"
               "mov r8, r5\n"
               "mov r9, r5\n"
               "mov r10, r5\n"
               "mov r11, r5\n"
               "mov r12, r5\n"
               "MSR APSR_nzcvq,r1\n" /*clear APSR*/
               "bxns r0\n"::"r"(boot_address)); /*jump to non-secure address*/
  /*BXNS, no return here possible*/
}

/*********************************
  * Loader specific configuration *
  *********************************/

/* GPIO configuration
  =================== */
/*----------------------|  USART  |-------------------------------------*/
/* USART: USART1 + USART2 + USART 3*/
/* USART1 (RX = PA10, TX = PA9), USART2 (RX = PA3, TX = PA2)  */
/* USART3 (RX = PD9, TX = PD8) */

/*----------------------|  SPI  |-------------------------------------*/
/* SPI: SPI1 + SPI2 + SPI3*/
/* SPI1: _MOSI  =  PA7, _MISO = PA6 , _SCK = PA5, _NSS= PA4 */
/* SPI2: _MOSI  =  PC1, _MISO = PB14 , _SCK = PB10, _NSS= PB12 */
/* SPI3: _MOSI  =  PC12, _MISO = PC11 , _SCK = PC10, !! _NSS= PA15 */

/*----------------------| I2C  |---------------------------------------*/
/*I2C:  I2C3 + I2C4*/
/*I2C3 (_SCL: PA8, _SDA:PC9 ) */
/*I2C4 (_SCL=PD12, _SDA=PD13 ) */

/*----------------------| I3C  |---------------------------------------*/
/*I3C1 (_SCL: PB6, _SDA:PB7 ) */

/*----------------------|  USB |---------------------------------------*/
/* USB*/
/*USB (_DM = PA11, _DP = PA12 )*/

/*----------------------|  FDCAN  |------------------------------------*/
/*FDCAN2 */
/*CAN (_RX = PB5, _TX = PB13 ) */

#define GPIOA_MASK_SECCFG    (GPIO_PIN_10 | GPIO_PIN_9  | GPIO_PIN_3  | GPIO_PIN_2  | GPIO_PIN_7  | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_4 | \
                              GPIO_PIN_15 | GPIO_PIN_8  | GPIO_PIN_11 | GPIO_PIN_12)
#define GPIOB_MASK_SECCFG    (GPIO_PIN_14 |\
                              GPIO_PIN_10 | GPIO_PIN_12 | GPIO_PIN_6  | GPIO_PIN_7  | GPIO_PIN_5 | GPIO_PIN_13)
#define GPIOC_MASK_SECCFG    (GPIO_PIN_1  | GPIO_PIN_12 | GPIO_PIN_11 | GPIO_PIN_10 | GPIO_PIN_9)
#define GPIOD_MASK_SECCFG    (GPIO_PIN_9  | GPIO_PIN_8  | GPIO_PIN_12 | GPIO_PIN_13)

/* Peripherals configuration
   ========================= */
/*----------------------|  USART  |-------------------------------------*/
/* USART: USART1 + USART2 + USART3 */

/*----------------------|  SPI  |-------------------------------------*/
/* SPI: SPI1 + SPI2 + SPI3 */

/*----------------------| I2C  |---------------------------------------*/
/* I2C: I2C3 + I2C4 */

/*----------------------| I3C  |---------------------------------------*/
/* I3C: I3C1*/

/*----------------------|  USB |---------------------------------------*/
/* USB */

/*----------------------|  FDCAN  |------------------------------------*/
/* FDCAN1 */

/*----------------------|  ICACHE  |------------------------------------*/
/* ICACHE */

/*----------------------|  IWDG |------------------------------------*/
/* IWDG */

#define TZSC_MASK_R1  (GTZC_CFGR1_USART2_Msk | GTZC_CFGR1_USART3_Msk | GTZC_CFGR1_SPI3_Msk  | GTZC_CFGR1_SPI2_Msk | \
                       GTZC_CFGR1_I3C1_Msk   | GTZC_CFGR1_IWDG_Msk)
#define TZSC_MASK_R2  (GTZC_CFGR2_USART1_Msk | GTZC_CFGR2_SPI1_Msk   | GTZC_CFGR2_I2C3_Msk  | GTZC_CFGR2_I2C4_Msk  | \
                       GTZC_CFGR2_USB_Msk  | GTZC_CFGR2_FDCAN2_Msk  | GTZC_CFGR2_FDCAN1_Msk | GTZC_CFGR2_UCPD1_Msk)
#define TZSC_MASK_R3  (GTZC_CFGR3_ICACHE_REG_Msk | GTZC_CFGR3_CRC_Msk)

/* SAU configuration
   ================= */
const struct sau_cfg_t sau_load_cfg[] =
{
  /* allow non secure access to SRAM1 */
  {
    0,
    (uint32_t)SRAM1_BASE_NS,
    ((uint32_t)SRAM1_BASE_NS + SRAM1_SIZE - 1U),
  },
  /* allow non secure access to periph */
  {
    1,
    (uint32_t)PERIPH_BASE_NS,
    ((uint32_t)PERIPH_BASE_NS + 0xFFFFFFFUL),
  },
  /* allow non secure access to all user flash except secure part and area covered by HDP extension */
  {
    2,
    (uint32_t)FLASH_BASE_NS,
    (uint32_t)(FLASH_BASE_NS + FLASH_SIZE_DEFAULT - 1U),
  },
  /* allow non secure access to bootloader code */
  {
    3,
    (uint32_t)BOOTLOADER_BASE_NS,
    ((uint32_t)BOOTLOADER_BASE_NS + BOOTLOADER_SIZE - 1U),
  },
  /* allow non secure access to Engi bits */
  {
    4,
    (uint32_t)ENGI_START,
    ((uint32_t)ENGI_START + ENGI_SIZE - 1U),
  },
};

/* NVIC configuration
   ================== */
/*Interrupts 0 .. 31 */
/*in ITNS0 GPDMA1_Channel0_IRQn | GPDMA1_Channel1_IRQn | GPDMA1_Channel2_IRQn bit27|28|29  is non secure (1) */
#define RSS_NVIC_INIT_ITNS0_VAL      (0x38000000U)

/*Interrupts 32 .. 63 */
/*in ITNS1 no bit is non secure (1) */
#define RSS_NVIC_INIT_ITNS1_VAL      (0x00000000U)

/*Interrupts 64 .. 95 */
/*in ITNS2 OTG_FS_IRQn 74, i.e bit 10 is non secure (1) */
#define RSS_NVIC_INIT_ITNS2_VAL      (0x00000400U)

/*Interrupts 96 .. 128 */
/*I3C1_EV_IRQn = 123*/
#define RSS_NVIC_INIT_ITNS3_VAL      (0x08000000U)

/* SRAM1 configuration
   =================== */
/* SRAM1 NB super-block */
#define GTZC_MPCBB1_NB_VCTR (16U)

/* SRAM3 NB super-block */
#define GTZC_MPCBB3_NB_VCTR (20U)

/* MPCBB : All SRAM block privileged only */
#define GTZC_MPCBB_ALL_PRIV (0xFFFFFFFFUL)

/* MPCBB : All SRAM block non secure */
#define GTZC_MPCBB_ALL_NSEC (0x00000000UL)

/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  configure MPU before jumping into loader
  * @param  pOBK_Hdpl1Cfg: Configuration (flash layout) stored into OBkeys Hdpl 1
  * @retval None
  */
void mpu_loader_cfg(void)
{
   /* disable the secure MPU already enabled by the STIROT FW */
  /* Reset all fields as enable does full setup */
  MPU->CTRL = 0;
}

/**
  * @brief  GTZC configuration before jumping into loader
  * @param  None
  * @retval None
  */
void gtzc_loader_cfg(void)
{
  uint32_t i;

  /* configuration stage */
  __HAL_RCC_GTZC1_CLK_ENABLE();

  /* Allow secure to access to non secure */
  GTZC_MPCBB1_S->CR |= GTZC_MPCBB_CR_SRWILADIS_Msk;

  /* All bocks of SRAM1 configured non secure / privileged (default value)  */
  for (i = 0; i < GTZC_MPCBB1_NB_VCTR; i++)
  {
    /*SRAM1 -> MPCBB1*/
    GTZC_MPCBB1_S->SECCFGR[i] = GTZC_MPCBB_ALL_NSEC;
  }

  /* Required peripherals configured non secure (default value) / privileged */
  GTZC_TZSC1_S->PRIVCFGR1 = TZSC_MASK_R1;
  GTZC_TZSC1_S->PRIVCFGR2 = TZSC_MASK_R2;
  GTZC_TZSC1_S->PRIVCFGR3 = TZSC_MASK_R3;
}

/**
  * @brief  GPIO configuration before jumping into loader
  * @param  None
  * @retval None
  */
void gpio_loader_cfg(void)
{
  /* configuration stage */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /* Required GPIO configured non secure */
  GPIOA_S->SECCFGR = ~GPIOA_MASK_SECCFG;
  GPIOB_S->SECCFGR = ~GPIOB_MASK_SECCFG;
  GPIOC_S->SECCFGR = ~GPIOC_MASK_SECCFG;
  GPIOD_S->SECCFGR = ~GPIOD_MASK_SECCFG;
}

/**
  * @brief  Sau idau configuration before jumping into loader
  * @param  pOBK_Hdpl1Cfg: Configuration (flash layout) stored into OBkeys Hdpl 1
  * @retval None
  */
void sau_loader_cfg(void)
{
  uint32_t i = 0U;

  for (i = 0U; i < ARRAY_SIZE(sau_load_cfg); i++)
  {
    SAU->RNR = sau_load_cfg[i].RNR;
    SAU->RBAR = sau_load_cfg[i].RBAR & SAU_RBAR_BADDR_Msk;
    SAU->RLAR = (sau_load_cfg[i].RLAR & SAU_RLAR_LADDR_Msk) |
                SAU_RLAR_ENABLE_Msk;
  }

  /* Force memory writes before continuing */
  __DSB();

  /* Flush and refill pipeline with updated permissions */
  __ISB();

  /* Enable SAU */
  TZ_SAU_Enable();
}

/**
  * @brief  configure NVIC before jumping into loader
  * @param  None
  * @retval None
  */
void nvic_loader_cfg(void)
{
  /* Enable HardFault/busFault and NMI exception in ns.
   * It is up to BL to drive non-secure faults
   * Do not enter in secure on non-secure fault
   */
  SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) |
                           (SCB->AIRCR & 0x0000FFFFU) |
                           SCB_AIRCR_BFHFNMINS_Msk);


  NVIC->ITNS[0U] = RSS_NVIC_INIT_ITNS0_VAL;
  NVIC->ITNS[1U] = RSS_NVIC_INIT_ITNS1_VAL;
  NVIC->ITNS[2U] = RSS_NVIC_INIT_ITNS2_VAL;
  NVIC->ITNS[3U] = RSS_NVIC_INIT_ITNS3_VAL;
}
