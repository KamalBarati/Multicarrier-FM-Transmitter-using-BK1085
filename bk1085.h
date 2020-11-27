#ifndef _BK1085_H_
#define _BK1085_H_

#include <stdint.h>


typedef enum {

	BK_FREQ=0,
	//BK_CLOCK,
	BK_TRAN_PWR,
	BK_GAIN,
	BK_ST,
	BK_BY_PASS,
	BK_PM
}BK_CMD;

typedef enum {
	NOISE_LO,
	NOISE_MID,
	NOISE_HI
}FM_NOISE;

#define FM_FREQ_00 1028 //bruno
#define FM_FREQ_01 968  //stella
#define FM_FREQ_02 1050  //dj
#define FM_FREQ_03 1045  //rtl102.5
#define FM_FREQ_04 1047  //rtl102.5-2




#define BK_FREQ_ADD	0x00
#define BK_REG_2		0x02
#define BK_REG_3		0x03
#define BK_REG_4		0x04

#define BK_REG2_PILOT_DEV_MASK	0x000F
#define BK_REG2_AUD_DEV_MASK		0x01F0
#define BK_REG2_MONO_MASK			0x0200
#define BK_REG2_BYP_EM_MASK		0x1000
#define BK_REG2_GAIN_MASK			0xE000

#define BK_REG3_PWR_MASK			0x000F
#define PWR_LEVEL_16dBm		0
#define PWR_LEVEL_10dBm		1
#define PWR_LEVEL_5dBm			3
#define PWR_LEVEL_0dBm			7


#define BK_REG4_XTAL_PD_MASK			0x0001
#define BK_REG4_RF_PD_MASK			0x0002
#define BK_REG4_R_PD_MASK			0x0010
#define BK_REG4_L_PD_MASK			0x0020
#define BK_REG4_MIC_GAIN_MASK			0x0030
#define BK_REG4_PA_PD_MASK			0x0100
#define BK_REG4_PLL_PD_MASK			0x0200
#define BK_REG4_XTAL			0x3ffff
#define BK_REG4_16MHZ			31250

void BK1085_Config(uint16_t para,BK_CMD bk_cmd);
void BK_I2C_init(void);


#endif
