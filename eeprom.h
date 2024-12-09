

//関数プロトタイプ宣言
extern unsigned char eeprom_din( void );
extern void eeprom_dout( unsigned char data );
extern void eeprom_cmd( unsigned char data );
extern unsigned char eeprom_readst( void );
extern void eeprom_init( void );
extern void eeprom_end( void );
extern unsigned int eeprom_write( unsigned long add, unsigned int cnt, unsigned char *data );
extern unsigned int eeprom_read( unsigned long add, unsigned int cnt, unsigned char *data );
extern void nop_wait( unsigned short cnt );
extern void wait_1ms( unsigned short cnt );
extern void wait_05us( unsigned short cnt );
