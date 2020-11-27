/*--------------------------------------------------------------------------*/
/**@file     bk1085.c
   @brief    FM transmiter
   @details
   @author
   @date   2011-3-7
   @note
*/
/*----------------------------------------------------------------------------*/
#include "bk1085.h"
#include <stdint.h>
#include <mega328p.h>
#include <delay.h>

uint8_t BK_reg[4];
uint16_t BK_reg_2_shadow=0;
uint16_t BK_reg_3_shadow=0;
const uint32_t freq_table[] = {0x2E286BD, 0x2E435E5, 0x2E5E50D, 0x2E79436, 0x2E9435E, 0x2EAF287, 0x2ECA1AF, 0x38286BD, 0x38435E5, 0x385E50D, 0x3879436, 0x389435E, 0x38AF287, 0x38CA1AF};

#define I2C_PORT PORTC
#define I2C_PINPORT PINC
#define I2C_DATA_PIN PINC4
#define I2C_CLOCK_PIN PINC5

#define BK_SDADIROUT()       		DDRC |= 1<<I2C_DATA_PIN
#define BK_SDADIRIN()        		DDRC &= ~(1<<I2C_DATA_PIN)

#define BK_1085_DATA_HIGH()   	I2C_PORT |= 1<<I2C_DATA_PIN
#define BK_1085_DATA_LOW()      I2C_PORT &= ~(1<<I2C_DATA_PIN)
//#define BK_1085_DATA_READ()    	I2C_DATA_PIN


#define BK_SCLDIROUT()        	   DDRC |= 1<<I2C_CLOCK_PIN
#define BK_1085_CLK_HIGH()     I2C_PORT |= 1<<I2C_CLOCK_PIN
#define BK_1085_CLK_LOW()      I2C_PORT &= ~(1<<I2C_CLOCK_PIN)

#define SER_SPED 20


void BK_I2C_init(void)
{
    BK_SDADIROUT();                  //SDA output
    BK_SCLDIROUT();                  //SCL output
    BK_1085_CLK_HIGH();
    BK_1085_DATA_HIGH();
}

void BK_I2C_Start(void)
{
   // BK_I2C_init();
    delay_us(SER_SPED);
    BK_1085_DATA_LOW();
    delay_us(SER_SPED);
    BK_1085_CLK_LOW();
    delay_us(SER_SPED);
    BK_1085_DATA_HIGH();
}

void BK_I2C_Stop(void)
{
    BK_SDADIROUT();
    BK_1085_DATA_LOW();
    delay_us(SER_SPED);
    BK_1085_CLK_HIGH();
    delay_us(SER_SPED);
    BK_1085_DATA_HIGH();
    delay_us(SER_SPED);
}


void BK_I2C_ack(void)
{
    BK_SDADIROUT();

    BK_1085_CLK_LOW();
    BK_1085_DATA_LOW();
    delay_us(SER_SPED);

    BK_1085_CLK_HIGH();
    delay_us(SER_SPED);
    BK_1085_CLK_LOW();
}      

void BK_I2C_nack(void)
{
    BK_SDADIROUT();

    BK_1085_CLK_LOW();
    BK_1085_DATA_HIGH();
    delay_us(SER_SPED);

    BK_1085_CLK_HIGH();
    delay_us(SER_SPED);
    BK_1085_CLK_LOW();
}

uint8_t BK_I2C_ReceiveACK(void)
{
    uint32_t ackflag;
    BK_SDADIRIN();
    delay_us(SER_SPED);
    BK_1085_CLK_HIGH();
    delay_us(SER_SPED);
    //ackflag = (uint8_t)BK_1085_DATA_READ();
    if(I2C_PINPORT & I2C_DATA_PIN)
    	ackflag=1;
    else
    	ackflag=0;
    BK_1085_CLK_LOW();
    delay_us(SER_SPED);
    return ackflag;
}

void BK_I2C_sendbyte(uint8_t I2CSendData)
{
    //uint8_t data = I2CSendData & ~(1<<0); //set the write condition
    uint8_t data = I2CSendData;
    uint8_t  i;
    BK_SDADIROUT();
    //delay_us(1);
    for(i = 8; i>0; i--)
    {
        if(data & 0x80)
        {
            BK_1085_DATA_HIGH(); //if high bit is 1,SDA= 1
        }
        else
            BK_1085_DATA_LOW();  //else SDA=0

       	delay_us(18);
        BK_1085_CLK_HIGH();
       	delay_us(SER_SPED);
       	data <<= 1;              //shift left 1 bit
        BK_1085_CLK_LOW();
    }                       
}
void bk1085_multi_write(uint8_t dev_add,uint8_t *buf,uint8_t lenth)
{
    uint8_t  byte;

    BK_I2C_Start();                    //I2C真真
    //BK_I2C_sendbyte(0x1d<<1);  //dev id is 7 bit, left shift is required
    BK_I2C_sendbyte(0b00011101); //7 bit address +1
    BK_I2C_ReceiveACK();
    BK_I2C_sendbyte((dev_add<<1));
    BK_I2C_ReceiveACK();

    for(byte = 0;byte<lenth;byte++)
     {
	  BK_I2C_sendbyte(*(buf+byte)); //send data
      BK_I2C_ReceiveACK();
     }

    BK_I2C_Stop();                  //I2C真真
}

void set_freq(uint16_t para)
{
    uint32_t freq_temp = freq_table[para];
    BK_reg[0] = (uint8_t)(freq_temp>>8);  //86 R1-->D15-D8
    BK_reg[1] = (uint8_t)(freq_temp);     //db R1-->D7-D0
    BK_reg[2] = (uint8_t)(freq_temp>>24); //02 R0-->D15-D8
    BK_reg[3] = (uint8_t)(freq_temp>>16); //e2 R0-->D7-D0
    
    bk1085_multi_write(BK_FREQ_ADD,BK_reg,4); 
}

void set_clock(uint16_t para)
{
    uint32_t clock = para;
    BK_reg[0] = (uint8_t)(clock>>8); //D17-D8
    BK_reg[1] = (uint8_t)(clock);    //D7-D0
    BK_reg[2] = (uint8_t)(clock);    //D15-D8

    bk1085_multi_write(BK_FREQ_ADD,BK_reg,4);
}
void set_bk_power(uint16_t para)
{
	para = para&BK_REG3_PWR_MASK;
	
	BK_reg[0] = 	para>>8;
	BK_reg[1] = 	para&0x00FF;
	
	bk1085_multi_write(BK_REG_3,BK_reg,2);     
          
}
void set_bk_stereo(uint16_t para)
{
	BK_reg_2_shadow &= ~BK_REG2_MONO_MASK;
	para = para&BK_REG2_MONO_MASK;
	BK_reg_2_shadow |=para;
	
	BK_reg[0] = 	BK_reg_2_shadow>>8;
	BK_reg[1] = 	BK_reg_2_shadow&0x00FF;
	
	bk1085_multi_write(BK_REG_2,BK_reg,2);     
}
void set_bk_gain(uint16_t para)
{
	BK_reg_2_shadow &= ~BK_REG2_GAIN_MASK;
	para = para&BK_REG2_GAIN_MASK;
	BK_reg_2_shadow |=para;
	
	BK_reg[0] = 	BK_reg_2_shadow>>8;
	BK_reg[1] = 	BK_reg_2_shadow&0x00FF;
	
	bk1085_multi_write(BK_REG_2,BK_reg,2);     
}
void set_bk_pmu(uint16_t para)
{

}
void BK1085_Config(uint16_t para,BK_CMD bk_cmd)
{
	switch(bk_cmd)
	{
		case BK_FREQ:
			set_freq(para);
			break;
	  //case BK_CLOCK:
		  //set_COLCK(para);
		//	break;
		case BK_TRAN_PWR:
			set_bk_power(para);
			break;
		case BK_GAIN:
			set_bk_gain(para);
			break;
		case BK_ST:
			set_bk_stereo(para);			
			break;
		case BK_BY_PASS:
			break;
		case BK_PM:
			set_bk_pmu(para);
			break;
		default:break;

	}
}

void FM_NOISE_Config(FM_NOISE fm_noise)
	{
		switch(fm_noise)
		{
			case NOISE_LO:
			TCCR1B = 0b101; //TIMER1 CLK/1024 PRESC
			break;

			case NOISE_MID:
			TCCR1B = 0b100; //TIMER1 CLK/256 PRESC
			break;

			case NOISE_HI:
			TCCR1B = 0b010; //TIMER1 CLK/8 PRESC
			break;

	     default:break;

		}  
        
}

void bk1085_init(void)
{
	BK1085_Config(1028,BK_FREQ);
	BK1085_Config(PWR_LEVEL_0dBm,BK_TRAN_PWR);
}


