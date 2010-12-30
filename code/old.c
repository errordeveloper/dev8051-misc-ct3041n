/** @file main.c XX XX XX
 *
*/

#include <8051.h>

/* LCD driver is connected to P0 */
#define LCD_RS (P0^1)
#define LCD_RW (P0^2)
#define LCD_EN (P0^3)
#define D0 (P0^4)
#define D1 (P0^5)
#define D2 (P0^6)
#define D3 (P0^7)

/* LCD driver commands */

#define CLEAR			0x01
#define HOME			0x02
#define ENTRY_MODE		0x04
#define SET_DISPLAY		0x08
#define DISPLAY_OFF		0x00
#define DISPLAY_ON		0x04
#define SET_CURSOR		0x10
#define CURSOR_OFF		0x00
#define CURSOR_ON		0x02

#define FUNCTION		0x20
#define DECREMENT		0x00
#define INCREMENT		0x02

#define SHIFT_OFF		0x00
#define SHIFT_ON		0x01
#define BLINK_OFF		0x00
#define BLINK_ON		0x01

#define LCD_CONF_4_BITS		0x00
#define LCD_CONF_8_BITS		0x10
#define LCD_CONF_1_LINE		0x00
#define LCD_CONF_2_LINE		0x08
#define LCD_CONF_FONT_5x7	0x00
#define LCD_CONF_FONT_5x10	0x04


/* Program configuration */
#define INFINITE_LOOP 1

/** == PROTOTYPES == **/

/** == MACROS == **/

#define LOWER(x) (x & 0x0F)
#define UPPER(x) ((x & 0xF0) >> 4)

/** == IMPLEMENTATION == **/


/*------------------------------------------------------------*/
/** @brief This function sends a control or data byte to the LCD.
 *
 *  @note Assuming a 4-bit Interface.
 *
 *  @param v Control or Data byte
 *  @param m if(0) -> 'control_mode'; if(1) -> 'data_mode';
*/
void lcd_write(unsigned char v, bool m)
{

//  unsigned char DL, DH;


//  DL = D & 0x0F; /* Get the Low Nibble */
//  DH = (D & 0xF0) >> 4; /* Get the High Nibble */
  LCD_RS = m;
  LCD_RW = 0;
  LCD_EN = 1; /* Enable LCD */

//  LCD_Send(DH); /* Send the High Nibble first */
  lcd_write(UPPER(v));

  /* Now Pulse the LCD Enable Pin */
  LCD_EN = 0;
  short_delay(); /* Short Delay */
  LCD_EN = 1;

//  LCD_Send(DL); /* Send the Low Nibble Next */ /* Now Pulse the LCD Enable Pin again */
  lcd_write(LOWER(v));

  /* Now Pulse the LCD Enable Pin */
  LCD_EN = 0;
  short_delay(); /* Short Delay */
  LCD_EN = 1;
}

#if DOXYGEN
/** @brief This macro calls function @ref lcd_write() in control mode.
 *  @param v Control byte
*/
void lcd_wconf(uchar v){};
/** @brief This macro calls function @ref lcd_write() in data mode.
 *  @param v Data byte
*/
void lcd_wdata(uchar v){};
#else
#define lcd_wconf(v) lcd_write(v, 0)
#define lcd_wdata(v) lcd_write(v, 1)
#endif

#define lcd_home() lcd_wconf(HOME)
#define lcd_clear() lcd_wconf(CLEAR)

#define lcd_cursor_on() lcd_wconf(CURSOR_ON)

/*------------------------------------------------------------*/
/** @brief A very short delay for the LCD driver.
 *
*/
void short_delay(void)
{

  unsigned char j,i;

  for(i=0; i<64; i++)
    for(j=0; j<255; j++);
}

/*------------------------------------------------------------*/
/** @brief This function initialises the LCD driver
 *
*/
void lcd_init(void)
{

  lcd_ctlw( FUNCTION + LCD_CONF_4_BITS + LCD_CONF_2_LINE );
    lcd_ctlw( FUNCTION + LCD_CONF_4_BITS + LCD_CONF_2_LINE );
      lcd_ctlw( FUNCTION + LCD_CONF_4_BITS + LCD_CONF_2_LINE );
        lcd_ctlw( FUNCTION + LCD_CONF_4_BITS + LCD_CONF_2_LINE );

  lcd_ctlw( SET_DISPLAY + DISPLAY_ON + CURSOR_ON + BLINK_ON );

  cursor_on();

  lcd_ctlw( ENTRY_MODE + INCREMENT + SHIFT_OFF );
}

/*------------------------------------------------------------*/
/** \brief 
 *
*/
void main(void)
{

  while (INFINITE_LOOP) {
  };

}