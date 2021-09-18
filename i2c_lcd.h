#include <xc.h>
#include <stdint.h>
#define _XTAL_FREQ 16000000

void master_init();
void master_wait();
void master_start();
void master_stop();
void master_restart();
void Ack();
void Nack();
unsigned char master_write(unsigned char data);
unsigned char I2C_read();

void IO_Expander_Write(unsigned char data);
void write4bits(unsigned char nible);
void lcd_cmd(unsigned char cmd);
void lcd_write(unsigned char data);
void lcd_init(unsigned char I2C_add);
void lcd_write(unsigned char data);
void lcd_string(char* str);
void lcd_clear();
void lcd_SR();
void lcd_SL();
void set_cursor(unsigned char, unsigned char);

#define Baud_rate 100000
#define Backlight 0x08 
#define LCD_TYPE  2

unsigned char RS, Backstate, i2c_add = Backlight;

// ======================================
// =========== I2C_Functions ============
void master_wait(void){
	while(SSPSTAT & 0x04 || SSPCON2 & 0x1F);
}

void master_start(void){
	master_wait();
	SEN = 1;
}

void master_stop(void){
	master_wait();
	PEN = 1;
}

void master_restart(void){
	master_wait();
	RSEN = 1;
}

void Ack(void){
	ACKDT = 0;
	master_wait();	
	ACKEN = 1;
} 

void Nack(void){
	ACKDT = 1;
	master_wait();
	ACKEN = 1;
} 

void master_init(void){
	SSPCON = 0x28;
	SSPCON2 = 0x00;
	SSPSTAT = 0x00;
	SSPADD = ((_XTAL_FREQ/4)/Baud_rate) - 1;
	TRISC3 = 1;
	TRISC4 = 1;
}

unsigned char master_write(unsigned char data){
	master_wait();
	SSPBUF = data;
	while(!SSPIF);
	SSPIF = 0;
	return ACKSTAT;
}

unsigned char I2C_read(void){
	master_wait();
	RCEN = 1;
	while(!SSPIF);
	SSPIF = 0;
	return SSPBUF;
}

//=========================================
//============= LCD Functions =============

void lcd_init(unsigned char I2C_add){
	i2c_add = I2C_add;
	IO_Expander_Write(0x00);
	__delay_ms(30);
	lcd_cmd(0x03);
	__delay_ms(5);
	lcd_cmd(0x03);
	__delay_ms(5);
	lcd_cmd(0x03);
	__delay_ms(5);
	lcd_cmd(0x02);
	__delay_ms(5);
	lcd_cmd(0x20 | (LCD_TYPE << 2));//lcd_cmd(0x28); // Merage two steps togather (0x02) and (0x08) the first is 0x02 and the second (0x08) 
	__delay_ms(50);
	lcd_cmd(0x0C);  // Wrong in datasheet 
	__delay_ms(50);
	lcd_cmd(0x00);
	__delay_ms(50);
	lcd_cmd(0x06);
	__delay_ms(50);
}

void IO_Expander_Write(unsigned char data){
	master_start();
	master_write(i2c_add);
	master_write(data | Backstate);
	master_stop();
}

void write4bits(unsigned char nible){
	nible |= RS;
	IO_Expander_Write(nible | 0x04);  // To make sure that EN = 1 without changing RS and R/W  
	IO_Expander_Write(nible << 4 & 0xFB); // (F) To make sure that not change (B) don't know
	__delay_us(50);
}

void lcd_cmd(unsigned char cmd){
	RS = 0;
	write4bits(cmd & 0xF0);
	write4bits((cmd << 4) & 0xF0);
}

void lcd_write(unsigned char data){
	RS = 1;
	write4bits(data & 0xF0);
	write4bits((data << 4) & 0xF0);
}

void lcd_string(char* str){
	for(int i = 0; str != '\0' ; i++)
		lcd_write(str[i]);
}

void lcd_clear(void){
	lcd_cmd(0x01);
	__delay_us(40);
}

void lcd_SR(void){
	lcd_cmd(0x1C);
	__delay_us(40);
}

void lcd_SL(void){
	lcd_cmd(0x18);
	__delay_us(40);
}

void set_cursor(unsigned char r, unsigned char c){
	switch(r){
		case 2:
			lcd_cmd(0xC0 + c - 1);
			break;
		case 3:
			lcd_cmd(0x94 + c - 1);
			break;
		case 4:
			lcd_cmd(0xD4 + c - 1);
			break;
		default:
			lcd_cmd(0x80 + c - 1);
	}
}
