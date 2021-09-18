#include <xc.h>
#include "config.h"
#include "i2c_lcd.h"

#define _XTAL_FREQ 16000000

void main(void){
	master_init();
	lcd_init(0x4E);
	set_cursor(1,1);
	lcd_string("  Hello world");
	set_cursor(2,1);
	lcd_string("  Ahmed Kamal");
	__delay_ms(2500);
	while(1){
       lcd_SR();
       __delay_ms(350); 
       lcd_SR();
       __delay_ms(350); 
       lcd_SL();
       __delay_ms(350);
       lcd_SL();
       __delay_ms(350);
	}
	return;
}