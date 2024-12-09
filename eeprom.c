#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
#include "r_cg_userdefine.h"

//define定義
//EEPROM コマンド宣言
#define EEP_CMD_READ 0x03	//読み出し
#define EEP_CMD_WRITE 0x02	//書き込み
#define EEP_CMD_WREN 0x06	//書き込み許可
#define EEP_CMD_WRDI 0x04	//書き込み禁止
#define EEP_CMD_RDSR 0x05	//ステータス読み出し

#define EEP_CMD_WRSR 0x01	//ステータス書き込み
#define EEP_CMD_PE 	 0x42	//ページイレース
#define EEP_CMD_SE	 0xd8	//セクターイレース
#define EEP_CMD_CE   0xc7	//チップイレース
#define EEP_CMD_RDID 0xab	//ID読み出し
#define EEP_CMD_DPD  0xb9	//POWER DOWN

//ポート設定　実際のポートに割り振り
#define FPGA_PROG_B		P5_bit.no0
#define FPGA_INIT_B		P0_bit.no1
#define FPGA_DONE		P1_bit.no0	//(P1 & 0x01)
#define FPGA_DIN		P1_bit.no2
#define FPGA_CCLK		P1_bit.no0

#if 0
#define	EEPROM_CS	P5_bit.no0		//P50 OUT チップセレクト
#define	EEPROM_SO	P12_bit.no4;	//P124 IN  データ入力
#define	EEPROM_SI	P14_bit.no7;	//P147 OUT データ出力
#define	EEPROM_SCK	P14_bit.no5;	//P145 OUT クロック
#else
unsigned short EEPROM_CS;	//デバッグ用
unsigned short EEPROM_SO;	//デバッグ用
unsigned short EEPROM_SI;	//デバッグ用
unsigned short EEPROM_SCK;	//デバッグ用
#endif

//関数プロトタイプ宣言
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

//グローバル変数
extern volatile uint16_t  g_csi00_tx_count;            /* csi00 send data count */
extern unsigned int g_Sendend;

//関数


#define FPGA_DATA_CNT 256		//FPGA転送バイト数
#define FPGA_DATA_OFF 0x67		//データオフセット

unsigned short fpga_config3(void)
{
	unsigned long lp1;
	unsigned long tmp = 0;
	unsigned long tmp1 = 0;
	unsigned char data[FPGA_DATA_CNT];
	
	FPGA_PROG_B = 1;			//PROG_B = H

	while( FPGA_INIT_B == 0 )	//INIT_BがHになるのを待つ
		;

	wait_1ms(1);				//1ms待ち

	//FPGAデータ読み出し、データ送信数を取得する
	eeprom_read( 0, FPGA_DATA_CNT, &data[0] );	//FPGAデータ読み出し
	tmp  = data[0x64 + 0] * 0x10000;	//送信データバイト数の取得
	tmp += data[0x64 + 1] * 0x100;
	tmp += data[0x64 + 2];
	
	if( tmp == 0xfffff ){	//EEPROM未書き込み？
		return 0;
	}
	
	FPGA_PROG_B = 0;			//PROG_B = L　FPGAコンフィグレーションクリア
	wait_05us(2);				//1us待ち（PROG_BのLパルス500ns以上）
	
	while( FPGA_INIT_B != 0 )	//INIT_BがLになるのを待つ
		;

	FPGA_PROG_B = 1;			//PROG_B = H　FPGA初期化開始
	
	while( FPGA_INIT_B == 0 )	//INIT_BがHになるのを待つ（FPGA初期化完了待ち）
		;

	wait_1ms(1);				//1ms待ち（500ns以上待つ）
//	for( lp = 0 ; lp < 5000; lp++){	//1ms待ち
//		NOP();
//	}

//	tmp += FPGA_DATA_OFF; 				//データではないヘッダの容量を加算
	
	lp1 = 0;
	g_Sendend = 0;

	while( tmp > 0 ){
		eeprom_read( lp1 * FPGA_DATA_CNT + FPGA_DATA_OFF, FPGA_DATA_CNT, &data[0] );	//FPGAデータ読み出し

		if( tmp > FPGA_DATA_CNT ){
			tmp1 = FPGA_DATA_CNT;
		}
		else{
			tmp1 = tmp;
		}

		g_Sendend = 1;
//		R_CSI11_Start();
//		R_CSI11_Send( &data[0], tmp1 );	//FPGAデータ送信開始
		
		while(g_Sendend != 0) 	//送信完了待ち
			;

		tmp = tmp - tmp1;
		lp1 = lp1 + 1;
		
	}

	while( FPGA_DONE != 0 )	//FPGAプログラム完了待ち
		;

	return 1;
}



//EEPROM 1バイトコマンド出力
void eeprom_cmd( unsigned char data )
{
	eeprom_init();
	eeprom_dout( data );
	eeprom_end();
}

//EEPROMステータス読み出し
unsigned char eeprom_rdsr( void )
{
	unsigned char tmp = 0;
	
	eeprom_init();
	eeprom_dout( EEP_CMD_RDSR );
	tmp = eeprom_din();
	eeprom_end();
	
	return tmp;
}

//最大256バイト書き込み
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
	
	//書き込み待ちはステータスを読み出す。
	
	return tmp;
}


//最大1024バイト読み出し
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

//EEPROMポート初期化
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

//EEPROMポート終了
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

//EEPROMに8ビット出力
void eeprom_dout( unsigned char data )
{
	unsigned short intlp;
	unsigned char tmp;
	
	tmp = data;
	
	for( intlp = 0; intlp < 8; intlp++ ){
		EEPROM_SI = (tmp & 0x80) == 0 ? 0 : 1 ; 	//MSBより出力
		tmp <<= 1;

		NOP();	//WAIT 0.06us 8MHz SCK
		NOP();
		EEPROM_SCK = 1;
		
		NOP();	//WAIT 0.06us 8MHz SCK
		NOP();
		EEPROM_SCK = 0;

	}
}

//EEPROMより8ビット入力
unsigned char eeprom_din( void )
{
	unsigned short intlp;
	unsigned char tmp;
	
	tmp = 0;
	
	for( intlp = 0; intlp < 8; intlp++ ){
		tmp <<= 1;
		tmp |= ( EEPROM_SO == 0 ) ? 0 : 1; 	//MSBより入力

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
		NOP();	//NOP実行時間:0.03125us(32MHz)
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
		NOP();	//NOP実行時間:0.03125us(32MHz)  NOP16個で0.5us(500ns:2MHz)
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
