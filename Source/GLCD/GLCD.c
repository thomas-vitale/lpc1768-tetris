

/****************************************Copyright (c)**************************************************
**
** http://www.powermcu.com
**
**--------------File Info-------------------------------------------------------------------------------
** File name:           GLCD.c
** Descriptions:        Has been tested SSD1289, ILI9320, R61505U, SSD1298, ST7781, SPFD5408B, ILI9325, ILI9328,
** HX8346A, HX8347A
**------------------------------------------------------------------------------------------------------
** Created by:          AVRman
** Created date:        2012-3-10
** Version:             1.3
** Descriptions:        The original version
**
**------------------------------------------------------------------------------------------------------
** Modified by:         Paolo Bernardi
** Modified date:       03/01/2020
** Version:             2.0
** Descriptions:        simple arrangement for screen usage
********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "GLCD.h"
#include "AsciiLib.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t LCD_Code;

/* Private define ------------------------------------------------------------*/
#define  ILI9320    0  /* 0x9320 */
#define  ILI9325    1  /* 0x9325 */
#define  ILI9328    2  /* 0x9328 */
#define  ILI9331    3  /* 0x9331 */
#define  SSD1298    4  /* 0x8999 */
#define  SSD1289    5  /* 0x8989 */
#define  ST7781     6  /* 0x7783 */
#define  LGDP4531   7  /* 0x4531 */
#define  SPFD5408B  8  /* 0x5408 */
#define  R61505U    9  /* 0x1505 0x0505 */
#define  HX8346A    10 /* 0x0046 */
#define  HX8347D    11 /* 0x0047 */
#define  HX8347A    12 /* 0x0047 */
#define  LGDP4535   13 /* 0x4535 */
#define  SSD2119    14 /* 3.5 LCD 0x9919 */

/*******************************************************************************
* Function Name  : LCD_Configuration
* Description    : Configures LCD Control lines
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static void LCD_Configuration(void)
{
    /* Configure the LCD Control pins */
    
    /* EN = P0.19 , LE = P0.20 , DIR = P0.21 , CS = P0.22 , RS = P0.23 */
    /* WR = P0.24 , RD = P0.25 , DB[0.7] = P2.0...P2.7 , DB[8.15]= P2.0...P2.7 */
    LPC_GPIO0->FIODIR   |= 0x03f80000;
    LPC_GPIO0->FIOSET    = 0x03f80000;
}

/*******************************************************************************
* Function Name  : LCD_Send
* Description    : Writes data byte to LCD
* Input          : - byte: byte to be sent
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_Send (uint16_t byte)
{
    LPC_GPIO2->FIODIR |= 0xFF;          /* P2.0...P2.7 Output */
    LCD_DIR(1)                          /* Interface A->B */
    LCD_EN(0)                           /* Enable 2A->2B */
    LPC_GPIO2->FIOPIN =  byte;          /* Write D0..D7 */
    LCD_LE(1)
    LCD_LE(0)                           /* latch D0..D7 */
    LPC_GPIO2->FIOPIN =  byte >> 8;     /* Write D8..D15 */
}

/*******************************************************************************
* Function Name  : wait_delay
* Description    : Delay Time
* Input          : - nCount: Delay Time
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static void wait_delay(int count)
{
    while(count--);
}

/*******************************************************************************
* Function Name  : LCD_Read
* Description    : Reads data from LCD
* Input          : None
* Output         : None
* Return         : Data read from LCD
* Attention      : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_Read (void)
{
    uint16_t value;
    
    LPC_GPIO2->FIODIR &= ~(0xFF);              /* P2.0...P2.7 Input */
    LCD_DIR(0);                                /* Interface B->A */
    LCD_EN(0);                                 /* Enable 2B->2A */
    wait_delay(30);                            /* delay some times */
    value = LPC_GPIO2->FIOPIN0;                /* Read D8..D15 */
    LCD_EN(1);                                 /* Enable 1B->1A */
    wait_delay(30);                            /* delay some times */
    value = (value << 8) | LPC_GPIO2->FIOPIN0; /* Read D0..D7 */
    LCD_DIR(1);
    return  value;
}

/*******************************************************************************
* Function Name  : LCD_WriteIndex
* Description    : Writes to LCD Index Register
* Input          : - index: Register Address
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteIndex(uint16_t index)
{
    LCD_CS(0);
    LCD_RS(0);
    LCD_RD(1);
    LCD_Send( index );
    wait_delay(22);
    LCD_WR(0);
    wait_delay(1);
    LCD_WR(1);
    LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_WriteData
* Description    : Writes data to LCD Register
* Input          : - data: Data to be written
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteData(uint16_t data)
{
    LCD_CS(0);
    LCD_RS(1);
    LCD_Send( data );
    LCD_WR(0);
    wait_delay(1);
    LCD_WR(1);
    LCD_CS(1);
}

/*******************************************************************************
* Function Name  : LCD_ReadData
* Description    : Reads data from LCD Controller
* Input          : None
* Output         : None
* Return         : Data read
* Attention      : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadData(void)
{
    uint16_t value;
    
    LCD_CS(0);
    LCD_RS(1);
    LCD_WR(1);
    LCD_RD(0);
    value = LCD_Read();
    
    LCD_RD(1);
    LCD_CS(1);
    
    return value;
}

/*******************************************************************************
* Function Name  : LCD_WriteReg
* Description    : Writes to the selected LCD register.
* Input          : - LCD_Reg: address of the selected register.
* - LCD_RegValue: value to write to the selected register.
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static __attribute__((always_inline)) void LCD_WriteReg(uint16_t LCD_Reg,uint16_t LCD_RegValue)
{
    /* Write 16-bit Index, then Write Reg */
    LCD_WriteIndex(LCD_Reg);
    /* Write 16-bit Reg */
    LCD_WriteData(LCD_RegValue);
}

/*******************************************************************************
* Function Name  : LCD_ReadReg
* Description    : Reads the selected LCD Register.
* Input          : - LCD_Reg: address of the selected register.
* Output         : None
* Return         : LCD Register Value.
* Attention      : None
*******************************************************************************/
static __attribute__((always_inline)) uint16_t LCD_ReadReg(uint16_t LCD_Reg)
{
    uint16_t LCD_RAM;
    
    /* Write 16-bit Index (then Read Reg) */
    LCD_WriteIndex(LCD_Reg);
    /* Read 16-bit Reg */
    LCD_RAM = LCD_ReadData();
    return LCD_RAM;
}

/*******************************************************************************
* Function Name  : LCD_SetCursor
* Description    : Sets the cursor position.
* Input          : - Xpos: specifies the X position.
* - Ypos: specifies the Y position.
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static void LCD_SetCursor(uint16_t Xpos,uint16_t Ypos)
{
    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
    
    uint16_t temp = Xpos;

             Xpos = Ypos;
             Ypos = ( MAX_X - 1 ) - temp;

    #elif  ( DISP_ORIENTATION == 0 ) || ( DISP_ORIENTATION == 180 )
        
    #endif

  switch( LCD_Code )
  {
     default:        /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x7783 0x4531 0x4535 */
          LCD_WriteReg(0x0020, Xpos );
          LCD_WriteReg(0x0021, Ypos );
          break;

     case SSD1298:   /* 0x8999 */
     case SSD1289:   /* 0x8989 */
          LCD_WriteReg(0x004e, Xpos );
          LCD_WriteReg(0x004f, Ypos );
          break;

     case HX8346A:   /* 0x0046 */
     case HX8347A:   /* 0x0047 */
     case HX8347D:   /* 0x0047 */
          LCD_WriteReg(0x02, Xpos>>8 );
          LCD_WriteReg(0x03, Xpos );

          LCD_WriteReg(0x06, Ypos>>8 );
          LCD_WriteReg(0x07, Ypos );
    
          break;
     case SSD2119:   /* 3.5 LCD 0x9919 */
          break;
  }
}

/*******************************************************************************
* Function Name  : delay_ms
* Description    : Delay Time in milliseconds
* Input          : - ms: Delay Time
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
static void delay_ms(uint16_t ms)
{
    uint16_t i,j;
    for( i = 0; i < ms; i++ )
    {
        for( j = 0; j < 1141; j++ );
    }
}


/*******************************************************************************
* Function Name  : LCD_Initialization
* Description    : Initialize TFT Controller.
* Input          : None
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void LCD_Initialization(void)
{
    uint16_t DeviceCode;
    
    LCD_Configuration();
    delay_ms(100);
    DeviceCode = LCD_ReadReg(0x0000);       /* Read LCD ID */
    
    if( DeviceCode == 0x9325 || DeviceCode == 0x9328 )
    {
        LCD_Code = ILI9325;
        LCD_WriteReg(0x00e7,0x0010);
        LCD_WriteReg(0x0000,0x0001);    /* start internal osc */
        LCD_WriteReg(0x0001,0x0100);
        LCD_WriteReg(0x0002,0x0700);    /* power on sequence */
        LCD_WriteReg(0x0003,(1<<12)|(1<<5)|(1<<4)|(0<<3) );     /* importance */
        LCD_WriteReg(0x0004,0x0000);
        LCD_WriteReg(0x0008,0x0207);
        LCD_WriteReg(0x0009,0x0000);
        LCD_WriteReg(0x000a,0x0000);    /* display setting */
        LCD_WriteReg(0x000c,0x0001);    /* display setting */
        LCD_WriteReg(0x000d,0x0000);
        LCD_WriteReg(0x000f,0x0000);
        /* Power On sequence */
        LCD_WriteReg(0x0010,0x0000);
        LCD_WriteReg(0x0011,0x0007);
        LCD_WriteReg(0x0012,0x0000);
        LCD_WriteReg(0x0013,0x0000);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0010,0x1590);
        LCD_WriteReg(0x0011,0x0227);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0012,0x009c);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0013,0x1900);
        LCD_WriteReg(0x0029,0x0023);
        LCD_WriteReg(0x002b,0x000e);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0020,0x0000);
        LCD_WriteReg(0x0021,0x0000);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0030,0x0007);
        LCD_WriteReg(0x0031,0x0707);
        LCD_WriteReg(0x0032,0x0006);
        LCD_WriteReg(0x0035,0x0704);
        LCD_WriteReg(0x0036,0x1f04);
        LCD_WriteReg(0x0037,0x0004);
        LCD_WriteReg(0x0038,0x0000);
        LCD_WriteReg(0x0039,0x0706);
        LCD_WriteReg(0x003c,0x0701);
        LCD_WriteReg(0x003d,0x000f);
        delay_ms(50);  /* delay 50 ms */
        LCD_WriteReg(0x0050,0x0000);
        LCD_WriteReg(0x0051,0x00ef);
        LCD_WriteReg(0x0052,0x0000);
        LCD_WriteReg(0x0053,0x013f);
        LCD_WriteReg(0x0060,0xa700);
        LCD_WriteReg(0x0061,0x0001);
        LCD_WriteReg(0x006a,0x0000);
        LCD_WriteReg(0x0080,0x0000);
        LCD_WriteReg(0x0081,0x0000);
        LCD_WriteReg(0x0082,0x0000);
        LCD_WriteReg(0x0083,0x0000);
        LCD_WriteReg(0x0084,0x0000);
        LCD_WriteReg(0x0085,0x0000);
        
        LCD_WriteReg(0x0090,0x0010);
        LCD_WriteReg(0x0092,0x0000);
        LCD_WriteReg(0x0093,0x0003);
        LCD_WriteReg(0x0095,0x0110);
        LCD_WriteReg(0x0097,0x0000);
        LCD_WriteReg(0x0098,0x0000);
        /* display on sequence */
        LCD_WriteReg(0x0007,0x0133);
        
        LCD_WriteReg(0x0020,0x0000);  /* Row Start 0 */
        LCD_WriteReg(0x0021,0x0000);  /* Col Start 0 */
    }

    delay_ms(50);   /* delay 50 ms */
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : Fill the screen with specified color
* Input          : - Color: Screen Color
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void LCD_Clear(uint16_t Color)
{
    uint32_t index;
    
    if( LCD_Code == HX8347D || LCD_Code == HX8347A )
    {
        LCD_WriteReg(0x02,0x00);
        LCD_WriteReg(0x03,0x00);
        
        LCD_WriteReg(0x04,0x00);
        LCD_WriteReg(0x05,0xEF);
        
        LCD_WriteReg(0x06,0x00);
        LCD_WriteReg(0x07,0x00);
        
        LCD_WriteReg(0x08,0x01);
        LCD_WriteReg(0x09,0x3F);
    }
    else
    {
        LCD_SetCursor(0,0);
    }

    LCD_WriteIndex(0x0022);
    for( index = 0; index < MAX_X * MAX_Y; index++ )
    {
        LCD_WriteData(Color);
    }
}

/******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : Convert RRRRRGGGGGGBBBBB to BBBBBGGGGGGRRRRR
* Input          : - color: BRG Color Value
* Output         : None
* Return         : RGB Color Value
* Attention      : Used internally
*******************************************************************************/
static uint16_t LCD_BGR2RGB(uint16_t color)
{
    uint16_t  r, g, b, rgb;
    
    b = ( color>>0 )  & 0x1f;
    g = ( color>>5 )  & 0x3f;
    r = ( color>>11 ) & 0x1f;
    
    rgb =  (b<<11) + (g<<5) + (r<<0);
    
    return( rgb );
}

/******************************************************************************
* Function Name  : LCD_GetPoint
* Description    : Get color at specified coordinate
* Input          : - Xpos: Row Coordinate
* - Ypos: Line Coordinate
* Output         : None
* Return         : Screen Color
* Attention      : None
*******************************************************************************/
uint16_t LCD_GetPoint(uint16_t Xpos,uint16_t Ypos)
{
    uint16_t dummy;
    
    LCD_SetCursor(Xpos,Ypos);
    LCD_WriteIndex(0x0022);
    
    switch( LCD_Code )
    {
        case ST7781:
        case LGDP4531:
        case LGDP4535:
        case SSD1289:
        case SSD1298:
             dummy = LCD_ReadData();   /* Empty read */
             dummy = LCD_ReadData();
             return  dummy;
        case HX8347A:
        case HX8347D:
             {
                uint8_t red,green,blue;
                
                dummy = LCD_ReadData();   /* Empty read */

                red = LCD_ReadData() >> 3;
                green = LCD_ReadData() >> 2;
                blue = LCD_ReadData() >> 3;
                dummy = (uint16_t) ( ( red<<11 ) | ( green << 5 ) | blue );
             }
             return  dummy;

        default:    /* 0x9320 0x9325 0x9328 0x9331 0x5408 0x1505 0x0505 0x9919 */
             dummy = LCD_ReadData();   /* Empty read */
             dummy = LCD_ReadData();
             return  LCD_BGR2RGB( dummy );
    }
}

/******************************************************************************
* Function Name  : LCD_SetPoint
* Description    : Draw point at specified coordinate
* Input          : - Xpos: Row Coordinate
* - Ypos: Line Coordinate
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void LCD_SetPoint(uint16_t Xpos,uint16_t Ypos,uint16_t point)
{
    if( Xpos >= MAX_X || Ypos >= MAX_Y )
    {
        return;
    }
    LCD_SetCursor(Xpos,Ypos);
    LCD_WriteReg(0x0022,point);
}

/******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : Bresenham's line algorithm
* Input          : - x1: Start X
* - y1: Start Y
* - x2: End X
* - y2: End Y
* - color: Line Color
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/


void LCD_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    short dx, dy;
    short temp;

    if (x0 > x1)
    {
        temp = x1;
        x1 = x0;
        x0 = temp;
    }
    if (y0 > y1)
    {
        temp = y1;
        y1 = y0;
        y0 = temp;
    }

    dx = x1 - x0;
    dy = y1 - y0;

    if (dx == 0)
    {
        do
        {
            LCD_SetPoint(x0, y0, color);
            y0++;
        } while (y1 >= y0);
        return;
    }
    if (dy == 0)
    {
        do
        {
            LCD_SetPoint(x0, y0, color);
            x0++;
        } while (x1 >= x0); 
        return;
    }

    if (dx > dy)
    {
        temp = 2 * dy - dx;
        while (x0 != x1)
        {
            LCD_SetPoint(x0, y0, color);
            x0++;
            if (temp > 0)
            {
                y0++;
                temp += 2 * dy - 2 * dx;
            }
            else
            {
                temp += 2 * dy;
            }
        }
        LCD_SetPoint(x0, y0, color);
    }
    else
    {
        temp = 2 * dx - dy;
        while (y0 != y1)
        {
            LCD_SetPoint(x0, y0, color);
            y0++;
            if (temp > 0)
            {
                x0++;
                temp += 2 * dy - 2 * dx;
            }
            else
            {
                temp += 2 * dy;
            }
        }
        LCD_SetPoint(x0, y0, color);
    }
}
/******************************************************************************
* Function Name  : LCD_DrawRectangle
* Description    : Disegna un rettangolo (pieno o vuoto) 
* Input          : - x1, y1: Angolo 1
* - x2, y2: Angolo opposto
* - color: Colore
* - fill: 1 = pieno, 0 = solo bordi
* - thickness: spessore (se fill=0)
* Output         : None
*******************************************************************************/
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, uint8_t fill, uint8_t thickness)
{
    uint16_t temp;
    int i, j; 
    if (x1 > x2) { temp = x1; x1 = x2; x2 = temp; }
    if (y1 > y2) { temp = y1; y1 = y2; y2 = temp; }

    if (x2 >= MAX_X) x2 = MAX_X - 1;
    if (y2 >= MAX_Y) y2 = MAX_Y - 1;

    if (fill)
    {

        for (i = y1; i <= y2; i++) 
        {
            LCD_SetCursor(x1, i); 
            
            LCD_WriteIndex(0x0022); 
            
            for (j = x1; j <= x2; j++) 
            {
                LCD_WriteData(color);
            }
        }
    }
        else
    {
        for (i = 0; i < thickness; i++)
        {
            if ((x1 + i > x2 - i) || (y1 + i > y2 - i)) break;
            
            LCD_DrawLine(x1, y1 + i, x2, y1 + i, color);     // Top
            LCD_DrawLine(x1, y2 - i, x2, y2 - i, color);     // Bottom
            LCD_DrawLine(x1 + i, y1, x1 + i, y2, color);     // Left
            LCD_DrawLine(x2 - i, y1, x2 - i, y2, color);     // Right
        }
    }
}
/******************************************************************************
* Function Name  : PutChar
* Description    : Put a character on LCD
* Input          : - Xpos: X Coordinate
* - Ypos: Y Coordinate
* - ASCI: Character code
* - charColor: Character color
* - bkColor: Background color
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void PutChar( uint16_t Xpos, uint16_t Ypos, uint8_t ASCI, uint16_t charColor, uint16_t bkColor )
{
    uint16_t i, j;
    uint8_t buffer[16], tmp_char;
    GetASCIICode(buffer,ASCI);  /* Get Font Data */
    for( i=0; i<16; i++ )
    {
        tmp_char = buffer[i];
        for( j=0; j<8; j++ )
        {
            if( ((tmp_char >> (7 - j)) & 0x01) == 0x01 )
            {
                LCD_SetPoint( Xpos + j, Ypos + i, charColor );  /* Character Color */
            }
            else
            {
                LCD_SetPoint( Xpos + j, Ypos + i, bkColor );  /* Background Color */
            }
        }
    }
}

/******************************************************************************
* Function Name  : GUI_Text
* Description    : Display String at coordinates
* Input          : - Xpos: X Coordinate
* - Ypos: Y Coordinate
* - str: String
* - charColor: Character Color
* - bkColor: Background Color
* Output         : None
* Return         : None
* Attention      : None
*******************************************************************************/
void GUI_Text(uint16_t Xpos, uint16_t Ypos, uint8_t *str,uint16_t Color, uint16_t bkColor)
{
    uint8_t TempChar;
    do
    {
        TempChar = *str++;
        PutChar( Xpos, Ypos, TempChar, Color, bkColor );
        if( Xpos < MAX_X - 8 )
        {
            Xpos += 8;
        }
        else if ( Ypos < MAX_Y - 16 )
        {
            Xpos = 0;
            Ypos += 16;
        }
        else
        {
            Xpos = 0;
            Ypos = 0;
        }
    }
    while ( *str != 0 );
}


/*******************************************************************************
* Function Name  : LCD_SetWindow
* Description    : Sets a rectangular active window for burst writing
* Input          : - x1, y1: Start coordinates
* - x2, y2: End coordinates
* Output         : None
* Return         : None
*******************************************************************************/
static void LCD_SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    /* Gestione Orientamento (preso da LCD_SetCursor) */
    #if  ( DISP_ORIENTATION == 90 ) || ( DISP_ORIENTATION == 270 )
        uint16_t temp;
        // Swap e rotazione coordinate per Landscape
        temp = x1; x1 = y1; y1 = (MAX_X - 1) - temp;
        temp = x2; x2 = y2; y2 = (MAX_X - 1) - temp;
        
        // Riordina se necessario dopo la rotazione
        if(x1 > x2) { temp = x1; x1 = x2; x2 = temp; }
        if(y1 > y2) { temp = y1; y1 = y2; y2 = temp; }
    #endif

    switch( LCD_Code )
    {
        case HX8347D:
        case HX8347A:
            /* Codice specifico per HX8347 (basato su LCD_Clear) */
            LCD_WriteReg(0x02, x1 >> 8);    // Column Start High
            LCD_WriteReg(0x03, x1 & 0xFF);  // Column Start Low
            LCD_WriteReg(0x04, x2 >> 8);    // Column End High
            LCD_WriteReg(0x05, x2 & 0xFF);  // Column End Low
            
            LCD_WriteReg(0x06, y1 >> 8);    // Row Start High
            LCD_WriteReg(0x07, y1 & 0xFF);  // Row Start Low
            LCD_WriteReg(0x08, y2 >> 8);    // Row End High
            LCD_WriteReg(0x09, y2 & 0xFF);  // Row End Low
            break;

        case SSD1289:
             /* SSD1289 ha registri diversi per le finestre (0x44, 0x45, 0x46) */
             LCD_WriteReg(0x0044, (x2 << 8) | x1); // Horizontal RAM address position
             LCD_WriteReg(0x0045, y1);             // Vertical RAM address start
             LCD_WriteReg(0x0046, y2);             // Vertical RAM address end
             LCD_SetCursor(x1, y1);                // Posiziona cursore all'inizio
             break;

        default: 
            /* ILI9320, ILI9325, ILI9328, ecc. */
            LCD_WriteReg(0x0050, x1); /* Horizontal GRAM Start Address */
            LCD_WriteReg(0x0051, x2); /* Horizontal GRAM End Address */
            LCD_WriteReg(0x0052, y1); /* Vertical GRAM Start Address */
            LCD_WriteReg(0x0053, y2); /* Vertical GRAM End Address */
            LCD_SetCursor(x1, y1);    /* Posiziona il cursore all'inizio della finestra */
            break;
    }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/


