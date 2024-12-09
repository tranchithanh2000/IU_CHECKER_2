#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
#include "r_cg_userdefine.h"

//define��`
//EEPROM �R�}���h�錾
#define EEP_CMD_READ 0x03	//�ǂݏo��
#define EEP_CMD_WRITE 0x02	//��������
#define EEP_CMD_WREN 0x06	//�������݋���
#define EEP_CMD_WRDI 0x04	//�������݋֎~
#define EEP_CMD_RDSR 0x05	//�X�e�[�^�X�ǂݏo��

#define EEP_CMD_WRSR 0x01	//�X�e�[�^�X��������
#define EEP_CMD_PE 	 0x42	//�y�[�W�C���[�X
#define EEP_CMD_SE	 0xd8	//�Z�N�^�[�C���[�X
#define EEP_CMD_CE   0xc7	//�`�b�v�C���[�X
#define EEP_CMD_RDID 0xab	//ID�ǂݏo��
#define EEP_CMD_DPD  0xb9	//POWER DOWN

//�|�[�g�ݒ�@���ۂ̃|�[�g�Ɋ���U��
#define FPGA_PROG_B		P5_bit.no0
#define FPGA_INIT_B		P0_bit.no1
#define FPGA_DONE		P1_bit.no0	//(P1 & 0x01)
#define FPGA_DIN		P1_bit.no2
#define FPGA_CCLK		P1_bit.no0

#if 0
#define	EEPROM_CS	P5_bit.no0		//P50 OUT �`�b�v�Z���N�g
#define	EEPROM_SO	P12_bit.no4;	//P124 IN  �f�[�^����
#define	EEPROM_SI	P14_bit.no7;	//P147 OUT �f�[�^�o��
#define	EEPROM_SCK	P14_bit.no5;	//P145 OUT �N���b�N
#else
unsigned short EEPROM_CS;	//�f�o�b�O�p
unsigned short EEPROM_SO;	//�f�o�b�O�p
unsigned short EEPROM_SI;	//�f�o�b�O�p
unsigned short EEPROM_SCK;	//�f�o�b�O�p
#endif

//�֐��v���g�^�C�v�錾
unsigned char eeprom_din( void );
void eeprom_dout( unsigned char data );
void eeprom_cmd( unsigned char data );
unsigned char eeprom_readst( void );
void eeprom_init( void );
void eeprom_end( void );
unsigned int eeprom_write( unsigned long add, unsigned int cnt, unsigned char *data );
unsigned int eeprom_read( unsigned long add, unsigned int cnt, unsigned char *data );
void nop_wait( unsigned short cnt );
void wait_1ms( unsigned short cnt );
void wait_05us( unsigned short cnt );

//�O���[�o���ϐ�
extern volatile uint16_t  g_csi00_tx_count;            /* csi00 send data count */
extern unsigned int g_Sendend;

//�֐�


#define FPGA_DATA_CNT 256		//FPGA�]���o�C�g��
#define FPGA_DATA_OFF 0x67		//�f�[�^�I�t�Z�b�g

unsigned short fpga_config3(void)
{
	unsigned long lp1;
	unsigned long tmp = 0;
	unsigned long tmp1 = 0;
	unsigned char data[FPGA_DATA_CNT];
	
	FPGA_PROG_B = 1;			//PROG_B = H

	while( FPGA_INIT_B == 0 )	//INIT_B��H�ɂȂ�̂�҂�
		;

	wait_1ms(1);				//1ms�҂�

	//FPGA�f�[�^�ǂݏo���A�f�[�^���M�����擾����
	eeprom_read( 0, FPGA_DATA_CNT, &data[0] );	//FPGA�f�[�^�ǂݏo��
	tmp  = data[0x64 + 0] * 0x10000;	//���M�f�[�^�o�C�g���̎擾
	tmp += data[0x64 + 1] * 0x100;
	tmp += data[0x64 + 2];
	
	if( tmp == 0xfffff ){	//EEPROM���������݁H
		return 0;
	}
	
	FPGA_PROG_B = 0;			//PROG_B = L�@FPGA�R���t�B�O���[�V�����N���A
	wait_05us(2);				//1us�҂��iPROG_B��L�p���X500ns�ȏ�j
	
	while( FPGA_INIT_B != 0 )	//INIT_B��L�ɂȂ�̂�҂�
		;

	FPGA_PROG_B = 1;			//PROG_B = H�@FPGA�������J�n
	
	while( FPGA_INIT_B == 0 )	//INIT_B��H�ɂȂ�̂�҂iFPGA�����������҂��j
		;

	wait_1ms(1);				//1ms�҂��i500ns�ȏ�҂j
//	for( lp = 0 ; lp < 5000; lp++){	//1ms�҂�
//		NOP();
//	}

//	tmp += FPGA_DATA_OFF; 				//�f�[�^�ł͂Ȃ��w�b�_�̗e�ʂ����Z
	
	lp1 = 0;
	g_Sendend = 0;

	while( tmp > 0 ){
		eeprom_read( lp1 * FPGA_DATA_CNT + FPGA_DATA_OFF, FPGA_DATA_CNT, &data[0] );	//FPGA�f�[�^�ǂݏo��

		if( tmp > FPGA_DATA_CNT ){
			tmp1 = FPGA_DATA_CNT;
		}
		else{
			tmp1 = tmp;
		}

		g_Sendend = 1;
//		R_CSI11_Start();
//		R_CSI11_Send( &data[0], tmp1 );	//FPGA�f�[�^���M�J�n
		
		while(g_Sendend != 0) 	//���M�����҂�
			;

		tmp = tmp - tmp1;
		lp1 = lp1 + 1;
		
	}

	while( FPGA_DONE != 0 )	//FPGA�v���O���������҂�
		;

	return 1;
}



//EEPROM 1�o�C�g�R�}���h�o��
void eeprom_cmd( unsigned char data )
{
	eeprom_init();
	eeprom_dout( data );
	eeprom_end();
}

//EEPROM�X�e�[�^�X�ǂݏo��
unsigned char eeprom_rdsr( void )
{
	unsigned char tmp = 0;
	
	eeprom_init();
	eeprom_dout( EEP_CMD_RDSR );
	tmp = eeprom_din();
	eeprom_end();
	
	return tmp;
}

//�ő�256�o�C�g��������
unsigned int eeprom_write( unsigned long add, unsigned int cnt, unsigned char *data )
{
	unsigned short intlp;
	unsigned int tmp = 0;
	
	if( cnt == 0 ){
		return 0;
	}
	
	if( cnt <= 256 ){
		tmp = cnt;
	}
	else{
		tmp = 256;
	}
	
	eeprom_init();

	eeprom_dout( EEP_CMD_WRITE );
	eeprom_dout( (add >> 16) & 0xff );
	eeprom_dout( (add >> 8 ) & 0xff );
	eeprom_dout( add & 0xff );
	
	for( intlp = 0; intlp < tmp; intlp++ ){
		eeprom_dout( data[intlp] );
	}
	eeprom_end();
	
	//�������ݑ҂��̓X�e�[�^�X��ǂݏo���B
	
	return tmp;
}


//�ő�1024�o�C�g�ǂݏo��
unsigned int eeprom_read( unsigned long add, unsigned int cnt, unsigned char *data )
{
	unsigned short intlp;
	unsigned int tmp = 0;
	
	if( cnt == 0 ){
		return 0;
	}
	
	if( cnt <= 1024 ){
		tmp = cnt;
	}
	else{
		tmp = 1024;
	}
	
	eeprom_init();

	eeprom_dout( EEP_CMD_READ );
	eeprom_dout( (add >> 16) & 0xff );
	eeprom_dout( (add >> 8 ) & 0xff );
	eeprom_dout( add & 0xff );
	
	for( intlp = 0; intlp < tmp; intlp++ ){
		data[intlp] = eeprom_din();
	}
	eeprom_end();

	return tmp;
}

//EEPROM�|�[�g������
void eeprom_init( void )
{
	EEPROM_CS = 1;
	EEPROM_SI = 0;
	EEPROM_SCK = 0;		//mode0
//	EEPROM_SCK = 1;		//mode3
	
	NOP();
	NOP();

	EEPROM_CS = 0;
	NOP();
	NOP();
//	EEPROM_SCK = 0;		//mode3
	
}

//EEPROM�|�[�g�I��
void eeprom_end( void )
{
	NOP();
	NOP();
	EEPROM_SCK = 0;	//mode0
//	EEPROM_SCK = 1;	//mode3

	NOP();
	NOP();
	EEPROM_CS = 1;
	EEPROM_SI = 0;
	
	wait_1ms(1);	//WAIT 1ms
}

//EEPROM��8�r�b�g�o��
void eeprom_dout( unsigned char data )
{
	unsigned short intlp;
	unsigned char tmp;
	
	tmp = data;
	
	for( intlp = 0; intlp < 8; intlp++ ){
		EEPROM_SI = (tmp & 0x80) == 0 ? 0 : 1 ; 	//MSB���o��
		tmp <<= 1;

		NOP();	//WAIT 0.06us 8MHz SCK
		NOP();
		EEPROM_SCK = 1;
		
		NOP();	//WAIT 0.06us 8MHz SCK
		NOP();
		EEPROM_SCK = 0;

	}
}

//EEPROM���8�r�b�g����
unsigned char eeprom_din( void )
{
	unsigned short intlp;
	unsigned char tmp;
	
	tmp = 0;
	
	for( intlp = 0; intlp < 8; intlp++ ){
		tmp <<= 1;
		tmp |= ( EEPROM_SO == 0 ) ? 0 : 1; 	//MSB������

		EEPROM_SCK = 1;
		NOP();	//WAIT 0.0625us 8MHz SCK
		NOP();

		EEPROM_SCK = 0;
		NOP();	//WAIT 0.0625us 8MHz SCK
		NOP();
	}
	return tmp;
}

void nop_wait( unsigned short cnt )
{
	unsigned short intlp;
	
	for( intlp = 0; intlp < cnt; intlp++ ){
		NOP();	//NOP���s����:0.03125us(32MHz)
	}
}

void wait_1ms( unsigned short cnt )
{
	unsigned short intlp;
	
	for( intlp = 0; intlp < cnt; intlp++){
		wait_05us(2000);
	}
}

void wait_05us( unsigned short cnt )
{
	unsigned short intlp;
	
	for( intlp = 0; intlp < cnt; intlp-- ){
		NOP();	//NOP���s����:0.03125us(32MHz)  NOP16��0.5us(500ns:2MHz)
		NOP();
		NOP();
		NOP();

		NOP();
		NOP();
		NOP();
		NOP();
		
		NOP();
		NOP();
		NOP();
		NOP();

		NOP();
		NOP();
		NOP();
		NOP();
	}
}
