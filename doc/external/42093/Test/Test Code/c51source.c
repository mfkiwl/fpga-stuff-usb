
#include <intrins.h>
#include <reg51.h>
#include <absacc.h>
//#define	DataBusH	P2
#define	DataBusH	P1
#define	DataBusL	P0

#define CHX       0xD0
#define CHY       0x90
#define general_delay 5
#define ZERO_FIELD_COUNT 4


#define TP_DCLK(a) T_DCLK=a
#define TP_CS(a) T_CS=a
#define TP_DIN(a) T_DIN=a
#define TP_DOUT T_DOUT
#define TP_IRQ T_IRQ


#define pp XBYTE[0x0000]
#define pic_select P1

//for lan se di ban.......
sbit	CSB	=	P3^4;
sbit	RESETB	=	P3^0;
sbit	RS	=	P3^5;
sbit	WRB	=	P3^6;
sbit	RDB	=	P3^7;

sbit	KEY =	P2^7;


sbit  T_CS      = P3^3;  //touch sreen select  1.5
//sbit  T_BUSY    = P3^7;  //touch sreen busy   1.0
sbit  T_DCLK    = P3^2;  //SPI CLOCK   1.4
sbit  T_DIN     = P3^1;  //SPI DATA IN
sbit  T_DOUT    = P2^1;  //SPI DATA OUT   1.3
sbit  T_IRQ     = P2^0;  //IRQ

/*

//for test jia
sbit	CSB	=	P3^6;
sbit	RESETB	=	P3^7;
sbit	RS	=	P3^5;
sbit	WRB	=	P3^4;
sbit	RDB	=	P3^0;

sbit	KEY =	P2^7;


sbit  T_CS      = P3^3;  //touch sreen select  1.5
//sbit  T_BUSY    = P3^7;  //touch sreen busy   1.0
sbit  T_DCLK    = P2^1;  //SPI CLOCK   1.4
sbit  T_DIN     = P2^2;  //SPI DATA IN
sbit  T_DOUT    = P2^4;  //SPI DATA OUT   1.3
sbit  T_IRQ     = P2^3;  //IRQ
/*
 //*****************8xiao zou test
sbit	CSB	=	P3^4;								
sbit	RESETB	=	P3^5;
sbit	RS	=	P3^3;
sbit	WRB	=	P3^6;
sbit	RDB	=	P3^7;

sbit	KEY =	P3^1;

sbit  T_CS      = P1^0;  //touch sreen select  1.5
//sbit  T_BUSY    = P3^7;  //touch sreen busy   1.0
sbit  T_DCLK    = P1^1;  //SPI CLOCK   1.4
sbit  T_DIN     = P1^3;  //SPI DATA IN
sbit  T_DOUT    = P3^2;  //SPI DATA OUT   1.3
sbit  T_IRQ     = P2^2;  //IRQ
*/

void LCD_INIT_HX8352A(void);
void LCD_TEST_SingleColor(char colorH,char colorL);
void LCD_TEST_Picture1(unsigned char *picture1);
void LCD_TEST_Picture2(unsigned char *picture1);
void LCD_TEST_Picture3(unsigned char *picture1);
void LCD_TEST_Picture4(unsigned char *picture1);
void LCD_TEST_Picture5(unsigned char *picture1);
void LCD_TEST_id(unsigned char *id);
void adderset(int xs,int xe,int ys,int ye);
void LCD_id_SHOW(int a,int b,int c,int d);
void LCD_show_window();

void LCD_WRITE_CMD(char CMD1,char CMD2);
void LCD_WRITE_DATA(char DATAA,char DATAB);
void LCD_READ_ID(void);
void LLCD_WRITE_CMD(int cmd);
void LLCD_WRITE_DATA(int dataa);
void send_host_reg_command(int cmd,int dataa);

void comm_out(unsigned char com2);
void data_out(unsigned char dat2);
void Write_regCOM(unsigned char com1,unsigned char com2);
void Write_DAT(unsigned char dat1,unsigned char dat2);
void Write_reg(unsigned char comx,unsigned char datex);

void HD_reset(void);
void Keypress(void);
void display_eprom(unsigned char p);

void delayms(int value);
void delayus(int value1);

void LCD_Color_with_ADD(char data1,char data2,int x1,int x2,int y1,int y2);
void ADS7843_disp();
void benchmark(void);

void TOUCH_RESET(void);
void TOUCH_WRITE(unsigned char command);
void TOUCH_READ(unsigned char *xy);
void TOUCH_START(unsigned char channel);
void TOUCH_CAIJI(void);
void DrawDot(unsigned int x_orgi,unsigned int y_orgi);
void location();


unsigned char code picture1[];
unsigned char code s[10][1300];

unsigned int  ciPressureWait =10;
unsigned int  ciStartWait =100;
unsigned int  ciCommandPenIrq =0xD0;       //  11010000B
unsigned int  ciCommandSampleX =0xD3;       //  11010011B
unsigned int  ciCommandSampleY =0x93;       //  10010011B
unsigned int  ciSampleTimes =6;


unsigned int ADC_X_START=230;
unsigned int ADC_X_END=3867;
//unsigned int ADC_X_SE=3867-230;
//unsigned int ADC_X_START=3867;
//unsigned int ADC_X_END=230;
unsigned int ADC_Y_START=287;
unsigned int ADC_Y_END=3853;
//unsigned int ACD_Y_SE=3856-287;
unsigned int SCREEN_X_START=0;
unsigned int SCREEN_X_END=240;
//unsigned int SCREEN_X_START=240;
//unsigned int SCREEN_X_END=0;
unsigned int SCREEN_Y_START=0;
unsigned int SCREEN_Y_END=400;


unsigned int  idata  vx,vy,x_len,y_len,x_org,y_org;
bit idata pass;
int px_max=0,py_max=0,px_min=-2,py_min=-2;

int x_slope,y_slope;
int x_offset,y_offset;


unsigned int idata  id;											  

/***********************************
 touch  sreen  function
 **********************************/


/*********************touch screen end**************/
 void serial_delay()
{
	int i;
	for (i = 0;i<general_delay; i++ ){};
}


void TOUCH_RESET(void)
{
 T_DCLK=0;
 T_CS=1;
 T_DIN=0;
 T_DCLK=0;
 T_DOUT=0;
 T_CS=0;
 }


void ADS7843_SPI_Start( void )
{
 TP_DCLK(0);
 TP_CS(1);
 TP_DIN(1);
 TP_DCLK(1);
 TP_CS(0);
}

void ADS7843_Write (unsigned char cmd )
{
    unsigned char buf, i, j ;
    TP_DCLK(0);
	for( i = 0; i < 8; i++ )
	{
		buf = (cmd >> (7-i)) & 0x1 ;
		TP_DIN(buf);

		for( j = 0; j < general_delay; j++ );
		TP_DCLK(1);

		for( j = 0; j < general_delay; j++ );
		TP_DCLK(0);
	}
}


unsigned int ADS7843_Read ( void )
{
	unsigned int buf = 0 ;
	unsigned char i, j ;

	for( i = 0; i < 12; i++ )
	{
		buf = buf << 1 ;
		TP_DCLK(1);
		for( j = 0; j < general_delay; j++ );
		TP_DCLK(0);
		for( j = 0; j < general_delay; j++ );
		if ( TP_DOUT )
                buf = buf + 1 ;
                //TP_DCLK(0);
		for( j = 0; j < general_delay; j++ );
	//for( j = 0; j < general_delay; j++ );
	}
	for( i = 0; i < 4; i++ )
	{
		TP_DCLK(1);
		for( j = 0; j < general_delay; j++ );		//200ns
		TP_DCLK(0);
		for( j = 0; j < general_delay; j++ );
	}
	return( buf ) ;
}

void ADS7843_disp()// interrupt 0 using 0
{
	long x, y;
	char j ;
	unsigned int px_max=0,py_max=0,px_min=0,py_min=0;
	x_slope=0.066;//0.066
	y_slope=0.090;//0.090
	x_offset = -15.18;
	y_offset = -25.83;

	for( j = 0; j < general_delay; j++ );
if (TP_IRQ == 0)
	{
		ADS7843_SPI_Start() ;
		for( j = 0; j < 5; j++ );
		//while ( TP_BUSY ) ;

		ADS7843_Write( CHY ) ;
		y = ADS7843_Read();//dummy
		ADS7843_Write( CHY ) ;
		y = ADS7843_Read();//dummy
		ADS7843_Write( CHY ) ;
		y += ADS7843_Read();//dummy
		y = y/2;
//		y= 0.066 * (ADC_Y_END - y);
		y= 0.112 * (y-287);

		ADS7843_Write( CHX ) ;
		x = ADS7843_Read() ;
		ADS7843_Write( CHX ) ;
		x = ADS7843_Read() ;
		ADS7843_Write( CHX ) ;
		x += ADS7843_Read();//dummy
		x = x/2;

//		x= 0.0896 * (x - 230);//0.896
	    x= 0.0896 * (ADC_X_END - x);//0.896
		//if( (x != 4095)&&(y != 4095)&&(x != 0)&&(y != 0) )
		//	{
				//x=x/(4095/240);
				//y=y/(4095/320);
				if(x<SCREEN_X_START)
					x=SCREEN_X_START;
				else if (x>SCREEN_X_END)
					x=SCREEN_X_END;

				if(y<SCREEN_Y_START)
					y=SCREEN_Y_START;
				else if (y>SCREEN_Y_END)
					y=SCREEN_Y_END;
				LCD_Color_with_ADD(0x07,0xE0,x,x+1,y,y+1);
		//	}
		TP_CS(1);
	}
}





void LCD_Color_with_ADD(char data1,char data2,int x1,int x2,int y1,int y2){
     unsigned char i,j;
//     unsigned char hor,ver;
     comm_out(0x02);        data_out(x1>>8);
	 comm_out(0x03);        data_out(x1); 
	 comm_out(0x04);        data_out(x2>>8); 
	 comm_out(0x05);        data_out(x2);  
	 comm_out(0x06);        data_out(y1>>8);
	 comm_out(0x07);        data_out(y1); 
	 comm_out(0x08);        data_out(y2>>8); 
	 comm_out(0x09);        data_out(y2);   

     LCD_WRITE_CMD(0x00,0x22);
	for(i=0;i<y2-y1+1;i++)
            for (j=0;j<x2-x1+1;j++)
			{
                        LCD_WRITE_DATA(data1,data2);
			}
	}



void DrawDot(unsigned int x_orgi,unsigned int y_orgi)
{
//	int i,j;
	int Pox,Poy,Px_bak,Py_bak;
//        unsigned char colorH,colorL;
        px_max=x_orgi>px_max?x_orgi:px_max; //get x(max)
        py_max=y_orgi>py_max?y_orgi:py_max; //get y(max)
        px_min=x_orgi>px_min?px_min:x_orgi; //get x(min)
        py_min=y_orgi<py_min?y_orgi:py_min; //get y(min)
	Px_bak=x_orgi;
	Py_bak=y_orgi;
	Pox=Px_bak/21;
	Poy=Py_bak/16;
	LCD_Color_with_ADD(0x07,0xE0,Pox,Pox+3,Poy,Poy+3);

}

void main(){

        HD_reset();
        delayms(50);
		LCD_INIT_HX8352A();


	while(1)
	{
    //   LCD_TEST_SingleColor(0xf8,0x00); //display red
//		Keypress();
   //     LCD_TEST_SingleColor(0x07,0xe0);  //display blue
//		Keypress();
//		LCD_TEST_SingleColor(0x00,0x1f);  //display blue
//       	Keypress();
//		LCD_TEST_SingleColor(0xff,0xff);   //display white
//		Keypress();
//	    LCD_TEST_SingleColor(0x00,0x00);  // display black
//		Keypress();
//		LCD_TEST_SingleColor(0xff,0xe0);   //display white
//		Keypress();
//	    LCD_TEST_SingleColor(0x07,0xff);  // display black
//		Keypress();
        LCD_TEST_Picture1(picture1);
        LCD_TEST_Picture2(picture1);
       LCD_TEST_Picture3(picture1);
      LCD_TEST_Picture4(picture1);
       LCD_TEST_Picture5(picture1);
//		        delayms(5000);
//		Keypress();				  
        LCD_show_window();
       do{
	ADS7843_disp();
        }
        while(1);

	}
}

void HD_reset(){
	RESETB=0;
	delayms(50);
	RESETB=1;
	delayms(10);
	}


  void LCD_INIT_HX8352A(void)
	{
  //**********************Start initial squence******************
  unsigned char kk;
	 Write_reg(0x83,0x02); 
	 Write_reg(0x85,0x03); 
    	 Write_reg(0x8B,0x00); 
    	 Write_reg(0x8C,0x13); 
    	 Write_reg(0x91,0x01); 
	 Write_reg(0x83,0x00); 
	 delayms(5);

         //---------------- Gamma Setting  ----------------------------
         Write_reg(0x3E,0xC4);
         Write_reg(0x3F,0x44);
         Write_reg(0x40,0x22);
         Write_reg(0x41,0x57);
         Write_reg(0x42,0x03);
         Write_reg(0x43,0x47);
         Write_reg(0x44,0x02);
         Write_reg(0x45,0x55);
         Write_reg(0x46,0x06);
         Write_reg(0x47,0x4C);
         Write_reg(0x48,0x06);
         Write_reg(0x49,0x8C); 

         //----------  Power Supply Setting  --------------------------      
         Write_reg(0x17,0x91); 
         Write_reg(0x23,0x01);
         Write_reg(0x2B,0xF9); 
         delayms(20);       
         Write_reg(0x18,0x3A); 

         Write_reg(0x1B,0x11); 
         Write_reg(0x83,0x02);
         Write_reg(0x8A,0x06);
         Write_reg(0x83,0x00);
         Write_reg(0x1A,0x11); 

         Write_reg(0x1C,0x0A); 
         Write_reg(0x1F,0x58); 
         delayms(30);
         Write_reg(0x19,0x0A);  
         Write_reg(0x19,0x1A); 
         delayms(50);
         Write_reg(0x19,0x12); 
         delayms(50);
         Write_reg(0x1E,0x2E); 
         delayms(100);
         
         //----------------  DUG FUNCTION ON ----------------------------
         Write_reg(0x5A,0x01);
       
	 //------------------- RGB LUT ----------------------------------     
         comm_out(0x5C);
         for(kk=0;kk<3;kk++)
         {
		data_out(0x00);//-----------0
		data_out(0x03);
		data_out(0x0A);
		data_out(0x0F);
		data_out(0x13);
		data_out(0x16);//-----------5
		data_out(0x19);
		data_out(0x1C);
		data_out(0x1E);
		data_out(0x1F);
		data_out(0x25);//-----------10
		data_out(0x2A);
		data_out(0x30);
		data_out(0x35);
		data_out(0x39);
		data_out(0x3D);//-----------15
		data_out(0x41);
		data_out(0x45);
		data_out(0x48);
		data_out(0x4C);
		data_out(0x4F);//-----------20
		data_out(0x53);
		data_out(0x58);
		data_out(0x5D);
		data_out(0x61);
		data_out(0x66);//-----------25
		data_out(0x6A);
		data_out(0x6E);
		data_out(0x72);
		data_out(0x76);
		data_out(0x7A);//-----------30
		data_out(0x7E);
		data_out(0x82);
		data_out(0x85);
		data_out(0x89);
		data_out(0x8D);//-----------35
		data_out(0x90);
		data_out(0x94);
		data_out(0x97);
		data_out(0x9A);
		data_out(0x9D);//-----------40
		data_out(0xA2);
		data_out(0xA5);
		data_out(0xA9);
		data_out(0xAC);
		data_out(0xB0);//-----------45
		data_out(0xB4);
		data_out(0xB8);
		data_out(0xBC);
		data_out(0xC0);
		data_out(0xC3);//-----------50
		data_out(0xC8);
		data_out(0xCC);
		data_out(0xD2);
		data_out(0xD6);
		data_out(0xDC);//-----------55		
		data_out(0xDF);
		data_out(0xE2);
		data_out(0xE5);
		data_out(0xE8);
		data_out(0xEC);//-----------60
		data_out(0xEF);
		data_out(0xF4);
		data_out(0xFF);//-----------63
         }

         //-------------  Display ON Setting  --------------------------
         Write_reg(0x3C,0xC0); 
         Write_reg(0x3D,0x1C); 
         Write_reg(0x34,0x38);
         Write_reg(0x35,0x38);
         Write_reg(0x24,0x38);
         delayms(50);
         Write_reg(0x24,0x3C);
         Write_reg(0x16,0x1C); 

         Write_reg(0x01,0x06); 
         Write_reg(0x55,0x00);
         Write_reg(0x02,0x00); 
         Write_reg(0x03,0x00);
         Write_reg(0x04,0);
         Write_reg(0x05,239);
         Write_reg(0x06,0x00); 
         Write_reg(0x07,0x00);
         Write_reg(0x08,0x01);
         Write_reg(0x09,0x8f);
         comm_out(0x22);
	 delayms(10); 	 
}

//-------------------------------8 bit chansfer------------
//************************************
void comm_out(unsigned char com2)
{
	RDB=1;
	RS=0;
	
	CSB=0;
//	WRB=0;
//	P1=0x00;
	P0=com2;		
	WRB=0;
	WRB=1;
	CSB=1;		
  }

void data_out(unsigned char dat2)
{
	RDB=1;
	RS=1;
	
	CSB=0;
//	WRB=0;
//	P1=0x00;
	P0=dat2;		
	WRB=0;
	WRB=1;
	CSB=1;	
  }
//*************write register command************
void Write_regCOM(unsigned char com1,unsigned char com2)
{
	RDB=1;
	RS=0;
	
	CSB=0;
//	WRB=0;
	P0=com1;
	WRB=0;
	WRB=1;
	P0=com2;		
	WRB=0;
	WRB=1;
	CSB=1;		
  }

//*************write  data************
void Write_DAT(unsigned char dat1,unsigned char dat2)
{
	RDB=1;
	RS=1;
	
	CSB=0;
//	WRB=0;
	P0=dat1;
	WRB=0;
	WRB=1;
	P0=dat2;		
	WRB=0;
	WRB=1;
	CSB=1;	
  }

void Write_reg(unsigned char comx,unsigned char datex)
{
	Write_regCOM(0x00,comx);
	Write_DAT(0x00,datex);
}

void LCD_WRITE_CMD(char cmd1,char cmd2){
      RS = 0;
      RDB = 1;
      WRB = 1;
      CSB = 0;
      DataBusL = cmd1;
      WRB = 0;
      //;
      WRB = 1;
      DataBusL = cmd2;
      WRB = 0;
      ;
      WRB = 1;
      CSB = 1;
	}

void LCD_WRITE_DATA(char dataa,char datab){
      RS = 1;
      RDB = 1;
      WRB = 1;
      CSB = 0;

      DataBusL = dataa;
      WRB = 0;
      //;
      WRB = 1;
      DataBusL = datab;
      WRB = 0;
      ;
      WRB = 1;
      CSB = 1;
	}

void LLCD_WRITE_CMD(int cmd){
      RS = 0;
      RDB = 1;
      WRB = 1;
      CSB = 0;

      DataBusL = cmd>>8;
      WRB = 0;
      //;
      WRB = 1;
      DataBusL = cmd;
      WRB = 0;
      ;
      WRB = 1;
      CSB = 1;
	}

void LLCD_WRITE_DATA(int dataa){
      RS = 1;
      RDB = 1;
      WRB = 1;
      CSB = 0;
      DataBusL = dataa>>8;
      //DataBusL = dataa;
     WRB = 0;
      //;
      WRB = 1;
      DataBusL = dataa;
      WRB = 0;
      ;
      WRB = 1;
      CSB = 1;
	}

/*
//-------------------------------16 bit chansfer------------
//************************************
void comm_out(unsigned char com2)
{
	RDB=1;
	RS=0;
	
	CSB=0;
//	WRB=0;
	P1=0x00;
	P0=com2;		
	WRB=0;
	WRB=1;
	CSB=1;		
  }

void data_out(unsigned char dat2)
{
	RDB=1;
	RS=1;
	
	CSB=0;
//	WRB=0;
	P1=0x00;
	P0=dat2;		
	WRB=0;
	WRB=1;
	CSB=1;	
  }
//*************write register command************
void Write_regCOM(unsigned char com1,unsigned char com2)
{
	RDB=1;
	RS=0;
	
	CSB=0;
//	WRB=0;
	P1=com1;
	P0=com2;		
	WRB=0;
	WRB=1;
	CSB=1;		
  }

//*************write  data************
void Write_DAT(unsigned char dat1,unsigned char dat2)
{
	RDB=1;
	RS=1;
	
	CSB=0;
//	WRB=0;
	P1=dat1;
	P0=dat2;		
	WRB=0;
	WRB=1;
	CSB=1;	
  }

void Write_reg(unsigned char comx,unsigned char datex)
{
	Write_regCOM(0x00,comx);
	Write_DAT(0x00,datex);
}

void LCD_WRITE_CMD(char cmd1,char cmd2){
      RS = 0;
      RDB = 1;
      WRB = 1;
      CSB = 0;
      DataBusH = cmd1;
     // DataBusL = cmd1;
      //WRB = 0;
      //;
      //WRB = 1;
      DataBusL = cmd2;
      WRB = 0;
      ;
      WRB = 1;
      CSB = 1;
	}

void LCD_WRITE_DATA(char dataa,char datab){
      RS = 1;
      RDB = 1;
      WRB = 1;
      CSB = 0;
      DataBusH = dataa;
      //DataBusL = dataa;
      //WRB = 0;
      //;
      //WRB = 1;
      DataBusL = datab;
      WRB = 0;
      ;
      WRB = 1;
      CSB = 1;
	}

void LLCD_WRITE_CMD(int cmd){
      RS = 0;
      RDB = 1;
      WRB = 1;
      CSB = 0;
      DataBusH = cmd>>8;
     // DataBusL = cmd1;
      //WRB = 0;
      //;
      //WRB = 1;
      DataBusL = cmd;
      WRB = 0;
      ;
      WRB = 1;
      CSB = 1;
	}

void LLCD_WRITE_DATA(int dataa){
      RS = 1;
      RDB = 1;
      WRB = 1;
      CSB = 0;
      DataBusH = dataa>>8;
      //DataBusL = dataa;
     // WRB = 0;
      //;
      //WRB = 1;
      DataBusL = dataa;
      WRB = 0;
      ;
      WRB = 1;
      CSB = 1;
	}
*/
      
void send_host_reg_command(int cmd,int dataa)
     {
     LLCD_WRITE_CMD(cmd);
     LLCD_WRITE_DATA(dataa);
     }



void adderset(int xs,int xe,int ys,int ye){

     comm_out(0x02);        data_out(xs>>8);
	 comm_out(0x03);        data_out(xs); 
	 comm_out(0x04);        data_out(xe>>8); 
	 comm_out(0x05);        data_out(xe);  
	 comm_out(0x06);        data_out(ys>>8);
	 comm_out(0x07);        data_out(ys); 
	 comm_out(0x08);        data_out(ye>>8); 
	 comm_out(0x09);        data_out(ye);   

     LLCD_WRITE_CMD(0x0022);
}


void LCD_TEST_id(unsigned char *id){
     unsigned	char i,j,pixH,pixL;
       	 for (i=0;i<25;i++){
			for (j=0;j<25;j++){
				pixH=*id++;
				pixL=*id++;
				LCD_WRITE_DATA(pixH,pixL);
                                //LCD_WRITE_DATA(0xf8,0x00);
				}
		}

	}


void LCD_id_SHOW(int a,int b,int c,int d){

     adderset(0,25,0,25);
     LCD_TEST_id(s[a]);
     adderset(25,50,0,25);
     LCD_TEST_id(s[b]);
     adderset(50,75,0,25);
     LCD_TEST_id(s[c]);
     adderset(75,100,0,25);
     LCD_TEST_id(s[d]);

}


void LCD_TEST_SingleColor(char colorH,char colorL){
     int i,j;
	     Write_reg(0x02,0x00); 
         Write_reg(0x03,0x00);
         Write_reg(0x04,0);
         Write_reg(0x05,239);
         Write_reg(0x06,0x00); 
         Write_reg(0x07,0x00);
         Write_reg(0x08,0x01);
         Write_reg(0x09,0x8f);
     LCD_WRITE_CMD(0x00,0x22);

	 for (i=0;i<400;i++){
		for (j=0;j<240;j++){
			LCD_WRITE_DATA(colorH,colorL);
			}
		}
	}



void LCD_TEST_Picture1(unsigned char *picture1){
     unsigned	char i,j,pixH,pixL;

	     Write_reg(0x02,0x00); 
         Write_reg(0x03,0x00);
         Write_reg(0x04,0);
         Write_reg(0x05,239);
         Write_reg(0x06,0x00); 
         Write_reg(0x07,0x00);
         Write_reg(0x08,0x01);
         Write_reg(0x09,0x8f);
     LCD_WRITE_CMD(0x00,0x22);


       	 for (i=0;i<80;i++){
			for (j=0;j<240;j++){
				pixH=*picture1++;
				pixL=*picture1++;
				LCD_WRITE_DATA(pixH,pixL);
                                //LCD_WRITE_DATA(0xf8,0x00);
				}
		}

	}

void LCD_TEST_Picture2(unsigned char *picture1){
     unsigned	char i,j,pixH,pixL;

       	 for (i=0;i<80;i++){
			for (j=0;j<240;j++){
				pixH=*picture1++;
				pixL=*picture1++;
				LCD_WRITE_DATA(pixH,pixL);
                                //LCD_WRITE_DATA(0xf8,0x00);
				}
		}

	}

void LCD_TEST_Picture3(unsigned char *picture1){
     unsigned	char i,j,pixH,pixL;

       	 for (i=0;i<80;i++){
			for (j=0;j<240;j++){
				pixH=*picture1++;
				pixL=*picture1++;
				LCD_WRITE_DATA(pixH,pixL);
                                //LCD_WRITE_DATA(0xf8,0x00);
				}
		}

	}


void LCD_TEST_Picture4(unsigned char *picture1){
     unsigned	char i,j,pixH,pixL;

       	 for (i=0;i<80;i++){
			for (j=0;j<240;j++){
				pixH=*picture1++;
				pixL=*picture1++;
				LCD_WRITE_DATA(pixH,pixL);
                                //LCD_WRITE_DATA(0xf8,0x00);
				}
		}

	}

void LCD_TEST_Picture5(unsigned char *picture1){
     unsigned	char i,j,pixH,pixL;

       	 for (i=0;i<80;i++){
			for (j=0;j<240;j++){
				pixH=*picture1++;
				pixL=*picture1++;
				LCD_WRITE_DATA(pixH,pixL);
                                //LCD_WRITE_DATA(0xf8,0x00);
				}
		}

	}  

void LCD_show_window(void){
     int i,j;
	     Write_reg(0x02,0x00); 
         Write_reg(0x03,0x00);
         Write_reg(0x04,0);
         Write_reg(0x05,239);
         Write_reg(0x06,0x00); 
         Write_reg(0x07,0x00);
         Write_reg(0x08,0x01);
         Write_reg(0x09,0x8f);
     LCD_WRITE_CMD(0x00,0x22);

	 for (i=0;i<1;i++){
		for (j=0;j<240;j++){
			LCD_WRITE_DATA(0xff,0xff);
			}
		}

	 for (i=0;i<398;i++){
		for (j=0;j<240;j++){
                if((j==0)||(j==239)){LCD_WRITE_DATA(0xff,0xff);}
                else
			{LCD_WRITE_DATA(0x00,0x00);}
			}
		}

	 for (i=0;i<1;i++){
		for (j=0;j<240;j++){
			LCD_WRITE_DATA(0xff,0xff);
			}
		}
	}
/*********************��ʾͼƬ�����ⲿ�洢����*******************************/
 
void display_eprom(unsigned char p)
{
	int i,d1,d2,k;
	unsigned int addr;	

	for(k=0;k<10;k++)
	{
	if(k<9)
		{
		addr=0x0000;		
		for(i=0;i<8192;i++)
			{
			   	d1=XBYTE[addr++];
				d2=XBYTE[addr++];
				LCD_WRITE_DATA(d1,d2);
				pic_select=(10*(p-1)+k);
			}
		}
	else
		{
		addr=0x0000;		
		for(i=0;i<3072;i++)
			{
                d1=XBYTE[addr++];
				d2=XBYTE[addr++];
				LCD_WRITE_DATA(d1,d2);
				pic_select=(10*(p-1)+k);
			}
		}
	}
} 

void Keypress()
	{
	while(KEY)
	delayms(10);
	}

void delayms(int value)
	{
	while (value){
		delayus(99);
		value--;
		}
	}

void delayus(int value1)
	{
	while (value1)

	value1--;
	}

unsigned char code picture1[]={
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XD6,0X37,0X59,0X00,0X59,0X00,0XD6,0X37,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,
0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,
0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,
0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,
0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,
0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,
0XFE,0X6B,0XF6,0X28,0XF5,0XA5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,
0XF5,0XA5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0X59,0X00,0XD6,0X36,0XD5,0XB0,0X59,0X00,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,
0XF6,0X29,0XCD,0X29,0X59,0X00,0X59,0X00,0XD6,0X37,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,
0XF5,0XE6,0XFE,0X25,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0X65,
0XF5,0XE4,0XED,0XEB,0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X66,
0XFE,0XE8,0XFF,0X28,0XFF,0X28,0XFE,0XE7,0XFE,0XA7,0XFE,0X66,0XFE,0X26,0XF6,0X2B,
0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XFE,0X27,0XFE,0XA7,0XFE,0XE8,0XFE,0XE8,
0XFF,0X28,0XFE,0XE7,0XFE,0XA7,0XFE,0X66,0XFE,0X26,0XED,0XEB,0XF6,0XF6,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X25,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,
0XFE,0XA6,0XFE,0X65,0XF5,0XE4,0XED,0XEB,0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,
0XF5,0XE6,0XFE,0X25,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0X65,
0XF5,0XE4,0XED,0XEB,0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X25,
0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0X65,0XF5,0XE4,0XED,0XEB,
0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X25,0XFE,0XA6,0XFE,0XE7,
0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0X65,0XF5,0XE4,0XED,0XEB,0XF6,0XF6,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X66,0XFE,0XE8,0XFF,0X28,0XFF,0X28,0XFE,0XE7,
0XFE,0XA7,0X59,0X00,0XD4,0XE4,0XCC,0XE8,0X59,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,
0XFE,0XE8,0XFF,0X69,0XFF,0X6A,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE8,
0XFE,0XE7,0XFE,0X24,0XED,0X65,0XED,0XEA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,0XFF,0X29,0XFF,0X69,
0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X66,
0XED,0XE6,0XEE,0X2A,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFF,0X29,0XFF,0X28,0XFF,0X29,0XFF,0X69,0XFF,0X69,
0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE7,0XFE,0XE7,0XFE,0X66,0XED,0XE6,0XED,0XEA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,
0XF6,0X6B,0XFE,0X26,0XFE,0XE7,0XFF,0X29,0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X29,
0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X24,0XED,0X65,0XED,0XEA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,
0XFE,0XE8,0XFF,0X69,0XFF,0X6A,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE8,
0XFE,0XE7,0XFE,0X24,0XED,0X65,0XED,0XEA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,0XFE,0XE7,0XFF,0X29,
0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X24,
0XED,0X65,0XED,0XEA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,0XFE,0XE8,0XFF,0X69,0XFF,0X6A,0XFF,0X69,
0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE8,0XFE,0XE7,0XFE,0X24,0XED,0X65,0XED,0XEA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,
0XED,0XA8,0XD4,0X63,0XCC,0X22,0XDC,0XA4,0XF6,0X67,0XFF,0X69,0XFF,0X69,0XFF,0X29,
0XFF,0X28,0XF6,0X25,0XDC,0XA3,0XCC,0X22,0XD4,0X63,0XE5,0XA8,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,0XFE,0X26,0XFF,0X6A,
0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,
0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XF5,0XA3,0XE5,0XAA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,0XFE,0X26,0XFF,0X6A,0X81,0X80,0XFF,0X6A,
0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0X81,0X80,0XFE,0XE7,0XFE,0XA6,
0XFE,0X64,0XF5,0XA3,0XEE,0X2A,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XF5,0X68,0XFA,0X80,0XF2,0X40,0XFD,0X66,0XFA,0X80,0XEA,0X00,0XFD,0XA7,
0XFF,0XAB,0XFE,0X68,0XFA,0X80,0XF2,0X40,0XFD,0X24,0XFA,0X80,0XEA,0X00,0XF4,0XE4,
0XE5,0XAA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,
0XBC,0X23,0X93,0X4B,0XE6,0X28,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,
0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XE5,0XA5,0XA3,0XCD,0XA3,0X01,0XE5,0XAA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,0XFE,0X26,0XFF,0X6A,
0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,
0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XF5,0XA3,0XE5,0XAA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,0XFE,0X26,0XFF,0X28,0XFF,0X6A,0XFF,0X6A,
0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,
0XFE,0X64,0XF5,0XA3,0XE5,0XAA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XF6,0X2B,0XFE,0X26,0XFF,0X6A,0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,
0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XED,0X63,
0XE5,0XAA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0X68,
0XD4,0XEA,0XEE,0XB6,0XFF,0XFF,0XEE,0XB6,0XD4,0XEA,0XEE,0X68,0XFF,0XAB,0XFF,0X6A,
0XEE,0X67,0XD4,0XEA,0XEE,0XB6,0XFF,0XFF,0XEE,0XB6,0XD4,0XEA,0XDD,0X67,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XF6,0X25,0XFF,0X6A,0XEE,0X67,
0XD4,0X63,0XCB,0XC2,0XD4,0X64,0XEE,0XAB,0XFF,0XAE,0XFF,0XAD,0XEE,0X69,0XD4,0X63,
0XCB,0XC2,0XD4,0X62,0XED,0XA4,0XFE,0XA6,0XE5,0X24,0XDD,0XED,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XF6,0X25,0XFF,0X69,0X81,0X80,0XFF,0XAB,0XFF,0XAD,
0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,0X81,0X80,0XFE,0XE7,
0XFE,0XA5,0XFE,0X24,0XE5,0X24,0XE6,0X2D,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFE,0XF3,0XFA,0X80,0XFE,0XB8,0XFC,0XED,0XEA,0X40,0XF2,0X40,0XF2,0X40,0XD9,0XC0,
0XFF,0XAE,0XFA,0X80,0XFE,0X76,0XFC,0XED,0XEA,0X00,0XF2,0X40,0XF2,0X40,0XD1,0X80,
0XE5,0X65,0XDD,0XED,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XF6,0X25,
0XD5,0X24,0X79,0XC0,0X59,0X00,0XDD,0XE9,0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,
0XFF,0XAB,0XFF,0X6A,0XEE,0X27,0X59,0X00,0X71,0XC0,0XE4,0XE3,0XDC,0XE4,0XDD,0XED,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0X39,0X82,0X42,0X08,
0X5A,0XC8,0X94,0X67,0XCE,0X2B,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,0XBD,0XA8,0X8C,0X25,
0X62,0XC7,0X5A,0X86,0X00,0X00,0X18,0XC0,0X00,0X00,0XDD,0XED,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XF6,0X25,0XAB,0X43,0XAB,0X83,0XAB,0X84,0X79,0X40,
0XEE,0XAC,0XFF,0XAF,0XFF,0XAE,0XFF,0XAD,0XE6,0X69,0X79,0X40,0XAB,0X43,0XAB,0X42,
0XAB,0X02,0XFE,0X24,0XE4,0XE4,0XDD,0XED,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFE,0XF3,0XF6,0X26,0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAD,0XFF,0XAE,0XFF,0XEF,
0XFF,0XAE,0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XFE,0XA6,
0XDC,0XE4,0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XCB,0XC2,
0XEE,0XB7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0XF8,0XCC,0X24,0XFF,0XAE,0XFF,0XAD,
0XCC,0X23,0XEE,0XB7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0XF8,0XC3,0X82,0XE6,0X2D,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XFF,0X29,0XFF,0X6A,0XCC,0X23,
0XE6,0X31,0XFF,0XFF,0XE6,0X31,0XCC,0X24,0XFF,0XF2,0XFF,0XF0,0XD4,0X63,0XE6,0X31,
0XFF,0XFF,0XEE,0XB5,0XD4,0X22,0XFE,0X64,0XFE,0X24,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XFE,0XE8,0X81,0X80,0XD4,0X63,0XCB,0XC2,0XD4,0X64,
0XEE,0XAD,0XFF,0XF3,0XFF,0XF2,0XEE,0XAC,0XD4,0X63,0XCB,0XC2,0XD4,0X63,0X81,0X80,
0XFE,0XA6,0XFE,0X64,0XFD,0XE3,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,
0XF5,0XE7,0XFA,0X80,0XFC,0XAD,0XF2,0X40,0XEA,0X40,0XEA,0X00,0XE2,0X00,0XD1,0X80,
0XFF,0XF2,0XFA,0X80,0XFC,0XAD,0XF2,0X40,0XEA,0X40,0XEA,0X00,0XE2,0X00,0XC9,0X80,
0XFD,0XE3,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XED,0XA5,0XD4,0X63,
0XE5,0XAF,0XF7,0X7B,0X9B,0X89,0X59,0X00,0XDD,0X68,0XF7,0X70,0XFF,0XF2,0XFF,0XF0,
0XFF,0X2C,0XDD,0X25,0X59,0X00,0X93,0X08,0XF7,0X7B,0XE5,0XF0,0XD4,0X22,0XDD,0X28,
0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0X00,0X00,0XF7,0XBE,0X9C,0XF3,0X7B,0XD0,
0X00,0X00,0X5A,0XCC,0X5A,0X85,0X8C,0X29,0XFF,0XF2,0X8C,0X29,0X4A,0X44,0X8C,0X72,
0X73,0XCF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X82,0XC3,0X83,0XCA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XFE,0XE7,0XFF,0X6A,0XFF,0XFF,0XF7,0X3A,0XE6,0X6D,
0XAB,0X86,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,0XAB,0X85,0XE6,0X69,0XFF,0XFF,0XDD,0XF1,
0XFE,0XA6,0XFE,0X64,0XFD,0XA3,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,
0XF5,0XA6,0XFF,0X29,0XFF,0X6A,0XFF,0X6B,0XFF,0XAE,0XFF,0XB0,0XFF,0XF2,0XFF,0XF3,
0XFF,0XF2,0XFF,0XF0,0XFF,0XAE,0XFF,0X6B,0XFF,0X6A,0XFF,0X28,0XFE,0XA6,0XFE,0X64,
0XFE,0X24,0XDD,0X28,0XE6,0XF9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XB3,0X01,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB3,0X01,0XFF,0XF2,0XFF,0XF0,
0XB3,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB3,0X01,0XDD,0X28,
0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,0XF6,0X66,0XFF,0X29,0XFF,0X6A,0XC3,0X82,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC3,0X82,0XFF,0XF6,0XFF,0XF4,0XC3,0X82,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XC3,0X82,0XFE,0X65,0XFE,0X23,0XED,0X22,0XDD,0XAD,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XF6,0XF3,0XF6,0X25,0X81,0X80,0XD4,0X63,0XF7,0X3A,0XFF,0XFF,0XF7,0X3A,
0XD4,0X65,0XFF,0XF7,0XFF,0XF6,0XD4,0X64,0XF7,0X3A,0XFF,0XFF,0XF7,0X3A,0XD4,0X63,
0X81,0X80,0XFE,0X65,0XFE,0X23,0XED,0X22,0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,
0XF6,0X66,0XFD,0XA6,0XEA,0X00,0XE2,0X00,0XE1,0XC0,0XD9,0XC0,0XD1,0X80,0XF6,0X30,
0XFF,0XF6,0XFE,0XAF,0XEA,0X00,0XE2,0X00,0XE1,0XC0,0XD9,0XC0,0XD1,0X80,0XF5,0X64,
0XFE,0X23,0XED,0X22,0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,0XDC,0XE4,0XDD,0X6E,
0XFF,0XFF,0XFF,0XFF,0XEF,0X7C,0XFF,0XFF,0X9B,0X48,0XC4,0X24,0XFF,0XF5,0XFF,0XF3,
0XCC,0X24,0X82,0X45,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0XAF,0XD4,0X62,
0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X52,0X02,0X62,0X42,0XBD,0XF8,0X9C,0XF4,0X6B,0X4D,
0X00,0X00,0X63,0X0C,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XF7,0XBE,0XA5,0X35,
0X8C,0X72,0X00,0X00,0X00,0X00,0X00,0X00,0X51,0XC0,0X00,0X00,0XD5,0X6B,0XEE,0XF7,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XF6,0XF3,0XF6,0X25,0XFF,0X29,0XFF,0XFF,0XE6,0X32,0XEE,0XB0,0XFF,0XF4,
0XFF,0XF6,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,0XFF,0XF0,0XFF,0XAD,0XFF,0XFF,0XD5,0XB2,
0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XED,0X22,0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,
0XF6,0X66,0XFF,0X28,0XFF,0X6A,0XFF,0X6D,0XFF,0XB0,0XFF,0XB3,0XFF,0XF6,0XFF,0XF6,
0XFF,0XF6,0XFF,0XF3,0XFF,0XB0,0XFF,0X6D,0XFF,0X2A,0XFE,0XE8,0XFE,0XA7,0XFE,0X65,
0XFE,0X23,0XED,0X22,0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,0XF6,0X25,0XBB,0X82,
0XE6,0XB7,0XFF,0XFF,0X81,0X80,0X81,0X80,0XEE,0XB7,0XBB,0X85,0XFF,0XF6,0XFF,0XF4,
0XBB,0X83,0XE6,0XB7,0XFF,0XFF,0X81,0X80,0X81,0X80,0XEE,0XB7,0XBB,0X41,0XED,0X22,
0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XC3,0X82,
0XAB,0XCB,0X79,0X40,0XAB,0XCB,0XC3,0X82,0XFF,0XF8,0XFF,0XF6,0XC3,0X82,0XAB,0XCB,
0X79,0X40,0XAB,0XCB,0XC3,0X82,0XFE,0XA5,0XFE,0X23,0XFD,0XA2,0XD4,0XA6,0XEF,0X3C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XED,0XE9,0XFE,0XA7,0XFF,0X69,0XCB,0XC2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XCB,0XC2,0XFF,0XFA,0XFF,0XF8,0XCB,0XC2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCB,0XC2,
0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XA2,0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,
0XFE,0XE7,0XFF,0X69,0XF5,0X26,0XD1,0X80,0XD1,0X80,0XC9,0X80,0XE4,0X6B,0XFF,0XFA,
0XFF,0XF8,0XFF,0XF6,0XF5,0X6B,0XD1,0X80,0XD1,0X80,0XC9,0X40,0XE4,0X23,0XFE,0XA5,
0XFE,0X23,0XFD,0XA2,0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0XCC,0X22,0XEE,0XF8,
0XFF,0XFF,0X81,0X80,0X81,0X80,0XFF,0XFF,0XFF,0XFF,0XC4,0X27,0XF7,0X34,0XF7,0X32,
0XC3,0XC6,0XFF,0XFF,0XFF,0XFF,0X81,0X80,0X81,0X80,0XFF,0XFF,0XF6,0XF9,0XC3,0X81,
0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0X72,0XC2,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X29,0X03,0X6B,0X07,0X29,0X42,0X84,0X30,0X6B,0X4D,
0X52,0X8A,0X00,0X00,0X00,0X00,0X00,0X00,0X72,0X81,0XED,0X22,0XCC,0X64,0XEF,0X3C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XED,0XE9,0XFE,0XA7,0XFF,0X69,0XFF,0XAB,0XFF,0XFF,0XCD,0X2D,0XFF,0XF6,
0XFF,0XF8,0XFF,0XFA,0XFF,0XF8,0XFF,0XF6,0XFF,0XF2,0XFF,0XAE,0XFF,0XFF,0XF7,0X7C,
0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XA2,0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,
0XFE,0XA7,0XB3,0X01,0XED,0X66,0XFE,0XED,0XFF,0X30,0XD4,0XE9,0XB3,0X01,0XFF,0XF9,
0XFF,0XF8,0XC3,0X82,0XED,0XEA,0XFF,0X2D,0XFE,0XAA,0XD4,0XA4,0XAA,0XC1,0XFE,0X65,
0XFE,0X23,0XFD,0XA2,0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0XFE,0XA7,0XE5,0XE7,
0XBC,0X2A,0XE6,0X77,0X59,0X00,0X81,0X80,0XBC,0X2A,0XE6,0X31,0XFF,0XF8,0XFF,0XF6,
0XE6,0X2D,0XBC,0X2A,0XE6,0X77,0X59,0X00,0X81,0X80,0XBC,0X2A,0XE4,0XE2,0XFD,0XA2,
0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFF,0X28,0XFF,0X69,0XFF,0XAB,0XCC,0X23,
0X79,0X40,0X59,0X00,0X79,0X40,0XCC,0X65,0XFF,0XFA,0XFF,0XF7,0XD4,0X64,0X79,0X40,
0X59,0X00,0X79,0X40,0XD4,0X62,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XCC,0X63,0XDE,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XCB,0XC2,0XFF,0XFF,0XCD,0X73,0X79,0X40,
0XCB,0XC2,0XFF,0XFA,0XFF,0XFA,0XCB,0XC2,0XFF,0XFF,0XCD,0X73,0X79,0X40,0XCB,0XC2,
0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,
0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XED,0XEA,0XC1,0X40,0XED,0XF0,0XFF,0XFA,0XFF,0XFA,
0XFF,0XFA,0XFF,0XF7,0XFF,0XF3,0XF6,0X2B,0XC1,0X40,0XF5,0XE7,0XFE,0XE7,0XFE,0XA5,
0XFE,0X24,0XFD,0XE2,0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XC3,0XC2,0XEE,0XF9,
0XFF,0XFF,0X81,0X80,0X59,0X00,0XFF,0XFF,0XFF,0XFF,0XC4,0X28,0XEE,0XF4,0XEE,0XF2,
0XBB,0XC7,0XFF,0XFF,0XFF,0XFF,0X59,0X00,0X81,0X80,0XFF,0XFF,0XF7,0X3A,0XBB,0X41,
0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XE6,0X27,0X18,0X80,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X08,0X00,0XA4,0X6B,0XEE,0XB1,0X9C,0X6A,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X18,0X80,0XD4,0XE2,0XF5,0X62,0XCC,0X63,0XDE,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XFF,0XFF,0XCD,0X2D,0XFF,0XF7,
0XFF,0XFA,0XFF,0XFA,0XFF,0XFA,0XFF,0XF7,0XFF,0XF3,0XFF,0XEF,0XFF,0XAB,0XFF,0X69,
0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,
0XFE,0XA7,0XED,0X25,0X9A,0X00,0X9A,0X00,0X9A,0X00,0X9A,0X00,0XC4,0X28,0XFF,0XB9,
0XFF,0XB9,0XED,0XEB,0X9A,0X00,0X9A,0X00,0X9A,0X00,0X9A,0X00,0XAA,0XC1,0XFD,0XE5,
0XFD,0XE4,0XFD,0XA2,0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,
0XDD,0XE8,0XB3,0X43,0X9A,0X00,0XB3,0X45,0XDE,0X31,0XFF,0XFA,0XFF,0XFA,0XFF,0XF7,
0XFF,0XF3,0XDD,0XEA,0XB3,0X43,0X9A,0X00,0XB3,0X02,0XDD,0X24,0XFE,0X24,0XFD,0XE2,
0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XEE,0XAB,
0XB3,0X83,0X8A,0X41,0XB3,0X84,0XEE,0XB3,0XFF,0XF8,0XFF,0XF6,0XEE,0XAD,0XB3,0X83,
0X8A,0X41,0XB3,0X42,0XEE,0X25,0XFE,0XA5,0XFE,0X23,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XD4,0X63,0XF7,0X3A,0X79,0X40,0X59,0X00,
0XD4,0X65,0XFF,0XFA,0XFF,0XF8,0XD4,0X65,0XF7,0X3A,0X79,0X40,0X59,0X00,0XD4,0X63,
0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,
0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XFF,0XAE,0XFF,0XF2,0XFF,0XF6,0XFF,0XF8,0XFF,0XFA,
0XFF,0XF8,0XFF,0XF6,0XFF,0XF2,0XFF,0XAE,0XFF,0XAB,0XFF,0X69,0XFE,0XE7,0XFE,0XA5,
0XFE,0X23,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XCC,0X64,0XD5,0X70,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0XF9,0XAA,0XC2,0XFF,0XF8,0XFF,0XF6,
0XAB,0X02,0XEE,0XF9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XD5,0X71,0XCB,0XC2,
0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XF6,0XE8,0X8B,0X84,0X5A,0X43,
0X5A,0X84,0X8B,0XC8,0XC5,0X2C,0XEE,0X70,0XFF,0X76,0XFF,0XF6,0XC5,0XEC,0X83,0X85,
0X5A,0X42,0X5A,0X42,0X93,0X83,0XE5,0X63,0XED,0X62,0XFD,0XA2,0XC4,0X22,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XFF,0XAF,0XFF,0XF2,0XFF,0XF6,
0XFF,0XF8,0XFF,0XFA,0XFF,0XF8,0XFF,0XF6,0XFF,0XF2,0XFF,0XAE,0XFF,0XAB,0XFF,0X69,
0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,
0XFE,0X67,0X9A,0X82,0XFC,0X29,0X9A,0X41,0XFC,0X6A,0X9A,0X41,0XFE,0X73,0XFF,0X77,
0XFF,0X77,0XFF,0X33,0XA2,0X81,0XFD,0X29,0X9A,0X41,0XFB,0XC8,0XA2,0X81,0XC3,0X42,
0XFD,0XA4,0XFD,0XA2,0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,
0XFF,0XAB,0XFF,0XAE,0XFF,0XF2,0XFF,0XF6,0XFF,0XF8,0XFF,0XFA,0XFF,0XF8,0XFF,0XF6,
0XFF,0XF2,0XFF,0XAE,0XFF,0XAB,0XFF,0X69,0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XE2,
0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,0XFF,0X6A,0XFF,0XAD,
0XFF,0XF0,0XFF,0XF4,0XFF,0XF6,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,0XFF,0XF0,0XFF,0XAD,
0XFF,0X6A,0XFF,0X29,0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,0XEE,0X68,0XD4,0XE5,0XCC,0X63,0XD4,0XE7,
0XEE,0XB0,0XFF,0XF7,0XFF,0XF6,0XEE,0XAE,0XD4,0XE6,0XD4,0X63,0XD4,0XE4,0XEE,0X26,
0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,
0XFE,0XE7,0XFF,0X29,0XFF,0X6A,0XFF,0XAD,0XFF,0XF0,0XFF,0XF4,0XFF,0XF6,0XFF,0XF7,
0XFF,0XF6,0XFF,0XF4,0XFF,0XF0,0XFF,0XAD,0XFF,0X6A,0XFF,0X29,0XFE,0XE7,0XFE,0X65,
0XFE,0X23,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,0XF5,0X6A,0XAA,0X82,
0XD5,0X71,0XFF,0XFF,0XFF,0XFF,0XEE,0XF9,0XC4,0X6C,0XDD,0XAE,0XFF,0XF6,0XFF,0XF4,
0XDD,0XAA,0XC4,0X6B,0XEE,0XB8,0XFF,0XFF,0XFF,0XFF,0XD5,0XB2,0XA2,0X41,0XF4,0XA7,
0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,0XFF,0X6A,0XF7,0X2B,
0XF6,0XED,0XF6,0XEF,0XF7,0X72,0XFF,0XB5,0XFF,0XF6,0XFF,0XF4,0XFF,0XF0,0XFF,0X6C,
0XF6,0XE9,0XF6,0X67,0XF6,0X25,0XF6,0X24,0XFE,0X23,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,0XFF,0X6A,0XFF,0XAD,0XFF,0XF0,0XFF,0XF3,
0XFF,0XF6,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,0XFF,0XF0,0XFF,0XAD,0XFF,0X6A,0XFF,0X29,
0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE4,0XE4,
0XFD,0XE6,0XFC,0XA8,0XFB,0X48,0XAA,0X42,0XFB,0X47,0XAA,0X83,0XFD,0XAE,0XFE,0XF3,
0XFF,0X34,0XFE,0XF0,0XB3,0X44,0XFC,0X68,0XAA,0X42,0XFB,0X07,0XFB,0X48,0XFC,0X66,
0XFD,0X64,0XFD,0X62,0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,
0XFF,0X6A,0XFF,0XAD,0XFF,0XF0,0XFF,0XF4,0XFF,0XF6,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,
0XFF,0XF0,0XFF,0XAD,0XFF,0X6A,0XFF,0X29,0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XFD,0XA2,
0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,
0XFF,0XAE,0XFF,0XF0,0XFF,0XF2,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,0XFF,0XAE,0XFF,0XAB,
0XFF,0X6A,0XFF,0X28,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,0XFF,0XAE,0XFF,0XF0,
0XFF,0XF2,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,0XFF,0XAE,0XFF,0XAB,0XFF,0X6A,0XFF,0X28,
0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,
0XFE,0XA6,0XFF,0X28,0XCB,0XC1,0XBB,0X00,0XBB,0X00,0XBB,0X00,0XBB,0X00,0XBB,0X00,
0XBB,0X00,0XBB,0X00,0XBB,0X00,0XBB,0X00,0XBB,0X00,0XBB,0X00,0XCB,0XC1,0XFE,0X64,
0XFE,0X23,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFD,0X6C,0XF4,0X6D,
0XBA,0XC6,0X92,0X00,0XA2,0X83,0XA2,0X42,0XDD,0X2C,0XFF,0XB3,0XFF,0XF2,0XFF,0XF0,
0XFF,0XAE,0XDD,0X28,0XA2,0X42,0XA2,0X83,0X9A,0X00,0XB2,0X85,0XEB,0XCC,0XFC,0XA9,
0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,
0XFF,0XAE,0XFF,0XF0,0XFF,0XF2,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,0XFF,0XAE,0XFF,0XAB,
0XFF,0X6A,0XFF,0X28,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,0XFF,0XAE,0XFF,0XF0,
0XFF,0XF2,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,0XFF,0XAE,0XFF,0XAB,0XFF,0X6A,0XFF,0X28,
0XFE,0XA6,0XFE,0X64,0XFD,0XE3,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE4,0XA4,
0XFD,0XE6,0XFC,0XE7,0XFB,0X86,0XFB,0X05,0XFB,0X05,0XFB,0XC7,0XFD,0X2B,0XFE,0XAF,
0XFF,0X30,0XFE,0XAE,0XFD,0X6A,0XFC,0X67,0XFB,0X45,0XFB,0X05,0XFB,0X85,0XFC,0X65,
0XFD,0X23,0XFD,0X61,0XC3,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,
0XFF,0X6A,0XFF,0XAB,0XFF,0XAE,0XF7,0X6F,0XEF,0X30,0XF7,0X72,0XFF,0XF2,0XFF,0XF0,
0XF7,0X6D,0XEE,0XEA,0XF6,0XE9,0XFF,0X28,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,
0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,0XFF,0X28,0XC4,0X23,
0XFE,0XE9,0XFF,0XAD,0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,
0XFF,0X28,0XE5,0XE5,0XC4,0X23,0XFE,0X24,0XFD,0XE2,0XF5,0X21,0XBC,0X25,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,0XFF,0XAD,
0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,
0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XF5,0X21,0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,
0XF6,0X25,0XFE,0XE7,0XBB,0X00,0XE6,0X33,0XF6,0XF8,0XF7,0X7B,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0X7B,0XF6,0XF8,0XE6,0X33,0XBB,0X00,0XFE,0X24,
0XFD,0XE2,0XF5,0X21,0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,0XFD,0X6A,0XFD,0X2D,
0XFC,0XF0,0XFC,0XF0,0XF5,0X2E,0XFE,0X6F,0XFF,0X2F,0XFF,0XAF,0XFF,0XAE,0XFF,0XFF,
0XC4,0X6B,0XFE,0XEB,0XFD,0XEC,0XF4,0XEC,0XFC,0XAE,0XFC,0X6D,0XFC,0XAB,0XF4,0XA7,
0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,0XFF,0X28,0XFF,0X6A,
0XFF,0XAB,0XFF,0XAD,0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,
0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XF5,0X21,0XBC,0X25,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,0XFF,0XAD,
0XCC,0X64,0XBB,0X81,0XC3,0XC2,0XD5,0X2A,0XFF,0XAB,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,
0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XF5,0X61,0XBB,0XC5,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X28,
0XFD,0XA5,0XFD,0X26,0XFC,0X25,0XFB,0X85,0XFB,0X45,0XFB,0XC6,0XFD,0X29,0XFE,0XAC,
0XFF,0X2D,0XFE,0XAB,0XFD,0XA8,0XFC,0XA6,0XFB,0XC5,0XFB,0X44,0XFB,0X84,0XFC,0X63,
0XFD,0X22,0XF4,0XE1,0XC4,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,
0XFF,0X28,0XFF,0X6A,0XF7,0X2B,0XD5,0XE9,0XFF,0XFF,0XD5,0XEA,0XF7,0X6E,0XF7,0X6C,
0XD5,0XA8,0XFF,0XFF,0XD5,0X66,0XF6,0XA6,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XF5,0X21,
0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,0XFE,0XE7,0XFF,0X6D,
0XBB,0X81,0XF6,0X67,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,
0XF6,0X67,0XAA,0XC0,0XFF,0X2C,0XFE,0X23,0XFD,0XA1,0XDC,0X61,0XC4,0XED,0XDE,0XFA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,0XFE,0XE7,0XFF,0X28,0XFF,0X6A,0XFF,0X6A,
0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,
0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XDC,0X61,0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,
0XE5,0X63,0XFE,0XA6,0XBB,0X00,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,
0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0XBB,0X00,0XFE,0X23,
0XFD,0XA1,0XDC,0X61,0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,0XED,0X26,0XFD,0XE9,
0XFD,0XEB,0XFE,0X2C,0XFE,0X6C,0XFE,0XEC,0XFF,0X6C,0XFF,0X6C,0XFF,0XAB,0XFF,0X6A,
0XFF,0X6A,0XFE,0XE9,0XFE,0X69,0XFD,0XE9,0XFD,0XA8,0XFD,0X67,0XFD,0X25,0XDC,0X63,
0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,0XFE,0XE7,0XFF,0X28,
0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XCC,0XA4,
0X81,0X80,0XED,0X64,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XDC,0X61,0XC4,0XED,0XDE,0XFA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,0XFE,0XE7,0XFF,0X28,0XFF,0X6A,0XFF,0X6A,
0XCC,0XA8,0XFF,0XBD,0XF7,0X3A,0XE6,0X33,0XCC,0XA8,0XFF,0X29,0XFE,0XE7,0XFE,0XA6,
0XFE,0X64,0XFE,0X23,0XFD,0XA2,0XDC,0X61,0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X31,
0XED,0X24,0XFD,0XA5,0XFC,0XE5,0XFC,0X65,0XFC,0X26,0XFC,0XA6,0XFD,0XA8,0XFE,0XAA,
0XFF,0X2A,0XFE,0XEA,0XFE,0X28,0XFD,0X26,0XFC,0X65,0XFC,0X24,0XFC,0X24,0XFC,0XA3,
0XFD,0X22,0XDC,0X61,0XC4,0XED,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,
0XFE,0XE7,0XFF,0X28,0XEE,0XA8,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X67,0XCD,0X67,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE5,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XDC,0X61,
0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,0XFE,0XA6,0XFE,0XE7,
0XFF,0X6D,0XBB,0X81,0XC3,0XC2,0XEE,0X27,0XFF,0X69,0XFF,0X29,0XEE,0X27,0XC3,0XC1,
0XAA,0XC0,0XFF,0X2C,0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XBB,0XC6,0XD6,0X37,0XEF,0X7C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,0XFE,0XE7,0XBA,0XC0,0XED,0XA6,0XED,0XA6,
0XBA,0XC0,0XBA,0XC0,0XED,0XA6,0XED,0XA6,0XBA,0XC0,0XBA,0XC0,0XED,0XA6,0XFE,0X64,
0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XBB,0XC6,0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,
0XCC,0X64,0XFE,0X65,0XFE,0XA6,0XBB,0X00,0X81,0X80,0X99,0XC0,0XEA,0XC0,0XFB,0X00,
0XFC,0XAB,0XFB,0X00,0XDA,0X80,0XBA,0X80,0XFF,0XBE,0XC3,0X84,0XFE,0X23,0XFD,0XE2,
0XF5,0X21,0XBB,0XC6,0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFD,0XE5,
0XFE,0X67,0XFE,0XA8,0XFE,0XE8,0XFF,0X29,0XFF,0X29,0XFF,0X69,0XFF,0X69,0XFF,0X29,
0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X25,0XFD,0XE4,0XFD,0XA3,0XF5,0X22,0XBB,0XC6,
0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,0XFE,0XA6,0XFE,0XE7,
0XFF,0X28,0XFF,0X29,0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XD4,0XE4,0X81,0X80,
0XFF,0X6B,0XFE,0X64,0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XBB,0XC6,0XD6,0X37,0XEF,0X7C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,0XFE,0XA6,0XFE,0XE7,0XFF,0X28,0XFF,0X69,
0XA2,0X80,0X89,0X80,0X89,0XC0,0X89,0XC0,0XAB,0X01,0XFF,0X28,0XFE,0XA6,0XFE,0X64,
0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XB3,0XC6,0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,
0XD4,0X64,0XFD,0XA4,0XFD,0XA5,0XFD,0X25,0XFD,0X26,0XFD,0X67,0XFE,0X28,0XFE,0XA9,
0XFE,0XE9,0XFE,0XE8,0XFE,0X67,0XFD,0XA6,0XFD,0X25,0XFC,0XA4,0XFC,0XA3,0XFC,0XE2,
0XF4,0XA1,0XBB,0XC6,0XD6,0X36,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,
0XFE,0XE7,0XFE,0XE7,0XF6,0XA7,0XD5,0X66,0XFF,0XFF,0XF7,0XBE,0XD5,0XF6,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XD5,0X24,0XF6,0X24,0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XBB,0XC6,
0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XE5,0X64,0XFE,0X64,0XFE,0XA5,
0XFE,0XA6,0XFF,0X2A,0XD5,0X27,0XBA,0XC0,0X8A,0X40,0X92,0X40,0XBA,0XC0,0XE5,0XE8,
0XFF,0X2A,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XDC,0XE3,0XFE,0X64,0XFF,0X6B,0XBA,0XC0,0XBA,0XC0,
0XFF,0X6B,0XFF,0X6B,0XBA,0XC0,0XBA,0XC0,0XFF,0X6B,0XFF,0X6B,0XBA,0XC0,0XFE,0X23,
0XFD,0XE2,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XE6,0X31,0XDC,0XE3,0XFE,0XA5,0XFE,0XA5,0XC3,0X83,0XDD,0X6D,0XED,0XF2,0XFE,0X75,
0XFF,0X7B,0XFF,0XBD,0XFF,0XFE,0XFF,0XFF,0XFF,0XFF,0XE5,0XF2,0XE4,0XE3,0XFD,0XA1,
0XAB,0X01,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XDC,0XE3,
0XFE,0X64,0XFE,0XA5,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,
0XFE,0XA6,0XFE,0XA5,0XFE,0X64,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XAB,0X02,0XC4,0XAC,
0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XE5,0X64,0XFE,0X64,0XFE,0XA5,
0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XB2,0XC0,0X81,0X80,0X81,0X80,0XA2,0X40,0XFF,0X6B,
0XFE,0X64,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XE5,0XF1,0XDC,0XE3,0XFE,0X64,0XFE,0XA5,0XFE,0XA6,0XFF,0X28,
0XBB,0X81,0X99,0XC0,0XEA,0XC0,0XA1,0XC0,0X9A,0X40,0XFE,0XE7,0XFE,0X64,0XFE,0X23,
0XFD,0XE2,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XE6,0X31,0XE5,0X64,0XFD,0XE4,0XFD,0XE5,0XFD,0XE6,0XFE,0X26,0XE5,0X24,0XFE,0XA7,
0XFE,0XE7,0XE5,0X64,0XBB,0X41,0XFE,0X27,0XFD,0XA4,0XFD,0X63,0XFD,0X22,0XFD,0X21,
0XB3,0X02,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XDC,0XE3,
0XFE,0X64,0XFE,0XA6,0XFE,0XA6,0XF6,0XA6,0XCC,0XE5,0XD5,0XF6,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCC,0XA3,0XF6,0X24,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,
0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,0XE5,0X24,0XFE,0X24,
0XFE,0X64,0XFE,0X65,0XFF,0X29,0XFF,0X2A,0XFF,0X6B,0XFF,0X6B,0XFF,0X2A,0XFF,0X2A,
0XFE,0X23,0XFD,0XE2,0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,0XDC,0XA2,0XFE,0X24,0XFF,0X6B,0XFF,0X6B,
0XFE,0XA5,0XFE,0XA5,0XFF,0X6B,0XFF,0X6B,0XFE,0X64,0XFE,0X24,0XFE,0X23,0XFD,0XE2,
0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCC,0X67,0XDC,0XA2,0XFE,0X65,0XFE,0X64,0XDC,0X62,0XCB,0XC1,0XBB,0X00,
0XBB,0X00,0XBB,0X00,0XC3,0X41,0XFF,0XFF,0XFF,0XFF,0XEE,0X75,0XD5,0X2D,0XCB,0XC3,
0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,
0XDC,0XA2,0XFE,0X24,0XFE,0X64,0XFE,0X65,0XFE,0XA5,0XFE,0XE7,0XA2,0X80,0XA2,0X80,
0XFE,0XA6,0XFE,0X24,0XFE,0X23,0XFD,0XE2,0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,
0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,0XE5,0X24,0XFE,0X24,
0XFE,0X64,0XFE,0X65,0XFE,0XA5,0XFF,0X6B,0XFF,0X6B,0XFF,0X6B,0XFF,0X6B,0XFE,0X24,
0XFE,0X23,0XFD,0XE2,0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0XA7,0XDC,0XA2,0XFE,0X24,0XFE,0X64,0XFE,0XE7,
0XDD,0X23,0XC3,0X40,0XD2,0X80,0XEA,0XC0,0XBA,0XC0,0XFE,0XA6,0XFE,0X23,0XFD,0XE2,
0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCC,0XA7,0XE5,0X24,0XFD,0XE4,0XFE,0X24,0XFE,0XA8,0XCB,0XC2,0XA2,0X40,
0XA2,0X40,0XBB,0X01,0XFE,0XE8,0XFD,0XE4,0XFD,0XA3,0XFD,0X62,0XFD,0X21,0XD3,0XC1,
0XBC,0X28,0XDE,0XB9,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,
0XDC,0XA2,0XFE,0X24,0XFE,0XA6,0XF6,0X25,0XC4,0X63,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XD5,0XF6,0XC4,0X22,0XF5,0XA3,0XFD,0XE2,0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,
0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XDD,0X23,
0XF5,0XA2,0XFE,0X23,0XFE,0X23,0XFE,0X24,0XFE,0X23,0XFE,0X23,0XFE,0X23,0XFD,0XE2,
0XFD,0XA1,0XE4,0XE1,0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XD4,0X62,0XF5,0XA2,0XFE,0X65,
0XFE,0X23,0XFE,0X24,0XFE,0X23,0XFE,0X23,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XE4,0XE1,
0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XD4,0X62,0XF5,0XE4,0XFE,0X23,0XFE,0X23,0XFE,0X24,
0XFE,0X23,0XFE,0X23,0XFE,0X23,0XE4,0XE3,0XFF,0XFF,0XFF,0XFF,0XD5,0X2D,0XC3,0XC6,
0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XC4,0X67,0XD4,0X62,0XF5,0XA2,0XFE,0X23,0XFE,0X23,0XFF,0X6B,0XA2,0X80,0XA2,0X80,
0XFF,0X6B,0XFD,0XE2,0XFD,0XA1,0XE4,0XE1,0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XDD,0X23,
0XF5,0XA2,0XFE,0X23,0XFE,0X23,0XFE,0X24,0XFE,0X23,0XFE,0X23,0XFE,0X23,0XFD,0XE2,
0XFD,0XA1,0XE4,0XE1,0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XD4,0X62,0XF5,0XA2,0XFE,0X23,
0XFE,0XE7,0XED,0X63,0XBB,0X80,0XAA,0XC0,0XDC,0X61,0XFE,0XA6,0XFD,0XA2,0XE4,0XA1,
0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XCC,0X68,0XE5,0X24,0XF5,0XA3,0XFE,0X23,0XFE,0XE8,0XDC,0XA2,
0XDC,0XA2,0XFE,0XE8,0XFD,0XE3,0XFD,0XE2,0XFD,0XA1,0XE4,0XA1,0XB3,0X02,0XBC,0X6A,
0XDE,0X79,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XC4,0X67,0XD4,0X62,0XED,0X62,0XD4,0XE3,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XD5,0XF6,
0XF7,0XBE,0XFF,0XFF,0XD4,0X61,0XDC,0XA1,0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,
0XC4,0X64,0XCC,0X21,0XE4,0XE2,0XF5,0X62,0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XA1,
0XCB,0XC1,0XAB,0X85,0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,0XB3,0X43,0XCC,0X21,
0XE4,0XE2,0XF5,0X62,0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XA1,0XCB,0XC1,0XAB,0X85,
0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,0XB3,0X43,0XCC,0X21,0XE4,0XE2,0XF5,0X62,
0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XE3,0XFF,0XFF,0XFF,0XFF,0XC3,0XC6,0XDE,0XB9,
0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCD,0X70,0XB3,0X43,0XCC,0X21,0XE4,0XE2,0XF5,0XE4,0XFF,0X6B,0XFF,0X6B,
0XF5,0XE4,0XE4,0XA1,0XCB,0XC1,0XAB,0X85,0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,
0XC4,0X64,0XCC,0X21,0XE4,0XE2,0XF5,0X62,0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XA1,
0XCB,0XC1,0XAB,0X85,0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,0XB3,0X43,0XCC,0X21,
0XE4,0XE2,0XFE,0XA7,0XFE,0XA7,0XFE,0XA6,0XFE,0X25,0XE4,0XA1,0XCB,0XC1,0XAB,0X85,
0XCD,0X72,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,0XC4,0X65,0XD4,0X62,0XF5,0X62,0XFE,0XE8,
0XFE,0XE8,0XFD,0XA2,0XF5,0X61,0XE4,0XE1,0XCC,0X21,0XB3,0X85,0XCD,0XB2,0XDE,0XB9,
0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCD,0X70,0XA3,0X02,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XAA,0XC0,0XAA,0XC0,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XA3,0X04,0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XEF,0X3B,0XCD,0X70,0XBC,0X28,0XB3,0X85,0XAA,0XC1,0XAB,0X01,0XAB,0X44,0XB4,0X29,
0XBC,0XAC,0XD6,0XB9,0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XCD,0X70,
0XBC,0X28,0XB3,0X85,0XAA,0XC1,0XAB,0X01,0XAB,0X44,0XB4,0X29,0XBC,0XAC,0XD6,0XB9,
0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XCD,0X70,0XBC,0X28,0XB3,0X85,
0XAA,0XC1,0XAB,0X01,0XAB,0X44,0XE4,0XE3,0XFF,0XFF,0XD5,0X2D,0XE6,0X76,0XF7,0XBE,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XCD,0X70,0XBC,0X28,0XB3,0X85,0XAA,0XC1,0XAB,0X01,
0XAB,0X44,0XB4,0X29,0XBC,0XAC,0XD6,0XB9,0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XEF,0X3B,0XCD,0X70,0XBC,0X28,0XB3,0X85,0XAA,0XC1,0XAB,0X01,0XAB,0X44,0XB4,0X29,
0XBC,0XAC,0XD6,0XB9,0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XCD,0X70,
0XB4,0X28,0XB3,0X85,0XAA,0XC1,0XA3,0X02,0XAB,0X44,0XB4,0X28,0XBC,0XAC,0XD6,0XB9,
0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XCD,0X2F,0XB3,0X85,0XDC,0X61,
0XDC,0X61,0XB3,0X82,0XAB,0X44,0XB4,0X28,0XBC,0XAC,0XD6,0XB9,0XDE,0XFA,0XF7,0XBE,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XE6,0XB9,0XA3,0XCB,0XFF,0XFF,0X92,0X83,0XA2,0XC1,0XA2,0XC1,
0X8A,0X83,0XFF,0XFF,0X9B,0X48,0XCE,0X38,0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XE6,0XFB,0XDE,0XFA,0XD6,0XB9,0XDE,0XB9,0XDE,0XBA,
0XE7,0X3B,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XF7,0X7D,0XE6,0XFB,0XDE,0XFA,0XD6,0XB9,0XDE,0XB9,0XDE,0XBA,0XE7,0X3B,0XEF,0X3C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XE6,0XFB,
0XDE,0XFA,0XD6,0XB9,0XDE,0XB9,0XDE,0X34,0XDD,0X2E,0XDD,0XB1,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XE6,0XFB,0XDE,0XFA,0XD6,0XB9,
0XDE,0XB9,0XDE,0XBA,0XE7,0X3B,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XE6,0XFB,0XDE,0XFA,0XD6,0XB9,0XDE,0XB9,0XDE,0XBA,
0XE7,0X3B,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XF7,0X7D,0XE6,0XFB,0XDE,0XFA,0XD6,0XB9,0XDE,0XB9,0XDE,0XBA,0XE7,0X3B,0XEF,0X3C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0XFA,0XBC,0XAC,
0XBC,0XAC,0XCD,0XB2,0XCD,0XF5,0XD6,0X77,0XE6,0XFB,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XDE,0XBA,0XDE,0XB9,0XDE,0XFA,0XD6,0XB9,
0XD6,0X78,0XCE,0X37,0XDE,0XBA,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XEF,0X7C,0XEF,0X3C,0XF7,0XBE,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XD6,0X37,0X59,0X00,0X59,0X00,0XD6,0X37,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XD6,0X37,0X59,0X00,0X59,0X00,0XD6,0X37,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,
0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,
0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,
0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,
0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,
0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X35,
0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,
0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0X59,0X00,0XD6,0X36,0XD5,0XB0,0X59,0X00,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,
0XF6,0X29,0X59,0X00,0XD5,0X6E,0XD5,0XF3,0X59,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,
0XF5,0XE6,0XFE,0X25,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0X65,
0XF5,0XE4,0XED,0XEB,0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X66,
0XFE,0XE8,0XFF,0X28,0XFF,0X28,0XFE,0XE7,0XFE,0XA7,0XFE,0X66,0XFE,0X26,0XF6,0X2B,
0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X25,0XFE,0XA6,0XFE,0XE7,
0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0X65,0XF5,0XE4,0XED,0XEB,0XF6,0XF6,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X66,0XFE,0XE8,0XFF,0X28,0XFF,0X28,0XFE,0XE7,
0XFE,0XA7,0XFE,0X66,0XFE,0X26,0XF6,0X2B,0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,
0XF5,0XE6,0XFE,0X66,0XD5,0X66,0X82,0X42,0XFF,0X28,0XE6,0X26,0XFE,0XA7,0XFE,0X66,
0XFE,0X26,0XF6,0X2B,0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XFE,0X27,0XFE,0XA7,
0XFE,0XE8,0XFE,0XE8,0XFF,0X28,0XFE,0XE7,0XFE,0XA7,0XFE,0X66,0XFE,0X26,0XF6,0X2B,
0XFF,0X36,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X25,0XFE,0XA6,0XFE,0XE7,
0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0X65,0XF5,0XE4,0XED,0XEB,0XF6,0XF6,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X25,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,
0XFE,0XA6,0XFE,0X65,0XF5,0XE4,0XED,0XEB,0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,
0XFE,0XE8,0XFF,0X69,0XFF,0X6A,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE8,
0XFE,0XE7,0XFE,0X24,0XED,0X65,0XED,0XEA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,0XFF,0X29,0XFF,0X69,
0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X66,
0XED,0XE6,0XEE,0X2A,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,0XFE,0XE8,0XFF,0X69,0XFF,0X6A,0XFF,0X69,
0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE8,0XFE,0XE7,0XFE,0X24,0XED,0X65,0XED,0XEA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,
0XF6,0X6B,0XFE,0X26,0XFF,0X29,0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X29,
0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X66,0XED,0XE6,0XEE,0X2A,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,
0X82,0X42,0X82,0X42,0X82,0X42,0XD5,0XA7,0XD6,0XBA,0XDD,0XE7,0X82,0X42,0XFE,0XE7,
0X82,0X42,0XD5,0X24,0XED,0XE6,0XEE,0X2A,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X6B,0XFF,0X29,0XFF,0X28,0XFF,0X29,
0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE7,0XFE,0XE7,0XFE,0X66,
0XED,0XE6,0XED,0XEA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,0XFE,0XE8,0XFF,0X69,0XFF,0X6A,0XFF,0X69,
0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE8,0XFE,0XE7,0XFE,0X24,0XED,0X65,0XED,0XEA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,
0XF6,0X6B,0XED,0X64,0XD4,0X62,0XCB,0XC2,0XD4,0X63,0XEE,0X67,0XFF,0X69,0XFF,0X29,
0XEE,0X26,0XD4,0X62,0XCB,0XC2,0XD4,0X22,0XDC,0XE4,0XED,0XEA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,0XFE,0X26,0XFF,0X6A,
0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,
0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XF5,0XA3,0XE5,0XAA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,0XFE,0X26,0XFF,0X6A,0XFF,0X6A,0XFF,0X6A,
0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,
0XFE,0X64,0XF5,0XA3,0XEE,0X2A,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XF6,0X2B,0XFE,0X26,0XFF,0X6A,0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,
0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XF5,0XA3,
0XE5,0XAA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,
0XFE,0X26,0XFF,0X6A,0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,
0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X64,0XF5,0XA3,0XEE,0X2A,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,0XED,0XA5,0XDC,0XA4,
0XCC,0X22,0XDC,0XA4,0XF6,0XA9,0XFF,0XAB,0XD5,0XAE,0XFF,0X6A,0XDE,0X27,0XFF,0X28,
0XD5,0X65,0X82,0X42,0X82,0X42,0X82,0X42,0XEE,0X2A,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,0XFF,0X29,0XFF,0X28,0XFF,0X6A,0XFF,0X6A,
0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,
0XFE,0X64,0XF6,0X26,0XED,0XEB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XF6,0X2B,0XFE,0X26,0XFF,0X6A,0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,
0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XF5,0XA3,
0XE5,0XAA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,
0XFE,0X26,0XD4,0X63,0X59,0X00,0X79,0X40,0XF7,0X3A,0XD4,0X63,0XFF,0XAB,0XFF,0X6A,
0XD4,0X63,0X59,0X00,0X79,0X40,0XF7,0X3A,0XD4,0X62,0XF5,0XA3,0XE5,0XAA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XF6,0X25,0XFF,0X6A,0XEE,0X67,
0XD4,0X63,0XCB,0XC2,0XD4,0X64,0XEE,0XAB,0XFF,0XAE,0XFF,0XAD,0XEE,0X69,0XD4,0X63,
0XCB,0XC2,0XD4,0X62,0XED,0XA4,0XFE,0XA6,0XE5,0X24,0XDD,0XED,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XF6,0X25,0XFF,0X69,0XE5,0XA6,0XD5,0X26,0XFF,0XAD,
0XFF,0XAE,0XFF,0XEF,0XE5,0XE9,0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,0XD4,0XE4,0XE5,0X64,
0XFE,0XA5,0XFE,0X24,0XE5,0X24,0XE6,0X2D,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFE,0XF3,0XF6,0X25,0XFF,0X6A,0XE5,0XE7,0X81,0X80,0X81,0X80,0XB4,0X26,0XFF,0XEF,
0XFF,0XAE,0XFF,0XAD,0XFF,0XAB,0XB3,0XC4,0X81,0X80,0X81,0X80,0XE5,0X64,0XFE,0XA6,
0XE5,0X24,0XDD,0XED,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XF6,0X25,
0XFF,0X69,0XAB,0X83,0X81,0X80,0XE6,0X2A,0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,
0XFF,0XAB,0XE5,0XE7,0X81,0X80,0XAB,0X42,0XFE,0XA5,0XFE,0X24,0XE5,0X24,0XE6,0X2D,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XED,0X64,0XD4,0XEA,0XEE,0XB6,
0XFF,0XFF,0XEE,0XB6,0XD4,0XEA,0XEE,0X6B,0XFF,0XAE,0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,
0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XE5,0X24,0XE6,0X2D,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XFE,0XE8,0XFF,0X28,0XEE,0X68,0X89,0X80,0X89,0X80,
0XEE,0XAC,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,0XE6,0X69,0X89,0X80,0X89,0X80,0XE5,0XE5,
0XFE,0XA5,0XFE,0X24,0XE5,0XA5,0XEE,0X2F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFE,0XF3,0XF6,0X25,0XFF,0X6A,0XEE,0X68,0X79,0X40,0X79,0X40,0XEE,0XAC,0XFF,0XEF,
0XFF,0XAE,0XFF,0XAD,0XE6,0X69,0X79,0X40,0X79,0X40,0XE5,0XA5,0XFE,0XA5,0XFE,0XA6,
0XE5,0X24,0XDD,0XED,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XF6,0X25,
0XFF,0X6A,0XCB,0XC2,0X79,0X40,0XFF,0XFF,0XFF,0XFF,0XCB,0XC2,0XFF,0XAE,0XFF,0XAD,
0XCB,0XC2,0X79,0X40,0XFF,0XFF,0XFF,0XFF,0XCB,0XC2,0XFE,0XA6,0XE5,0X24,0XDD,0XED,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XFF,0X29,0XFF,0X6A,0XCC,0X23,
0XE6,0X31,0XFF,0XFF,0XE6,0X31,0XCC,0X24,0XFF,0XF2,0XFF,0XF0,0XD4,0X63,0XE6,0X31,
0XFF,0XFF,0XEE,0XB5,0XD4,0X22,0XFE,0X64,0XFE,0X24,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XFE,0XE8,0XFF,0X6A,0XFF,0XAB,0XBB,0X43,0XD5,0X28,
0XFF,0XF2,0XFF,0XF3,0XC4,0X66,0XFF,0XF0,0XFF,0XAE,0XD5,0X26,0XBB,0X42,0XFF,0X28,
0XFE,0XA6,0XFE,0X64,0XFD,0XE3,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,
0XED,0XA6,0XFF,0X29,0XCD,0X26,0X81,0X80,0XE6,0X2A,0XFF,0XF0,0XFF,0XF2,0XFF,0XF3,
0XFF,0XF2,0XFF,0XF0,0XFF,0XAE,0XFF,0XAB,0XFF,0X6A,0XE5,0XA6,0X81,0X80,0XCC,0X62,
0XFE,0X24,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XFE,0XE8,
0XFF,0X6A,0XFF,0XAB,0XE6,0X2A,0X81,0X80,0X81,0X80,0XE6,0X2E,0XFF,0XF2,0XE6,0X2C,
0X81,0X80,0X81,0X80,0XE5,0XE7,0XFF,0X28,0XFE,0XA6,0XFE,0X64,0XFD,0XE3,0XDD,0X28,
0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XCC,0X23,0XEE,0XB7,0XFF,0XFF,
0X81,0X80,0X81,0X80,0XEE,0XF8,0XCC,0X24,0XFF,0XF2,0XFF,0XF0,0XEE,0XAB,0XCC,0X23,
0XC3,0XC3,0XC3,0XC2,0XD4,0XA3,0XF5,0XE4,0XFD,0XE3,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XF6,0X27,0XFE,0XE7,0XFF,0X6A,0X89,0X80,0XE6,0X6B,0XE6,0XAD,
0X89,0X80,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,0X89,0X80,0XE6,0X69,0XE6,0X27,0X89,0X80,
0XFE,0XA6,0XFE,0X64,0XFD,0XE3,0XE5,0X69,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,
0XED,0XA6,0XFF,0X29,0XFF,0X6A,0X79,0X40,0XE6,0X6B,0XE6,0X6D,0X79,0X40,0XFF,0XF3,
0XFF,0XF2,0XFF,0XF0,0X79,0X40,0XE6,0X69,0XE6,0X27,0X79,0X40,0XFE,0XA6,0XFE,0X64,
0XFE,0X24,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XFF,0X29,
0XFF,0X6A,0XCB,0XC2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCB,0XC2,0XFF,0XF2,0XFF,0XF0,
0XCB,0XC2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCB,0XC2,0XFE,0X64,0XFE,0X24,0XDD,0X28,
0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,0XF6,0X66,0XFF,0X29,0XFF,0X6A,0XC3,0X82,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC3,0X82,0XFF,0XF6,0XFF,0XF4,0XC3,0X82,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XC3,0X82,0XFE,0X65,0XFE,0X23,0XED,0X22,0XDD,0XAD,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XF6,0XF3,0X08,0X40,0X00,0X00,0X21,0X01,0X73,0X86,0XCE,0X2D,0XD5,0X2A,
0XAA,0X80,0XE6,0X2D,0XB3,0X43,0XEE,0XF0,0XAA,0X80,0XD5,0X27,0XD6,0X29,0X7B,0X44,
0X31,0X41,0X00,0X00,0X00,0X00,0XBC,0X22,0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,
0XF6,0X66,0XFF,0X29,0XFF,0X6A,0XF6,0XEB,0XDC,0XE5,0XD4,0X22,0XDC,0XE6,0XF7,0X32,
0XFF,0XF6,0XFF,0XF4,0XF7,0X2E,0XDC,0XE4,0XD4,0X22,0XDC,0XE3,0XF6,0X66,0XFE,0X65,
0XFE,0X23,0XED,0X22,0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,0XF6,0X25,0XFF,0X29,
0XFF,0X6A,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0XFF,0XF6,0X81,0X80,
0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XED,0X22,
0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,0XF6,0X25,0XB3,0X01,0XFF,0XFF,0XFF,0XFF,
0X81,0X80,0X59,0X00,0XFF,0XFF,0XB3,0X01,0XFF,0XF0,0XEE,0XAB,0XBB,0X41,0XEE,0XB6,
0X81,0X80,0X81,0X80,0XE6,0X33,0XBB,0X41,0XE5,0X22,0XED,0X22,0XDD,0XAD,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XF6,0XF3,0XF6,0X66,0XFF,0X29,0XFF,0X6A,0XB3,0XC4,0XFF,0XF0,0XFF,0XF4,
0XB3,0XC8,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,0XB3,0XC6,0XFF,0XAD,0XFF,0X6A,0XB3,0X83,
0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XED,0X22,0XE6,0X2F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,
0XF6,0X27,0XFE,0XE9,0XFF,0X2B,0XAB,0X85,0XFF,0XB1,0XFF,0XB4,0XAB,0X88,0XFF,0XF7,
0XFF,0XF6,0XFF,0XF4,0XAB,0X86,0XFF,0XAD,0XFF,0X2B,0XAB,0X44,0XFE,0XA8,0XFE,0X66,
0XFE,0X24,0XED,0X22,0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,0XF6,0X66,0XFF,0X29,
0XFF,0X6A,0XD4,0X63,0XF7,0X3A,0XFF,0XFF,0XF7,0X3A,0XD4,0X65,0XFF,0XF6,0XFF,0XF4,
0XD4,0X64,0XF7,0X3A,0XFF,0XFF,0XF7,0X3A,0XD4,0X62,0XFE,0X65,0XFE,0X23,0XED,0X22,
0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XC3,0X82,
0XAB,0XCB,0X79,0X40,0XAB,0XCB,0XC3,0X82,0XFF,0XF8,0XFF,0XF6,0XC3,0X82,0XAB,0XCB,
0X79,0X40,0XAB,0XCB,0XC3,0X82,0XFE,0XA5,0XFE,0X23,0XFD,0XA2,0XD4,0XA6,0XEF,0X3C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XED,0XE9,0X18,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X6B,0X49,
0XD5,0X2A,0XAA,0X80,0XFF,0XF8,0XAA,0X80,0XD5,0X28,0X73,0X46,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0XD4,0XE2,0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,
0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XCC,0X23,0XF7,0X3A,0X71,0X40,0X61,0X00,0XCC,0X23,
0XFF,0XF8,0XFF,0XF6,0XCC,0X23,0XF7,0X3A,0X71,0X40,0X61,0X00,0XCC,0X23,0XFE,0XA5,
0XFE,0X23,0XFD,0XA2,0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0XFE,0XA7,0XFF,0X69,
0XFF,0XAB,0XFF,0XAE,0XFF,0XF2,0XFF,0XF6,0XFF,0XF8,0XFF,0XFA,0XFF,0XF8,0XFF,0XF6,
0XFF,0XF2,0XFF,0XAE,0XFF,0XAB,0XFF,0X69,0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XA2,
0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0XFE,0XA7,0XBB,0X82,0XE6,0XB7,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XEE,0XB7,0XB3,0X84,0XDF,0X71,0XBC,0X25,0XE6,0X76,0XFF,0XFF,
0X59,0X00,0X81,0X80,0XFF,0XFF,0XEE,0XF9,0XB2,0XC1,0XF5,0X62,0XD4,0XA6,0XEF,0X3C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XED,0XE9,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XFF,0XAE,0XFF,0XF2,0XFF,0XF6,
0XFF,0XF8,0XFF,0XFA,0XFF,0XF8,0XFF,0XF6,0XFF,0XF2,0XFF,0XAE,0XFF,0XAB,0XFF,0X69,
0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XA2,0XD4,0XE7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,
0XFE,0X29,0XFE,0X6B,0XFE,0X2E,0XFE,0X70,0XFE,0XB2,0XFF,0X35,0XFF,0XB8,0XFF,0XFA,
0XFF,0XF8,0XFF,0XF6,0XFF,0XB2,0XFF,0X2F,0XFE,0X6D,0XFE,0X2C,0XFD,0XEB,0XFD,0XA8,
0XFD,0XA6,0XFD,0XA2,0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0XFE,0XE7,0XFF,0X69,
0XFF,0XAB,0XEE,0XAB,0XD4,0XE6,0XCC,0X64,0XD4,0XE8,0XEE,0XB3,0XFF,0XF8,0XFF,0XF6,
0XEE,0XAD,0XD4,0XE5,0XD4,0X63,0XD4,0XE4,0XEE,0X25,0XFE,0XA5,0XFE,0X23,0XFD,0XA2,
0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFF,0X28,0XFF,0X69,0XFF,0XAB,0XCC,0X23,
0X79,0X40,0X59,0X00,0X79,0X40,0XCC,0X65,0XFF,0XFA,0XFF,0XF7,0XD4,0X64,0X79,0X40,
0X59,0X00,0X79,0X40,0XD4,0X62,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XCC,0X63,0XDE,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE5,0X24,0X39,0X81,0X00,0X00,0X00,0X00,0X00,0X00,0XBC,0XAE,0X8B,0X08,
0X52,0X88,0XFF,0XFA,0XFF,0XFA,0XFF,0XF7,0X5A,0XC6,0X00,0X00,0X8B,0X08,0XBC,0XAE,
0X00,0X00,0X00,0X00,0X00,0X00,0XF5,0XA2,0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,
0XFF,0X28,0XFF,0X69,0XFF,0XAB,0XB3,0X01,0XFF,0XFF,0X71,0X40,0X71,0X40,0XB3,0X01,
0XFF,0XFA,0XFF,0XF7,0XB3,0X01,0XFF,0XFF,0X71,0X40,0X71,0X40,0XB3,0X01,0XFE,0XA5,
0XFE,0X24,0XFD,0XA2,0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,
0XFF,0XAB,0XFF,0XEF,0XFF,0XF3,0XFF,0XF7,0XFF,0XFA,0XFF,0XFA,0XFF,0XFA,0XFF,0XF7,
0XFF,0XF3,0XFF,0XEF,0XFF,0XAB,0XFF,0X69,0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,
0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XE5,0XE6,0XBC,0X2A,0XE6,0X77,
0XFF,0XFF,0XE6,0X77,0XBC,0X2A,0XA5,0X2D,0XA6,0X73,0XA4,0X28,0XCC,0XEE,0XFF,0XBE,
0XFF,0XFF,0XFF,0XFF,0XF7,0X3B,0XCC,0XEE,0XB3,0X01,0XFD,0XA2,0XCC,0X63,0XDE,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XFF,0XEF,0XFF,0XF3,0XFF,0XF7,
0XFF,0XFA,0XFF,0XFA,0XFF,0XFA,0XCC,0X22,0XCB,0XC1,0XCB,0XC1,0XEE,0X68,0XFF,0X69,
0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XCC,0X63,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,
0XFD,0XAC,0XFD,0X2E,0XFC,0XF0,0XFC,0XF2,0XFD,0X73,0XFE,0X76,0XFF,0X79,0XFF,0XFA,
0XFF,0XFA,0XFF,0XB7,0XFF,0X33,0XFE,0X30,0XFD,0X2F,0XFC,0XAF,0XFC,0XAF,0XFC,0XEC,
0XFD,0X29,0XFD,0XE2,0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFF,0X28,0XFF,0X69,
0XFF,0XAB,0XFF,0XEF,0XFF,0XF3,0XFF,0XF7,0XFF,0XFA,0XFF,0XFA,0XFF,0XFA,0XFF,0XF7,
0XFF,0XF3,0XFF,0XEF,0XFF,0XAB,0XFF,0X69,0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,
0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XEE,0XAB,
0XB3,0X83,0X8A,0X41,0XB3,0X84,0XEE,0XB3,0XFF,0XF8,0XFF,0XF6,0XEE,0XAD,0XB3,0X83,
0X8A,0X41,0XB3,0X42,0XEE,0X25,0XFE,0XA5,0XFE,0X23,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE5,0X24,0X83,0X83,0X00,0X00,0X00,0X00,0X00,0X00,0XBC,0XAE,0XFF,0XFF,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XBC,0XAE,
0X00,0X00,0X00,0X00,0X49,0XC1,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,
0XFE,0XE7,0XFF,0X29,0XFF,0X2B,0XB3,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB3,0X01,
0XFF,0XF8,0XFF,0XF6,0XB3,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB3,0X01,0XFE,0X66,
0XFE,0X24,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,
0XDD,0X66,0XA2,0X80,0XA2,0X80,0XAB,0X02,0XDE,0X30,0XFF,0XFA,0XFF,0XF8,0XFF,0XF6,
0XDE,0X2C,0XAB,0X01,0XA2,0X80,0XA2,0X80,0XDD,0X24,0XFE,0XA5,0XFE,0X23,0XFD,0XE2,
0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XDE,0X26,0XB3,0X42,
0X9A,0X00,0XA3,0X04,0XA5,0X2C,0XA6,0X33,0XA6,0X33,0XA6,0X32,0XA4,0X28,0X9A,0X41,
0XA2,0X83,0X9A,0X00,0XAB,0X01,0XD4,0XE4,0XFE,0X23,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XFF,0XAE,0XFF,0XF2,0XD4,0X22,
0XD4,0X22,0XF6,0XF3,0XCB,0XC2,0XFF,0XB0,0XFF,0XAF,0XEE,0X69,0XCC,0X63,0XF6,0XA7,
0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XE2,0XC4,0X22,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,
0XFD,0X6C,0XFC,0X70,0XFC,0X31,0XFC,0X32,0XFC,0XB3,0XFD,0XF5,0XFF,0X37,0XFF,0XB9,
0XFF,0XF8,0XFF,0XB6,0XFE,0XF2,0XFD,0XB1,0XFC,0XB0,0XFC,0X31,0XFB,0XD0,0XFC,0X2E,
0XFC,0XEA,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,
0XFF,0XAB,0XFF,0XAE,0XFF,0XF2,0XFF,0XF6,0XFF,0XF8,0XFF,0XFA,0XFF,0XF8,0XFF,0XF6,
0XFF,0XF2,0XFF,0XAE,0XFF,0XAB,0XFF,0X69,0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XE2,
0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,0XFF,0X6A,0XFF,0XAD,
0XFF,0XF0,0XFF,0XF4,0XFF,0XF6,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,0XFF,0XF0,0XFF,0XAD,
0XFF,0X6A,0XFF,0X29,0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE3,0XF6,0XA6,0X31,0X41,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X6B,0X49,0XFF,0XF6,0X6B,0X48,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X18,0X80,0XDD,0X23,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,
0XFE,0XA7,0XFE,0X69,0XFE,0X2B,0XCC,0X23,0XEE,0XB5,0XFF,0XFF,0XEE,0XB5,0XCC,0X23,
0XFF,0XF6,0XFF,0XF3,0XCC,0X23,0XEE,0XB5,0XFF,0XFF,0XEE,0XB5,0XCC,0X23,0XFD,0X66,
0XFD,0XA4,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XDC,0XE4,
0XA2,0X80,0XF6,0XF8,0XF6,0XF8,0XD5,0X70,0XA2,0XC1,0XA2,0X80,0XA2,0X80,0XA2,0X80,
0XA2,0XC1,0XD5,0X70,0XF6,0XF8,0XF6,0XF8,0XA2,0X80,0XDC,0XE4,0XFE,0X23,0XFD,0XA2,
0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,0XFF,0XAA,0XFF,0XEA,
0XF7,0XAB,0XDF,0X2E,0XCE,0XAF,0XC6,0X70,0XBE,0X70,0XBE,0X70,0XC6,0XAF,0XD6,0XED,
0XE6,0XEA,0XFF,0XA9,0XFF,0X68,0XFE,0XA5,0XFE,0X23,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,0XFF,0X6A,0XFF,0XAD,0XD4,0X22,0XFF,0XAF,
0XE5,0XA7,0XCB,0XC1,0XFF,0XAF,0XFF,0XAE,0XFF,0X6C,0XFF,0X2A,0XED,0XE6,0XCC,0X22,
0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XFD,0XA2,0XC3,0XC2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,
0XFD,0XAB,0XFD,0X2E,0XFC,0XB0,0XFC,0XF1,0XFD,0X72,0XFE,0X74,0XFF,0X76,0XFF,0XF7,
0XFF,0XF6,0XFF,0XB4,0XFF,0X31,0XFE,0X2F,0XFD,0X2F,0XFC,0XAF,0XFC,0X6E,0XFC,0XAC,
0XFD,0X29,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,
0XFF,0X6A,0XFF,0XAD,0XFF,0XF0,0XFF,0XF4,0XFF,0XF6,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,
0XFF,0XF0,0XFF,0XAD,0XFF,0X6A,0XFF,0X29,0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XFD,0XA2,
0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,
0XFF,0XAE,0XFF,0XF0,0XFF,0XF2,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,0XFF,0XAE,0XFF,0XAB,
0XFF,0X6A,0XFF,0X28,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFE,0XE8,0X9C,0XA6,0X63,0X04,0X63,0X05,0X94,0XA9,
0XDE,0XF0,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,0XE6,0XEC,0X9C,0XA7,0X62,0XC4,0X62,0XC3,
0X8B,0X83,0XF5,0XE4,0XFE,0X23,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,
0XFD,0XE7,0XFD,0X69,0XFC,0XAB,0XF4,0X2A,0XB3,0X42,0X9A,0X00,0XB3,0X42,0XF6,0X6F,
0XFF,0XF2,0XFF,0XB0,0XF6,0X6C,0XB3,0X42,0X9A,0X00,0XB3,0X42,0XF3,0X88,0XFB,0XC8,
0XFC,0XA6,0XFD,0X23,0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XA2,0X80,
0XEE,0XB8,0XF7,0X3A,0XF7,0X3A,0XFF,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0X7C,0XF7,0X3A,0XF7,0X3A,0XEE,0XB8,0XA2,0X80,0XFE,0X23,0XFD,0XA1,
0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,
0XFF,0XED,0XFF,0XED,0XFF,0XEE,0XFF,0XAD,0XF7,0XAD,0XF7,0XAC,0XFF,0XAC,0XFF,0XAA,
0XFF,0X69,0XFF,0X28,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,0XD4,0X22,0XD4,0X22,0XF6,0XAA,0XEE,0X28,
0XCC,0X22,0XFF,0X6C,0XFF,0X6C,0XFF,0X2B,0XFF,0X29,0XFE,0XE8,0XFE,0XA7,0XB3,0X41,
0XED,0X64,0XD4,0X22,0XFE,0X23,0XFD,0XA1,0XBB,0XC2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,
0XFE,0X28,0XFE,0X2A,0XFE,0X2C,0XFE,0X6E,0XFE,0XAF,0XFF,0X31,0XFF,0XB2,0XFF,0XF3,
0XFF,0XF2,0XFF,0XB0,0XFF,0X6E,0XFE,0XED,0XFE,0X6C,0XFD,0XEB,0XFD,0XAA,0XFD,0XA8,
0XFD,0XA5,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,
0XFF,0X6A,0XFF,0XAB,0XFF,0XAE,0XFF,0XF0,0XFF,0XF2,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,
0XFF,0XAE,0XFF,0XAB,0XFF,0X6A,0XFF,0X28,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,
0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,0XFF,0X28,0XFF,0X6A,
0XFF,0XAB,0XFF,0XAD,0XFF,0X6C,0XFF,0X2A,0XFF,0X6B,0XFF,0X6B,0XFF,0X2A,0XFF,0X6B,
0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XF5,0X21,0XBC,0X25,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,0XFF,0XAD,
0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,
0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XF5,0X21,0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,
0XFD,0X66,0XFC,0XA9,0XFB,0X8B,0XFA,0XCC,0XFB,0X0D,0XFB,0XCD,0XFD,0X2E,0XFE,0X6F,
0XFF,0XAE,0XFF,0X6D,0XFE,0XAC,0XFD,0X2B,0XFB,0X8B,0XFA,0XCB,0XFA,0X8B,0XFB,0X09,
0XFC,0X26,0XF4,0XA3,0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XA2,0X80,
0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,
0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0XA2,0X80,0XFD,0XE2,0XF5,0X21,
0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,0XFF,0X28,0XDF,0X3A,
0XC5,0XB4,0XFF,0XAD,0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,
0XFF,0X28,0XFE,0XE7,0XD5,0X2E,0XFE,0X24,0XFD,0XE2,0XF5,0X21,0XBC,0X25,0XD6,0XB9,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,0XED,0XA5,0XCC,0X22,0XFF,0X6C,0XE5,0X66,
0XFF,0X29,0XFF,0X29,0XFF,0X29,0XFE,0XE9,0XFE,0XE8,0XFE,0XA7,0XFE,0X26,0XB3,0X01,
0XBB,0X81,0XE5,0X22,0XFD,0XE2,0XF5,0X21,0XC4,0X26,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,
0XF6,0X25,0XFE,0XA7,0XFE,0XE9,0XFF,0X2B,0XBA,0XC0,0XF6,0XA9,0XFF,0XAF,0XFF,0XAF,
0XFF,0XAE,0XFF,0XAD,0XFF,0X6C,0XF6,0X67,0XBA,0XC0,0XFE,0XA8,0XFE,0X66,0XFE,0X24,
0XFD,0XE3,0XF5,0X21,0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,
0XFF,0X28,0XFF,0X6A,0XFF,0XAB,0XFF,0XAD,0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,
0XFF,0XAB,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XF5,0X21,
0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,0XFE,0XE7,0XFF,0X28,
0XFF,0X6A,0XFF,0X2A,0XD5,0X27,0XBA,0XC0,0X8A,0X40,0X92,0X40,0XBA,0XC0,0XE5,0XE8,
0XFF,0X2B,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XDC,0X61,0XC4,0XED,0XDE,0XFA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,0XFE,0XE7,0XFF,0X28,0XFF,0X6A,0XFF,0X6A,
0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,
0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XDC,0X61,0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,
0XEC,0XE5,0XFD,0X28,0XFC,0X6A,0XFB,0XCB,0XFB,0XCC,0XFC,0XAC,0XFD,0XAC,0XFE,0XAC,
0XFF,0X6B,0XFF,0X6B,0XFE,0XAA,0XFD,0X6A,0XFC,0X6A,0XFB,0X8A,0XFB,0X49,0XFB,0XC8,
0XFC,0X65,0XDC,0X22,0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XA2,0X80,
0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,
0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0XA2,0X80,0XFD,0XA1,0XDC,0X61,
0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,0XFE,0XE7,0XD5,0XAE,
0XD5,0X2C,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,
0XD7,0XFF,0XFE,0XA6,0XFF,0XFF,0XE6,0X2E,0XFD,0XA1,0XDC,0X61,0XC4,0XED,0XDE,0XFA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,0XFE,0XE7,0XCB,0XC1,0XFF,0X6C,0XFF,0X2A,
0XFE,0XE8,0XFE,0XE8,0XFE,0XE8,0XFE,0XA7,0XFE,0XA7,0XFE,0X26,0XE4,0XE4,0XBB,0X00,
0XE5,0X23,0XFE,0X23,0XFD,0XA1,0XDC,0X61,0XDD,0XB0,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,
0XE5,0X64,0XFE,0XA6,0XFE,0XE7,0XFF,0X29,0XF6,0XA8,0XBA,0XC0,0XF6,0XA8,0XFF,0XAB,
0XFF,0XAB,0XFF,0X6A,0XF6,0X67,0XBA,0XC0,0XF6,0X66,0XFE,0XA6,0XFE,0X64,0XFE,0X23,
0XFD,0XA2,0XDC,0X61,0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,
0XFE,0XE7,0XFF,0X28,0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,
0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XDC,0X61,
0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,0XFE,0XA6,0XFE,0XE7,
0XFF,0X6C,0XBB,0X81,0XC3,0XC2,0XEE,0X27,0XFF,0X69,0XFF,0X29,0XEE,0X27,0XC3,0XC1,
0XAA,0XC0,0XFE,0XEA,0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XBB,0XC6,0XD6,0X37,0XEF,0X7C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,0XFE,0XE7,0XFE,0XE7,0XFF,0X28,0XFF,0X29,
0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XFE,0XE8,0XFE,0XE7,0XFE,0XA6,0XFE,0X64,
0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XBB,0XC6,0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,
0XD4,0X24,0XFD,0XA5,0XFD,0XA7,0XFD,0X68,0XFD,0XA9,0XFD,0XE9,0XFE,0XAA,0XFE,0XE9,
0XFF,0X69,0XFF,0X29,0XFE,0XE8,0XFE,0X28,0XFD,0XA7,0XFD,0X27,0XFC,0XE6,0XFC,0XE4,
0XF4,0XA2,0XBB,0XC6,0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X32,0XCB,0XC2,0XCB,0XC1,
0XBB,0X41,0XA2,0X40,0X81,0X80,0X99,0XC0,0XEA,0XC0,0XFB,0X00,0XFB,0X00,0XFB,0X00,
0XEA,0XC0,0X99,0XC0,0X81,0X80,0XA2,0X40,0XC3,0X41,0XCB,0XC1,0XCB,0XC1,0XBB,0XC4,
0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,0XFE,0XE7,0XFE,0XE7,
0XFF,0X28,0XBB,0X41,0XED,0XE6,0XED,0XE6,0XBB,0X41,0XBB,0X41,0XED,0XA5,0XFE,0XE7,
0XD5,0XAE,0XFE,0X64,0XD7,0XFF,0XA3,0X8A,0XF5,0X21,0XBB,0XC6,0XD6,0X37,0XEF,0X7C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XCC,0X64,0XFE,0X65,0XED,0XA4,0XD4,0XA4,0XFF,0X2C,0XFE,0XE9,
0XFE,0XA7,0XFE,0XA7,0XFE,0XA7,0XFE,0X66,0XFE,0X26,0XFD,0XE5,0XBB,0X81,0XE5,0X23,
0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XBC,0X27,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,
0XCC,0X64,0XFE,0X24,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XA7,0XC3,0X41,0XBB,0X41,
0XBB,0X41,0XBB,0X41,0XC3,0X41,0XFE,0XA7,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XE2,
0XF5,0X21,0XB3,0XC6,0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,
0XFE,0XA6,0XFE,0XE7,0XFF,0X28,0XFF,0X29,0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X29,
0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XBB,0XC6,
0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XE5,0X64,0XFE,0X64,0XFE,0XA5,
0XBB,0X81,0XF6,0X67,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0XA5,
0XF6,0X67,0XBB,0X81,0XFD,0XE2,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XDC,0XE3,0XFE,0X64,0XFE,0XA6,0XFE,0XA6,0XFE,0XE7,
0XFE,0XE7,0XFE,0XE7,0XBB,0X41,0XBB,0X41,0XFE,0XE8,0XFE,0XA5,0XFE,0X64,0XFE,0X23,
0XFD,0XE2,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XE5,0XF1,0XE5,0X64,0XFE,0X25,0XFE,0X26,0XFE,0X67,0XFE,0XA7,0XFE,0XE7,0XFE,0XE7,
0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0X65,0XFE,0X25,0XFD,0XE4,0XFD,0XA3,0XFD,0X62,
0XB3,0X02,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X24,0XF6,0X67,0XFF,0X6A,
0XF6,0X67,0XC3,0X41,0XB2,0X00,0XEA,0XC0,0XFB,0X00,0XFB,0X00,0XFB,0X00,0XFB,0X00,
0XFB,0X00,0XEA,0XC0,0XB2,0X00,0XC3,0X81,0XF6,0X67,0XFF,0X6A,0XF6,0X67,0XC3,0X82,
0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XDC,0XE3,0XFE,0X64,0XFE,0XA6,
0XFE,0XA6,0XED,0XE7,0XBB,0X41,0XBB,0X41,0XFF,0X6B,0XFF,0X6B,0XBB,0X41,0XED,0X64,
0XFE,0X64,0XFE,0X23,0XA4,0XB0,0XA3,0X88,0XAB,0X01,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XDC,0XE3,0XC3,0XC1,0XE5,0XE8,0XFE,0XEA,0XFE,0XA7,
0XFE,0X67,0XFE,0X66,0XFE,0X26,0XFD,0XE5,0XFD,0XA4,0XAB,0X00,0XE5,0X22,0XF5,0XA3,
0XFD,0XE2,0XFD,0XA1,0XAB,0X02,0XD5,0X6F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XE6,0X31,0XE5,0X64,0XFE,0X64,0XFE,0XA5,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,
0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0XA5,0XFE,0X64,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,
0XAB,0X01,0XC4,0XEC,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XE5,0X64,
0XFE,0X64,0XFE,0XA5,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,
0XFE,0XA6,0XFE,0XA5,0XFE,0X64,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,
0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,0XE5,0X24,0XFE,0X24,
0XFE,0X64,0XFE,0X65,0XFE,0XA5,0XFE,0XA5,0XFE,0XA5,0XFE,0X65,0XFE,0X64,0XFE,0X24,
0XFE,0X23,0XFD,0XE2,0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,0XDC,0XA2,0XFE,0X24,0XFE,0XA6,0XFE,0X65,
0XFE,0XA5,0XFE,0XA5,0XBB,0X41,0X81,0X80,0XFE,0XE8,0XFE,0X24,0XFE,0X23,0XFD,0XE2,
0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCC,0X67,0XE5,0X24,0XFE,0X24,0XFE,0X64,0XFE,0X65,0XFE,0XA5,0XFE,0XA5,
0X81,0X80,0X81,0X80,0XFE,0X64,0XFE,0X24,0XFD,0XE3,0XFD,0XE2,0XFD,0X61,0XCC,0X21,
0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC3,0X81,0XFF,0X6A,0XFF,0X6B,
0XFF,0X6A,0XC3,0X81,0XA2,0X80,0XA2,0X80,0X82,0X00,0X82,0X00,0X82,0X00,0X82,0X00,
0X82,0X00,0XA2,0X80,0XA2,0X80,0XC3,0X81,0XFF,0X6A,0XFF,0X6B,0XFF,0X6A,0XC3,0X81,
0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,0XDC,0XA2,0XFE,0X24,
0XFE,0XA6,0XFE,0XE7,0XFF,0X6B,0XFF,0X6B,0XFE,0XA5,0XFE,0X65,0XFF,0X6B,0XBB,0X41,
0XED,0X22,0XFD,0XE2,0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC3,0X81,0XFF,0X6D,0XFF,0X2B,0XFE,0XA7,0XFE,0X66,
0XFE,0X25,0XFD,0XE5,0XFD,0XA4,0XD4,0X62,0XAB,0X00,0XE4,0XE2,0XED,0X22,0XFD,0XE2,
0XFD,0X61,0XCC,0X21,0XC4,0XAB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCC,0XA7,0XE5,0X24,0XFE,0X24,0XFE,0X64,0XFE,0X65,0XFE,0XA5,0XFE,0XA5,
0XFE,0XA5,0XFE,0X65,0XFE,0X64,0XFE,0X24,0XFE,0X23,0XFD,0XE2,0XFD,0X61,0XCC,0X21,
0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,
0XE5,0X24,0XFE,0X24,0XFE,0X64,0XFE,0X65,0XFE,0XA5,0XFF,0X6B,0XA2,0X80,0XA2,0X80,
0XFF,0X6B,0XFE,0X24,0XFE,0X23,0XFD,0XE2,0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,
0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XDD,0X23,
0XF5,0XA2,0XFE,0X23,0XFE,0X23,0XFE,0X24,0XFE,0X23,0XFE,0X23,0XFE,0X23,0XFD,0XE2,
0XFD,0XA1,0XE4,0XE1,0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XD4,0X62,0XF5,0XA2,0XFE,0X65,
0XFE,0X23,0XFE,0X24,0XFE,0XE8,0XFE,0XE8,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XE4,0XE1,
0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XDD,0X23,0XF5,0XA2,0XFE,0X23,0XFE,0X23,0X81,0X80,
0X81,0X80,0X81,0X80,0X81,0X80,0XFD,0XE2,0XFD,0XA1,0XE4,0XE1,0XAB,0X02,0XBC,0X6A,
0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC3,0X81,0XFE,0XE8,0XFF,0X6A,
0XFE,0XE8,0XC3,0X41,0XF5,0XA2,0XFE,0X65,0XFE,0X23,0XFE,0X24,0XFE,0X23,0XFE,0X23,
0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XC3,0X81,0XFE,0XE8,0XFF,0X6A,0XFE,0XE8,0XC3,0X41,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XD4,0X62,
0XF5,0XA2,0XFE,0X65,0XFE,0X23,0XFE,0X24,0XFE,0X23,0XFE,0X23,0XFE,0X23,0XFF,0X6B,
0XFE,0X65,0XE4,0XE1,0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XCC,0X23,0XD4,0XA5,0XFE,0XEB,0XFE,0X67,0XFE,0X25,0XED,0X64,
0XD4,0X62,0XB3,0X41,0XB3,0X41,0XC3,0XC1,0XE4,0XA2,0XED,0X22,0XFD,0XA1,0XE4,0XE1,
0XAB,0X02,0XCC,0XEC,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XDD,0X23,0XF5,0XA2,0XFE,0X23,0XFE,0X23,0XFE,0X24,
0XFE,0X23,0XFE,0X23,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XE4,0XE1,0XAB,0X02,0XBC,0X6A,
0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XC4,0X67,0XDD,0X23,0XF5,0XA2,0XFE,0X23,0XFE,0X23,0XFF,0X6B,0XA2,0X80,0XA2,0X80,
0XFF,0X6B,0XFD,0XE2,0XFD,0XA1,0XE4,0XE1,0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,
0XC4,0X64,0XCC,0X21,0XE4,0XE2,0XF5,0X62,0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XA1,
0XCB,0XC1,0XAB,0X85,0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,0XB3,0X43,0XCC,0X21,
0XE4,0XE2,0XF5,0X62,0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XA1,0XCB,0XC1,0XAB,0X85,
0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,0XC4,0X64,0XCC,0X21,0XE4,0XE2,0XF5,0X62,
0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XA1,0XCB,0XC1,0XAB,0X85,0XCD,0XB2,0XDE,0XB9,
0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X25,0XF5,0X63,0XFE,0X25,
0XF5,0X63,0XC3,0X83,0XB3,0X43,0XCC,0X21,0XE4,0XE2,0XF5,0X62,0XFD,0XE2,0XFD,0XA2,
0XF5,0X61,0XE4,0XA1,0XCB,0XC1,0XBB,0X41,0XF5,0X63,0XFE,0X25,0XF5,0X63,0XC3,0XC5,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,
0XB3,0X43,0XCC,0X21,0XE4,0XE2,0XF5,0X62,0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XA1,
0XCB,0XC1,0XAB,0X85,0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XCC,0X63,0XFE,0XA8,0XFE,0X25,0XFD,0XE4,0XE4,0XE3,0XBB,0X81,
0XC3,0X81,0XDC,0X61,0XE4,0XA1,0XE4,0XA1,0XE4,0XE1,0XE4,0XA1,0XCB,0XC1,0XB3,0XC6,
0XE6,0XB7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,0XC4,0X64,0XCC,0X21,0XE4,0XE2,0XF5,0X62,
0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XA1,0XCB,0XC1,0XAB,0X85,0XCD,0XB2,0XDE,0XB9,
0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCD,0X70,0XC4,0X64,0XCC,0X21,0XE4,0XE2,0XF5,0X62,0XFD,0XE2,0XFD,0XA2,
0XF5,0X61,0XE4,0XA1,0XCB,0XC1,0XAB,0X85,0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XEF,0X3B,0XCD,0X70,0XBC,0X28,0XB3,0X85,0XAA,0XC1,0XAB,0X01,0XAB,0X44,0XB4,0X29,
0XBC,0XAC,0XD6,0XB9,0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XCD,0X70,
0XBC,0X28,0XB3,0X85,0XAA,0XC1,0XAB,0X01,0XAB,0X44,0XB4,0X29,0XBC,0XAC,0XD6,0XB9,
0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XCD,0X70,0XBC,0X28,0XB3,0X85,
0XAA,0XC1,0XAB,0X01,0XAB,0X44,0XB4,0X29,0XBC,0XAC,0XD6,0XB9,0XDE,0XFA,0XF7,0XBE,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X33,0XBB,0X00,0XBB,0X00,
0XBB,0X00,0XE6,0X33,0XEF,0X3B,0XCD,0X70,0XBC,0X28,0XB3,0X85,0XAA,0XC1,0XAB,0X01,
0XAB,0X44,0XB4,0X29,0XBC,0XAC,0XCD,0X6F,0XBB,0X00,0XBB,0X00,0XBB,0X00,0XE6,0X33,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XEF,0X3B,0XCD,0X70,0XBC,0X28,0XB3,0X85,0XAA,0XC1,0XAB,0X01,0XAB,0X44,0XB4,0X29,
0XBC,0XAC,0XD6,0XB9,0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XED,0XA4,0XFD,0XE4,0XFD,0XA4,0XE4,0XE3,0XB3,0X01,0XFF,0XFF,
0XC4,0XAA,0XB3,0XC6,0XAA,0XC1,0XAB,0X02,0XB3,0X85,0XC4,0XAB,0XCD,0X2F,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XCD,0X70,0XB4,0X28,0XB3,0X85,
0XAA,0XC1,0XA3,0X01,0XAB,0X44,0XB4,0X28,0XBC,0XAC,0XD6,0XB9,0XDE,0XFA,0XF7,0XBE,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XCD,0X70,0XBC,0X28,0XB3,0X85,0XAA,0XC1,0XAB,0X01,
0XAB,0X44,0XB4,0X29,0XBC,0XAC,0XD6,0XB9,0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XE6,0XFB,0XDE,0XFA,0XD6,0XB9,0XDE,0XB9,0XDE,0XBA,
0XE7,0X3B,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XF7,0X7D,0XE6,0XFB,0XDE,0XFA,0XD6,0XB9,0XDE,0XB9,0XDE,0XBA,0XE7,0X3B,0XEF,0X3C,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XE6,0XFB,
0XDE,0XFA,0XD6,0XB9,0XDE,0XB9,0XDE,0XBA,0XE7,0X3B,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XE6,0XFB,0XDE,0XFA,0XD6,0XB9,
0XDE,0XB9,0XDE,0XBA,0XE7,0X3B,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XE6,0XFB,0XDE,0XFA,0XD6,0XB9,0XDE,0XB9,0XDE,0XBA,
0XE7,0X3B,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XA3,0XE4,0XA2,0XBB,0X81,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XE6,0XFB,
0XDE,0XFA,0XD6,0XB9,0XDE,0XB9,0XDE,0XBA,0XE7,0X3B,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0X7D,0XE6,0XFB,0XDE,0XFA,0XD6,0XB9,
0XDE,0XB9,0XDE,0XBA,0XE7,0X3B,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X00,
0X18,0X82,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0XFB,0XCF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X29,0X04,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0X59,0X00,0X59,0X00,0X59,0X00,0XD6,0X37,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XD6,0X37,0X59,0X00,0X59,0X00,0X59,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB5,0XB6,
0X21,0X04,0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,
0XFF,0X35,0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,
0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0XF8,0X00,0XF8,0X00,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,
0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,
0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XD5,0XB0,0X59,0X00,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,
0XF6,0X29,0X59,0X00,0XD5,0X6E,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XBB,0XFE,0XAF,0XFE,0X28,0XF5,0XE6,0XF5,0XE5,0XF5,0XA5,0XF5,0XE6,0XF6,0X28,
0XFE,0XAF,0XFF,0X7A,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,
0XFE,0X6B,0XF6,0X28,0XF5,0XE5,0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X21,0X04,
0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDE,0XFB,0XCE,0X79,0XBD,0XF7,0XBD,0XF7,0XBD,0XF7,
0XBD,0XF7,0XCE,0X79,0XDE,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XFE,0X68,
0XFE,0XE8,0XFF,0X29,0XFF,0X28,0XFF,0X28,0XFF,0X28,0XFE,0XE7,0XFE,0XA7,0XFE,0X67,
0XED,0XEB,0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XF8,0X00,0XC0,0X00,0XFB,0XCF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X25,
0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFE,0XA6,0XFE,0X65,0XF5,0XE4,0XED,0XEB,
0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XFF,0X35,0XFE,0X6B,0XF6,0X28,0XF5,0XE5,
0XF5,0XE6,0XF6,0X29,0XF6,0X6C,0XFE,0XF3,0XFF,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0X34,0XF5,0XE6,0XFE,0X66,0XFE,0XE8,0XFF,0X28,0XFF,0X28,0XFE,0XE7,
0XFE,0XA7,0XFE,0X66,0XFE,0X26,0XF6,0X2B,0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0X6D,
0XFE,0X26,0XFE,0XE8,0XFF,0X29,0XFF,0X29,0XFF,0X29,0XFE,0XE9,0XFE,0XE8,0XFE,0XA7,
0XFE,0X66,0XF5,0XA5,0XF6,0X2C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XF6,0X27,0XFE,0XA8,
0XFF,0X29,0XFF,0X2A,0XFF,0X6A,0XFF,0X6A,0XFF,0X29,0XFE,0XE8,0XFE,0X67,0XF6,0X2B,
0XFF,0X36,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X94,0XB2,
0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE7,0X3C,0XA5,0X34,0X5A,0XCB,0X31,0X86,
0X18,0XC3,0X18,0XC3,0X29,0X45,0X52,0X8A,0X9C,0XF3,0XE7,0X3C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDE,0XFB,0XBD,0XF7,0XCE,0X79,0XD6,0XBA,0XDE,0XFB,0XDE,0XFB,0XD6,0XBA,
0XCE,0X79,0XC6,0X38,0XBD,0XF7,0XBD,0XF7,0XE7,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XD9,0X40,0XE9,0XC6,0XF2,0XCB,
0XFB,0X4D,0XFC,0X71,0XFC,0XB2,0XFC,0X71,0XFB,0XCF,0XF3,0X4D,0XF9,0X86,0XE1,0X44,
0XC8,0XC2,0XB0,0XC2,0XA8,0XC1,0XE0,0X40,0XF8,0X00,0XFB,0XCF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF6,0X6B,0XFE,0X26,0XFE,0XE8,0XFF,0X69,
0XFF,0X6A,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XFF,0X28,0XFE,0XE8,0XFE,0XE7,0XAB,0X01,
0XAB,0X02,0XED,0XEA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0X34,0XFE,0X27,0XFE,0XA7,0XFE,0XE8,0XFE,0XE8,0XFF,0X28,
0XFE,0XE7,0XFE,0XA7,0XFE,0X66,0XFE,0X26,0XED,0XEB,0XF6,0XF6,0XFF,0XFF,0XFF,0XFF,
0X00,0X00,0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,
0XF6,0X6B,0XF5,0XE5,0XCC,0X22,0XBB,0X81,0XCC,0X22,0XF6,0XA8,0XFF,0X69,0XFF,0X29,
0XF6,0X67,0XCC,0X22,0XBB,0X81,0XCC,0X22,0XED,0X65,0XEE,0X2A,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF5,0XE5,0XFF,0X29,
0XFF,0X2A,0XFF,0X2A,0XFF,0X6A,0XFF,0X6A,0XFF,0X6A,0XFF,0X2A,0XFF,0X29,0XFE,0XE8,
0XFE,0XE8,0XFE,0XA7,0XF6,0X25,0XED,0X64,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF6,0X2A,0XFF,0X29,0XFF,0X6A,0XFF,0XAC,
0XFF,0XAC,0XFF,0XAD,0XFF,0XAC,0XFF,0XAC,0XFF,0X6A,0XFF,0X69,0XFF,0X29,0XFE,0XA7,
0XED,0XE6,0XED,0XEA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X94,0XB2,0X21,0X04,
0X21,0X04,0X21,0X04,0XFF,0XFF,0X84,0X30,0X4A,0X49,0X18,0XC3,0X21,0X04,0X21,0X04,
0X21,0X04,0X18,0XC3,0X18,0XC3,0X10,0X82,0X08,0X41,0X39,0XC7,0X7B,0XCF,0XFF,0XFF,
0XFF,0XFF,0X21,0X04,0X21,0X04,0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XC6,0X38,0XCE,0X79,0XEF,0X7D,0XEF,0X7D,0XEF,0X7D,0XEF,0X7D,0XE7,0X3C,0XE7,0X3C,
0XDE,0XFB,0XD6,0XBA,0XCE,0X79,0XC6,0X38,0XB5,0XB6,0XCE,0X79,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDA,0XC4,0XD8,0X00,0XF0,0X00,0XF0,0X00,
0XF0,0X00,0XF0,0XC3,0XF0,0XC3,0XF0,0XC3,0XF0,0XC3,0XF0,0XC3,0XE0,0XC2,0XC9,0X02,
0XB0,0X81,0XA8,0X81,0XA0,0X40,0XA0,0X80,0XFB,0XCF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,0XC4,0X23,0XC4,0XA5,0XFF,0X6A,0XFF,0X6A,
0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XBC,0X23,0XB3,0X82,
0XD4,0XA3,0XF5,0XA3,0XE5,0XAA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XF6,0X6B,0XFF,0X29,0XFF,0X28,0XFF,0X29,0XFF,0X69,0XFF,0X69,0XFF,0X69,
0XFF,0X29,0XFF,0X28,0XF6,0X66,0XDC,0XE3,0XD4,0X22,0XD4,0X23,0XDC,0XE5,0XFF,0XBD,
0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X2B,
0XFE,0X26,0XBB,0X81,0XEE,0XB5,0XFF,0XFF,0XEE,0XB5,0XBB,0X81,0XFF,0XAB,0XFF,0X6A,
0XBB,0X81,0XEE,0XB5,0XFF,0XFF,0XEE,0XB5,0XBB,0X81,0XF5,0XA3,0XEE,0X2A,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF5,0XA5,0XFF,0X29,0XFF,0X6A,
0XFF,0X6A,0XFF,0XAA,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6B,0XFF,0X6A,0XFF,0X2A,
0XFF,0X29,0XFE,0XE7,0XFE,0XA6,0XF6,0X66,0XED,0X24,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0XFF,0X29,0XFF,0X6B,0XFF,0XAD,0XFF,0XAE,
0XFF,0XF0,0XFF,0XF0,0XFF,0XF0,0XFF,0XAE,0XFF,0XAD,0XFF,0X6B,0XFF,0X69,0XFF,0X28,
0XFE,0XA6,0XF6,0X66,0XED,0XEB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XC6,0X38,0X21,0X04,0X21,0X04,0X18,0XC3,0X29,0X45,0X29,0X45,0X31,0X86,0X31,0X86,
0X31,0X86,0X29,0X45,0X29,0X45,0X18,0XC3,0X10,0X82,0X08,0X41,0X00,0X00,0X39,0XC7,
0X21,0X04,0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBD,0XF7,
0XD6,0XBA,0XF7,0XBE,0XF7,0XBE,0XF7,0XBE,0XF7,0XBE,0XF7,0XBE,0XF7,0XBE,0XEF,0X7D,
0XE7,0X3C,0XDE,0XFB,0XD6,0XBA,0XCE,0X79,0XBD,0XF7,0XB5,0XB6,0XC6,0X38,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XFE,0XE9,0XC3,0X01,0XBB,0X0B,0XB8,0X80,
0XD1,0XC2,0XC9,0X01,0XD0,0X40,0XC8,0X40,0XC0,0X40,0XC0,0XC1,0XB1,0X01,0XB0,0X80,
0XCC,0X2F,0XBA,0X81,0XED,0X23,0XE5,0X66,0XDD,0XED,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XF6,0X25,0X9A,0XC2,0XB3,0X83,0XB3,0XC4,0XFF,0XAD,
0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,0XFF,0X6B,0XCD,0X26,0XAB,0X43,0XFE,0XE7,
0XFE,0XA5,0XFE,0XA6,0XE5,0X24,0XDD,0XED,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XF6,0X2B,0XFF,0X29,0XFF,0X28,0XFF,0X6A,0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,
0XFF,0X6A,0XF6,0X68,0XCB,0XC2,0XEE,0X74,0XFF,0XBC,0XF7,0X7B,0XE5,0XF0,0XCC,0X23,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,0XF6,0X25,
0XFF,0X69,0XB3,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB3,0X01,0XFF,0XAE,0XFF,0XAD,
0XB3,0X01,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB3,0X00,0XFE,0X24,0XE5,0X24,0XE6,0X2D,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X6C,0XFE,0XE8,0XFF,0X6A,0X9A,0X40,
0X9A,0X40,0X9A,0X40,0XFF,0XEB,0XFF,0XEB,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,
0X9A,0X40,0X9A,0X40,0X9A,0X40,0XFE,0XA6,0XF5,0XE5,0XED,0XEC,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XEE,0X70,0XFE,0XE8,0XFF,0X6A,0XFF,0XAD,0XFF,0XEF,0XFF,0XF2,
0XFF,0XF3,0XFF,0XF4,0XFF,0XF3,0XFF,0XF2,0XFF,0XEF,0XFF,0XAD,0XFF,0X6A,0XFF,0X29,
0XFE,0XE7,0XFE,0XA6,0XE5,0XA5,0XEE,0X2F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0X39,0XC7,0X21,0X04,0X29,0X45,0X31,0X86,0X39,0XC7,0X42,0X08,0X42,0X08,
0X42,0X08,0X39,0XC7,0X31,0X86,0X29,0X45,0X21,0X04,0X10,0X82,0X08,0X41,0X00,0X00,
0X39,0XC7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCE,0X79,0XCE,0X79,
0XF7,0XBE,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0XBE,
0XEF,0X7D,0XE7,0X3C,0XDE,0XFB,0XCE,0X79,0XC6,0X38,0XBD,0XF7,0XAD,0X75,0XD6,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XF6,0X67,0XD4,0X22,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0X81,0X80,0XEE,0X27,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,0XEE,0X27,0X81,0X80,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XD4,0X22,0XFD,0XE3,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XFF,0X29,0XFF,0X6A,0XFF,0X6B,0XCC,0XE5,0X92,0X41,
0XFF,0XF2,0XFF,0XF3,0XFF,0XF2,0XFF,0XB0,0XE6,0X2A,0X92,0X41,0XA3,0XC7,0XFF,0X28,
0XFE,0XA6,0XFE,0X64,0XFE,0X24,0XDD,0X28,0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFE,0XF3,
0XF6,0X27,0XDC,0XE4,0XD4,0X63,0XD4,0XA3,0XE5,0XA7,0XFF,0X6E,0XFF,0XEF,0XFF,0XAE,
0XFF,0XAD,0XCC,0X23,0XEE,0XB6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0XAF,
0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XED,0XA6,0XFE,0XE8,
0XFF,0X6A,0XB3,0X00,0XFF,0XFF,0X71,0X40,0X71,0X40,0XB3,0X01,0XFF,0XF2,0XFF,0XF0,
0XB3,0X01,0XFF,0XFF,0X71,0X40,0X71,0X40,0XB3,0X00,0XFE,0X64,0XFD,0XE3,0XDD,0X28,
0XE6,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBB,0XF6,0X26,0XFF,0X29,0X9A,0X40,0XFF,0XAA,
0XFF,0XAA,0XFF,0XEA,0X9A,0X40,0XFF,0XEA,0XFF,0XEA,0XFF,0XEA,0XFF,0XAB,0X9A,0X40,
0XFF,0XAA,0XFF,0X69,0XFF,0X28,0X9A,0X40,0XFE,0X65,0XE5,0X24,0XFF,0X7B,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XEF,0X3A,0XF5,0XE6,0XFF,0X29,0XFF,0XAC,0XAB,0X85,0X81,0X80,0XE6,0X6F,
0XFF,0XF6,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,0XFF,0XF2,0XE6,0X2B,0X81,0X80,0XAB,0X83,
0XFF,0X28,0XFE,0XA6,0XFE,0X24,0XE5,0X69,0XFF,0XBD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0X84,0X30,0X18,0XC3,0X63,0X0C,0XDE,0XFB,0XFF,0XFF,0XFF,0XFF,0XE7,0X3C,0X6B,0X4D,
0X4A,0X49,0X63,0X0C,0XE7,0X3C,0XFF,0XFF,0XFF,0XFF,0XDE,0XFB,0X4A,0X49,0X00,0X00,
0X00,0X00,0X7B,0XCF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X7D,0XBD,0XF7,0XF7,0XBE,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XF7,0XBE,0XEF,0X7D,0XEF,0X7D,0XE7,0X3C,0XD6,0XBA,0XC6,0X38,0XB5,0XB6,0XAD,0X75,
0XEF,0X7D,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,0XFE,0XA7,0XD4,0X22,0XFF,0XFF,0XFF,0XFF,0X71,0X40,
0X61,0X00,0X81,0X80,0XEE,0X27,0XFF,0XF6,0XEE,0X27,0X81,0X80,0X61,0X00,0X71,0X40,
0XFF,0XFF,0XFF,0XFF,0XD4,0X22,0XFE,0X23,0XED,0X22,0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XF6,0XF3,0XF6,0X66,0XFF,0X29,0XFF,0X6A,0XFF,0XAD,0XA3,0XC7,0XE6,0X28,
0X92,0X41,0XFF,0XF6,0XFF,0XF6,0XFF,0XF3,0X92,0X41,0X8A,0XC4,0X59,0X00,0X69,0XC1,
0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XED,0X22,0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFD,0XED,0X65,
0XCB,0XC2,0XEE,0X74,0XFF,0XBC,0XF7,0X7B,0XE5,0XF0,0XD4,0X65,0XFF,0XB2,0XFF,0XF2,
0XF7,0X2E,0XC3,0XC6,0XFF,0XFF,0XFF,0XFF,0X81,0X80,0X81,0X80,0XFF,0XFF,0XEE,0X75,
0X18,0X40,0X18,0X80,0XD4,0X22,0XE5,0XEF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XF3,0XF6,0X25,0XFF,0X29,
0XFF,0X6A,0XBB,0X81,0XE6,0X31,0X71,0X40,0X61,0X00,0XBB,0X81,0XFF,0XF6,0XFF,0XF4,
0XBB,0X81,0XEE,0XB5,0X71,0X40,0X61,0X00,0XBB,0X81,0XFE,0X65,0XFE,0X23,0XED,0X22,
0XDD,0XAD,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0X6E,0XFE,0XA7,0X9A,0X40,0XFF,0XAA,0XFF,0XAA,
0X9A,0X40,0XFF,0XE9,0XFF,0XE9,0X9A,0X40,0XFF,0XE9,0XFF,0XE9,0X9A,0X40,0XFF,0XAA,
0XFF,0XAA,0X9A,0X40,0XFF,0X28,0XFE,0XE7,0X9A,0X40,0XF5,0XA4,0XED,0XEE,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE6,0X2F,0XF6,0X67,0XFF,0X6A,0XFF,0XAC,0XFF,0XF0,0XE6,0X2E,0X81,0X80,
0X81,0X80,0XE6,0X73,0XFF,0XF9,0XE6,0X71,0X81,0X80,0X81,0X80,0XE6,0X29,0XFF,0X6A,
0XFF,0X28,0XFE,0XE6,0XFE,0X65,0XED,0X63,0XE6,0X2F,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE7,0X3C,
0X4A,0X49,0X5A,0XCB,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0X73,0X8E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0XBE,0X4A,0X49,
0X00,0X00,0X39,0XC7,0XE7,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCE,0X79,0XD6,0XBA,0XF7,0XBE,
0XF7,0XBE,0X84,0X30,0X39,0XC7,0X39,0XC7,0X84,0X30,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,
0XF7,0XBE,0X84,0X30,0X39,0XC7,0X39,0XC7,0X84,0X30,0XD6,0XBA,0XBD,0XF7,0XAD,0X75,
0XCE,0X79,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0XFE,0XE8,0XDC,0XE4,0XFF,0XFF,0XFF,0XFF,0X71,0X40,
0X71,0X40,0XFF,0XFF,0X81,0X80,0XFF,0XF8,0X81,0X80,0XFF,0XFF,0X71,0X40,0X71,0X40,
0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFE,0X23,0XFD,0XA2,0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XED,0XE9,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0X8A,0XC4,0X59,0X00,0X69,0XC1,
0XFF,0XF8,0XFF,0XFA,0XFF,0XF8,0XFF,0XF6,0XFF,0XF2,0X72,0X02,0X59,0X00,0X69,0XC1,
0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XA2,0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF6,0XB2,0XCB,0XC2,
0XEE,0XB6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0XAF,0XE5,0XAC,0XFF,0XF6,
0XEE,0XF0,0XBB,0XC7,0XFF,0XFF,0XFF,0XFF,0X59,0X00,0X81,0X80,0XFF,0XFF,0XD4,0XE7,
0XEE,0X27,0XFF,0X6B,0XFF,0X6B,0XE5,0X66,0XE5,0XAE,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XE9,0XFE,0XA7,0XFF,0X69,
0XFF,0XAB,0XEE,0X6A,0XA2,0X80,0X81,0X80,0XA2,0X80,0XEE,0XB1,0XFF,0XF8,0XFF,0XF6,
0XEE,0XAC,0XA2,0X80,0X81,0X80,0XA2,0X80,0XED,0XE5,0XFE,0XA5,0XFE,0X23,0XFD,0XA2,
0XD4,0XA6,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XA8,0XFF,0X29,0X9A,0X40,0XFF,0XAA,0X9A,0X40,
0XFF,0XEA,0XFF,0XE9,0XFF,0XE8,0X9A,0X40,0XFF,0XE8,0XFF,0XE9,0X9A,0X40,0XFF,0XEA,
0X9A,0X40,0XFF,0X6A,0XFF,0X29,0XFE,0XE7,0X9A,0X40,0XFD,0XE4,0XE5,0X27,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XEF,0X7C,0XE5,0XA8,0XFE,0XE8,0XFF,0X6A,0XFF,0XAD,0X81,0X80,0X81,0X80,0X81,0X80,
0X81,0X80,0X81,0X80,0XFF,0XFA,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,
0XFF,0X28,0XFE,0XE6,0XFE,0X65,0XFD,0XE3,0XD4,0XE7,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XA5,0X34,
0X18,0XC3,0XDE,0XFB,0XFF,0XFF,0XFF,0XFF,0XAD,0X75,0X21,0X04,0XAD,0X75,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XA5,0X34,0X21,0X04,0XA5,0X34,0XFF,0XFF,0XFF,0XFF,0XDE,0XFB,
0X00,0X00,0X00,0X00,0X9C,0XF3,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB5,0XB6,0XE7,0X3C,0XFF,0XFF,
0X84,0X30,0X29,0X45,0X39,0XC7,0X42,0X08,0X4A,0X49,0X94,0XB2,0XFF,0XFF,0XFF,0XFF,
0X84,0X30,0X29,0X45,0X39,0XC7,0X42,0X08,0X4A,0X49,0X8C,0X71,0XCE,0X78,0XAD,0X75,
0XB5,0XB6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XEE,0X27,0XDC,0XE4,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0X7B,0XD4,0X22,0XFF,0XFA,0XD4,0X22,0XFF,0X7B,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE4,0XED,0XA4,0XFE,0X24,0XFD,0XE2,0XCC,0X63,0XDE,0XBA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE5,0X24,0XFF,0X28,0XFF,0X69,0XFF,0XAB,0X72,0X02,0X59,0X00,0X69,0XC1,
0XFF,0XFA,0XFF,0XFA,0XFF,0XFA,0XFF,0XF7,0XFF,0XF3,0XFF,0XEF,0XA3,0XC7,0XFF,0X69,
0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X68,0XC3,0XC6,
0XFF,0XFF,0XFF,0XFF,0X81,0X80,0X81,0X80,0XFF,0XFF,0XF6,0XF9,0XC4,0X26,0XFF,0XF8,
0XFF,0XF6,0XAB,0X02,0XEE,0XF9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X7D,0XD4,0X22,
0XFF,0X6B,0XFF,0X6B,0XFF,0X6B,0XFF,0X6B,0XCB,0XC2,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,
0XFF,0XAB,0XFF,0XEF,0XFF,0XF3,0XFF,0XF7,0XFF,0XFA,0XFF,0XFA,0XEE,0XF3,0XBB,0X81,
0XBB,0X81,0XFF,0X6D,0XFF,0X6B,0XFF,0X69,0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,
0XCC,0X63,0XDE,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0X65,0XFF,0X29,0X9A,0X40,0XFF,0X6B,0XFF,0XAB,
0X9A,0X40,0X9A,0X40,0X9A,0X40,0XFF,0XE7,0XFF,0XE7,0XFF,0XE8,0X9A,0X40,0XFF,0XAA,
0XFF,0XAB,0X9A,0X40,0X9A,0X40,0X9A,0X40,0XFE,0X66,0XFD,0XE3,0XDC,0XA4,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XDE,0XFA,0XE5,0X24,0XFF,0X28,0XFF,0X6A,0XFF,0XAC,0XFF,0XF0,0XE6,0X2E,0X81,0X80,
0X81,0X80,0XE6,0X73,0XFF,0XF9,0XE6,0X71,0X81,0X80,0X81,0X80,0XE6,0X29,0XFF,0X6A,
0XFF,0X28,0XFE,0XE6,0XFE,0X65,0XFE,0X23,0XCC,0X63,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X5A,0XCB,
0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X21,0X04,0X21,0X04,0X21,0X04,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0X21,0X04,0X21,0X04,0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0X00,0X00,0X00,0X00,0X4A,0X49,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XA5,0X34,0XEF,0X7D,0XFF,0XFF,
0X42,0X08,0X39,0XC7,0X42,0X08,0X4A,0X49,0X5A,0XCB,0X6B,0X4D,0XFF,0XFF,0XFF,0XFF,
0X42,0X08,0X39,0XC7,0X42,0X08,0X4A,0X49,0X5A,0XCB,0X6B,0X4D,0XD6,0XBA,0XAD,0X75,
0XA5,0X34,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XEE,0X68,0XDC,0XE4,0XD4,0X22,
0XD4,0X22,0XD4,0X22,0XEE,0XB1,0XFF,0XF8,0XEE,0XAF,0XD4,0X22,0XD4,0X22,0XD4,0X22,
0XDC,0XE4,0XED,0XE5,0XFE,0XA5,0XFE,0X23,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,0XFF,0XAB,0XFF,0XAE,0XA3,0XC7,0XFF,0XF6,
0XFF,0XF8,0XFF,0XFA,0XFF,0XF8,0XFF,0XF6,0XFF,0XF2,0XFF,0XAE,0XFF,0X6B,0XFF,0X69,
0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XE2,0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XA4,0XBB,0XC7,
0XFF,0XFF,0XFF,0XFF,0X81,0X80,0X59,0X00,0XFF,0XFF,0XF7,0X3A,0XBB,0XC6,0XFF,0XFA,
0XFF,0XF7,0XDD,0XAC,0XC4,0X6B,0XEE,0XB8,0XFF,0XFF,0XFF,0XFF,0XC4,0XF0,0XCB,0XC2,
0XFF,0X29,0XFF,0X29,0XFF,0X29,0XFF,0X29,0XCB,0X81,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X24,0XFE,0XE7,0XFF,0X69,
0XFF,0XAB,0XFF,0XAE,0XFF,0XF2,0XFF,0XF6,0XFF,0XF8,0XFF,0XFA,0XBB,0X81,0XEE,0X69,
0XA3,0X01,0XF6,0XAA,0XFF,0X2A,0XFF,0X69,0XFE,0XE7,0XFE,0XA5,0XFE,0X23,0XFD,0XE2,
0XC4,0X22,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0X25,0XFE,0XAA,0X9A,0X40,0XFE,0XEC,0XFE,0XEC,
0XFF,0X2B,0XFF,0X6A,0XFF,0XA8,0XFF,0XE7,0XFF,0XE7,0XFF,0XE9,0X9A,0X40,0XFF,0XAB,
0XFF,0X2B,0XFE,0XEB,0XFE,0X6B,0XFE,0X2A,0XFD,0XA8,0XFD,0X65,0XD4,0X23,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XD6,0XB9,0XE5,0X24,0XFF,0X28,0XFF,0X6A,0XFF,0XAC,0XAB,0X85,0X81,0X80,0XE6,0X6F,
0XFF,0XF6,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,0XFF,0XF2,0XE6,0X2B,0X81,0X80,0XAB,0X83,
0XFF,0X28,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XC4,0X22,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X29,0X45,
0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XAD,0X75,0X21,0X04,0XAD,0X75,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XA5,0X34,0X21,0X04,0XA5,0X34,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0X00,0X00,0X00,0X00,0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XA5,0X34,0XE7,0X3C,0XFF,0XFF,
0X39,0XC7,0X42,0X08,0X4A,0X49,0X5A,0XCB,0X63,0X0C,0X73,0X8E,0XFF,0XFF,0XFF,0XFF,
0X39,0XC7,0X42,0X08,0X4A,0X49,0X5A,0XCB,0X63,0X0C,0X73,0X8E,0XD6,0XBA,0XAD,0X75,
0XA5,0X34,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,0XFF,0X6A,0XFF,0XAD,0XFF,0XF0,
0XFF,0XF4,0XFF,0XF6,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,0XFF,0XF0,0XFF,0XAD,0XFF,0X6A,
0XFF,0X29,0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,0XFF,0X6A,0XFF,0XAD,0XFF,0XF0,0XFF,0XF4,
0XFF,0XF6,0XFF,0XF7,0XFF,0XF6,0XFF,0XF4,0XFF,0XF0,0XDD,0X66,0XBB,0X41,0XC4,0X22,
0XE5,0X64,0XFE,0X65,0XFE,0X23,0XFD,0XA2,0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE4,0XE5,0XAA,0XC1,
0XEE,0XF9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XD5,0X71,0XCC,0XEB,0XFF,0XF8,
0XFF,0XF6,0XFF,0XB2,0XDD,0X2A,0XA2,0X42,0XA2,0X83,0X9A,0X00,0XA2,0X44,0XCB,0XC1,
0XFE,0XA6,0XFE,0XA6,0XFE,0XA6,0XED,0XA4,0XCC,0X25,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,
0XFF,0X6A,0XFF,0XAD,0XFF,0XF0,0XFF,0XF4,0XFF,0XF6,0XFF,0XF7,0XBB,0X81,0XFF,0XAF,
0XA3,0X01,0XEE,0X28,0XFF,0X29,0XFF,0X29,0XFE,0XE7,0XFE,0X65,0XFE,0X23,0XFD,0XA2,
0XC3,0XC2,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEC,0XE6,0XFD,0XEC,0XFD,0XEE,0X9A,0X40,0X9A,0X40,
0X9A,0X40,0XFE,0XAC,0XFF,0X6A,0XFF,0XE9,0XFF,0XE9,0XFF,0XE9,0XFF,0XAA,0X9A,0X40,
0X9A,0X40,0X9A,0X40,0XFD,0X2F,0XFC,0XAF,0XFC,0XAC,0XFC,0XE8,0XD4,0X24,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XD6,0XB9,0XDC,0XE3,0XFE,0XE7,0XFF,0X29,0XFF,0X6A,0XFF,0XAD,0XFF,0XEF,0XFF,0XF2,
0XFF,0XF3,0XFF,0XF4,0XFF,0XF3,0XFF,0XF2,0XFF,0XEF,0XFF,0XAD,0XFF,0X6A,0XFF,0X29,
0XFE,0XE7,0XFE,0XA6,0XFE,0X64,0XFD,0XE2,0XC3,0XC2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X18,0XC3,
0X21,0X04,0XDE,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDE,0XFB,
0X00,0X00,0X00,0X00,0X08,0X41,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XB5,0XB6,0XD6,0XBA,0XFF,0XFF,
0X63,0X0C,0X4A,0X49,0X5A,0XCB,0X63,0X0C,0X73,0X8E,0X94,0XB2,0XFF,0XFF,0XFF,0XFF,
0X63,0X0C,0X4A,0X49,0X5A,0XCB,0X63,0X0C,0X73,0X8E,0X94,0XB2,0XCE,0X79,0XA5,0X34,
0XB5,0XB6,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,0XFF,0XAE,
0XFF,0XF0,0XFF,0XF2,0XFF,0XF3,0XFF,0XF2,0XFF,0XF0,0XFF,0XAE,0XFF,0XAB,0XFF,0X6A,
0XFF,0X28,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,0XD5,0X26,0XBB,0X41,
0XC3,0XC3,0XCC,0XA6,0XD5,0X28,0XCC,0XA5,0XB3,0X01,0XF7,0X7B,0XE6,0X76,0XD5,0X6F,
0XAA,0XC1,0XD4,0XA2,0XFE,0X23,0XFD,0XA1,0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE4,0XA5,0XDC,0X67,
0XC4,0X6B,0XEE,0XB8,0XFF,0XFF,0XFF,0XFF,0XD5,0XB2,0XA2,0X82,0XF7,0X34,0XFF,0XF6,
0XFF,0XF4,0XFF,0XB1,0XFF,0X2E,0XFE,0X2E,0XF4,0XED,0XFC,0XAF,0XEC,0X2C,0XCB,0X82,
0XED,0X23,0XFE,0X24,0XE5,0X23,0XCB,0XC1,0XF6,0XF8,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0XE4,0XFE,0XA6,0XFF,0X28,
0XFF,0X6A,0XFF,0XAB,0XFF,0XAE,0XFF,0XF0,0XFF,0XF2,0XFF,0XF3,0XBB,0X81,0XFF,0XAF,
0XA3,0X01,0XEE,0X27,0XFE,0XE8,0XFF,0X28,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,
0XBB,0X82,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEC,0XA7,0XFD,0X6D,0XFC,0XF0,0XFC,0XF1,0XFC,0XF0,
0XFD,0X70,0XFE,0X2E,0XFF,0X2C,0XFF,0XAA,0XFF,0XEA,0XFF,0XEA,0XFF,0XAB,0XFE,0XEC,
0XFE,0X2D,0XFD,0X2F,0XFC,0X71,0XFB,0XD1,0XFC,0X2E,0XFC,0XA9,0XDB,0XC6,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XD6,0XB9,0XDC,0XE4,0XFE,0XE6,0XFF,0X28,0XFF,0X69,0XCD,0X66,0XB4,0X24,0XAA,0XC0,
0XAA,0XC0,0XAA,0XC0,0XAA,0XC0,0XAA,0XC0,0XAA,0XC0,0XAA,0XC0,0XBC,0X64,0XD5,0X65,
0XFE,0XE6,0XFE,0X65,0XFE,0X23,0XFD,0XE2,0XBB,0XC2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X18,0XC3,
0X18,0XC3,0X5A,0XCB,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0X6B,0X4D,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XF7,0XBE,0X4A,0X49,
0X00,0X00,0X00,0X00,0X08,0X41,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XD6,0XBA,0XBD,0XF7,0XFF,0XFF,
0XB5,0XB6,0X5A,0XCB,0X63,0X0C,0X73,0X8E,0X84,0X30,0XDE,0XFB,0XFF,0XFF,0XFF,0XFF,
0XB5,0XB6,0X5A,0XCB,0X63,0X0C,0X73,0X8E,0X84,0X30,0XD6,0XBA,0XBD,0XF7,0X9C,0XF3,
0XD6,0XBA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,0XFF,0X28,0XFF,0X6A,0XFF,0XAB,
0XFF,0XAD,0XFF,0XAE,0XFF,0XEF,0XFF,0XAE,0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,0XFF,0X28,
0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XF5,0X21,0XBC,0X25,0XD6,0XB9,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,0XFF,0X28,0XD4,0XE4,0X9A,0X40,0X81,0X80,
0X89,0XC0,0XD4,0XEE,0XE6,0X77,0XF7,0X7B,0XF7,0X7B,0X81,0X80,0X81,0X80,0X81,0X80,
0X89,0X80,0XAB,0X01,0XFD,0XE2,0XF5,0X21,0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDC,0X66,0XFD,0X6C,
0XDB,0X8A,0XA2,0X42,0XA2,0X83,0X9A,0X00,0XB3,0X05,0XEE,0X70,0XFF,0XF3,0XFF,0XF2,
0XFF,0XF0,0XFF,0XAE,0XFF,0X6C,0XFE,0XEB,0XFE,0X6A,0XFD,0XEA,0XED,0X28,0XE4,0XA5,
0XC3,0X81,0XC3,0X41,0XC3,0XC6,0XEE,0XF9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X28,0XF6,0X25,0XFE,0XE7,
0XFF,0X28,0XFF,0X6A,0XFF,0XAB,0XFF,0XAD,0XFF,0X6B,0XDE,0X2B,0XBB,0X81,0XFF,0XAF,
0XA3,0X01,0XDD,0X25,0XFE,0XE7,0XFE,0XE7,0XFE,0XA5,0XFE,0X24,0XFD,0XE2,0XF5,0X21,
0XBC,0X25,0XD6,0XB9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE4,0XE9,0XFD,0X6A,0XFD,0X2E,0XFC,0XF0,0XFC,0XF0,
0XFD,0X70,0XFE,0X6E,0XFF,0X2C,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6B,0XFE,0XEC,
0XFD,0XED,0XFD,0X2E,0XFC,0XAF,0XFC,0X6E,0XFC,0XAB,0XFC,0XA7,0XD4,0X27,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XD6,0XB9,0XD4,0XE7,0XF6,0X25,0XFE,0XE7,0XD5,0X65,0XAB,0XC3,0XAA,0XC0,0XE6,0X33,
0XF6,0XF8,0XF7,0X7B,0XFF,0XFF,0XF7,0X7B,0XF6,0XF8,0XE6,0X33,0XAA,0XC0,0XA3,0X41,
0XCC,0XE3,0XFE,0X64,0XFD,0XE3,0XF5,0X61,0XC4,0X26,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X21,0X04,
0X18,0XC3,0X29,0X45,0X63,0X0C,0XE7,0X3C,0XFF,0XFF,0XFF,0XFF,0XE7,0X3C,0X6B,0X4D,
0X52,0X8A,0X6B,0X4D,0XE7,0X3C,0XFF,0XFF,0XFF,0XFF,0XDE,0XFB,0XFF,0XFF,0X84,0X30,
0X00,0X00,0X00,0X00,0X21,0X04,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X9C,0XF3,0XDE,0XFB,
0XFF,0XFF,0XB5,0XB6,0X73,0X8E,0X84,0X30,0XD6,0XBA,0XFF,0XFF,0X6B,0X4D,0X4A,0X49,
0XFF,0XFF,0XB5,0XB6,0X73,0X8E,0X84,0X30,0XCE,0X79,0XCE,0X79,0XAD,0X75,0XA5,0X34,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,0XFE,0XE7,0XFF,0X28,0XFF,0X6A,
0XFF,0X6A,0XFF,0XAB,0XFF,0XAB,0XFF,0XAB,0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,
0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XDC,0X61,0XC4,0XED,0XDE,0XFA,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,0XFE,0XE7,0XAA,0XC0,0X81,0X80,0X81,0X80,
0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,
0X81,0X80,0XA2,0X80,0XFD,0XA1,0XDC,0X61,0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE4,0XEA,0XFD,0X6A,
0XFD,0X2D,0XFC,0XF0,0XF4,0XAF,0XFD,0X70,0XFE,0X6F,0XFF,0X2F,0XFF,0XAF,0XFF,0XAE,
0XFF,0XAD,0XFF,0XAB,0XFF,0X6A,0XFF,0X29,0XFE,0XA7,0XFE,0X66,0XFD,0XE5,0XED,0X23,
0XDC,0XA1,0XB3,0X85,0XD6,0X79,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X72,0XE5,0X63,0XFE,0XA6,
0XFE,0XE7,0XFF,0X28,0XFF,0X6A,0XFF,0X6A,0XFF,0X6B,0XB2,0XC0,0XBB,0X81,0XFF,0XAF,
0XA3,0X01,0XC3,0X81,0XFF,0X29,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XA1,0XDC,0X61,
0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XED,0XAE,0XF5,0X66,0XFD,0XEA,0XFD,0XEC,0XFE,0X2C,
0XFE,0X6C,0XFE,0XEC,0XFF,0X6B,0XFF,0X6B,0XFF,0XAB,0XFF,0X6B,0XFF,0X6A,0XFE,0XEA,
0XFE,0X6A,0XFD,0XEA,0XFD,0XA9,0XFD,0X68,0XFD,0X26,0XEC,0X63,0XDD,0X2D,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XDE,0XFA,0XD5,0XAE,0XE5,0X64,0XFE,0XA6,0X8A,0X00,0X81,0X80,0X99,0XC0,0XEA,0XC0,
0XFB,0X00,0XFB,0X00,0XFB,0X00,0XFB,0X00,0XFB,0X00,0XEA,0XC0,0X99,0XC0,0X81,0X80,
0X8A,0X00,0XFE,0X23,0XFD,0XE2,0XDC,0X61,0XDD,0XB0,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X52,0X8A,
0X10,0X82,0X18,0XC3,0X29,0X45,0X39,0XC7,0X42,0X08,0X4A,0X49,0X4A,0X49,0X52,0X8A,
0X4A,0X49,0X4A,0X49,0X42,0X08,0X39,0XC7,0X29,0X45,0X18,0XC3,0XFF,0XFF,0XCE,0X79,
0X00,0X00,0X00,0X00,0X4A,0X49,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCE,0X79,0XB5,0XB6,
0XEF,0X7D,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X94,0XB2,0X84,0X30,
0XFF,0XFF,0XDE,0XFB,0XE7,0X3C,0XDE,0XFB,0XCE,0X79,0XB5,0XB6,0X9C,0XF3,0XD6,0XBA,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0XA6,0XFE,0XA6,0XFE,0XE7,0XFF,0X28,
0XFF,0X29,0XFF,0X69,0XFF,0X69,0XFF,0X69,0XFF,0X29,0XFF,0X29,0XDD,0X24,0XBB,0X41,
0XBB,0X41,0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XBB,0XC6,0XD6,0X37,0XEF,0X7C,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,0XFE,0XA6,0X9A,0X40,0X81,0X80,0X81,0X80,
0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,0X81,0X80,
0X89,0XC0,0XAA,0XC0,0XF5,0X21,0XBB,0XC6,0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEE,0X32,0XED,0X26,
0XFD,0XE9,0XFD,0XEB,0XFE,0X2C,0XFE,0X6C,0XFE,0XEC,0XFF,0X6C,0XFF,0XAB,0XFF,0XAB,
0XFF,0X6A,0XFF,0X6A,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X64,0XFD,0XE3,0XFD,0XA2,
0XDC,0X61,0XC4,0XED,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XBC,0XCC,0X64,0XFE,0X24,
0XFE,0XE7,0XFE,0XE7,0XFF,0X28,0XFF,0X29,0XFF,0X6B,0XC4,0X23,0XBB,0X81,0XFF,0XAF,
0XA3,0X01,0XCC,0X62,0XFF,0X29,0XFE,0X64,0XFE,0X23,0XFD,0XE2,0XF5,0X21,0XBB,0XC6,
0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X7B,0XDC,0X64,0XFE,0X67,0XFE,0X68,0XFE,0XA9,
0XFE,0XEA,0XFF,0X2A,0XFF,0X2A,0XFF,0X6A,0XFF,0X6A,0XFF,0X2A,0XFF,0X29,0XFE,0XE8,
0XFE,0XA7,0XFE,0X26,0XFD,0XE5,0XFD,0XA4,0XFD,0X62,0XCB,0X82,0XF7,0X3A,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XEF,0X7C,0XD5,0X6E,0XCB,0XC2,0XCB,0XC1,0XDC,0XE4,0X8A,0X00,0XB2,0X00,0XB2,0X00,
0X92,0X40,0X8A,0X40,0X8A,0X40,0X8A,0X40,0X92,0X40,0XB2,0X00,0XB2,0X00,0X8A,0X00,
0XDC,0XE4,0XCB,0XC1,0XCB,0XC1,0XC3,0XC4,0XF7,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X9C,0XF3,
0X08,0X41,0X10,0X82,0X21,0X04,0X29,0X45,0X31,0X86,0X39,0XC7,0X42,0X08,0X42,0X08,
0X42,0X08,0X39,0XC7,0X31,0X86,0X29,0X45,0X21,0X04,0X8C,0X71,0XFF,0XFF,0XFF,0XFF,
0X00,0X00,0X00,0X00,0X9C,0XF3,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XAD,0X75,
0XBD,0XF7,0XE7,0X3C,0XE7,0X3C,0XEF,0X7D,0XEF,0X7D,0XE7,0X3C,0XF7,0XBE,0XF7,0XBE,
0XD6,0XBA,0XCE,0X79,0XC6,0X38,0XBD,0XF7,0XAD,0X75,0X9C,0XF3,0XB5,0XB6,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XDC,0XE3,0XFE,0XA6,0XFE,0XA5,0XFE,0XA6,
0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFF,0X29,0XDD,0X24,0XBB,0X41,0XDC,0XE3,0XFF,0X28,
0XFE,0XE7,0XFD,0XE2,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XE5,0X64,0XFE,0X64,0X9A,0X40,0X81,0X80,0X81,0X80,
0X99,0XC0,0XEA,0XC0,0XEA,0XC0,0XFB,0X00,0XFB,0X00,0XFF,0XFF,0XFB,0X00,0XDB,0X00,
0XA2,0X80,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0XAD,0XD4,0X62,
0XD4,0X22,0XDC,0XE3,0XF6,0X27,0XFE,0XE8,0XFF,0X29,0XFF,0X29,0XFF,0X69,0XFF,0X69,
0XFF,0X29,0XFF,0X28,0XFE,0XE7,0XFE,0XA6,0XFE,0X64,0XFE,0X23,0XFD,0XE2,0XF5,0X21,
0XB3,0XC6,0XD6,0X37,0XEF,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE6,0X31,0XDC,0XE3,
0XFE,0X64,0XFE,0XA6,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFF,0X6B,0XBB,0X81,0XFF,0XAF,
0XA3,0X01,0XEE,0X27,0XFE,0X24,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XAB,0X01,0XC4,0XAC,
0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0X6B,0XE5,0X23,0XFE,0XA7,0XFE,0XA7,
0XFE,0XA8,0XFE,0XE8,0XFE,0XE9,0XFE,0XE9,0XFE,0XE9,0XFE,0XE8,0XFE,0XA8,0XFE,0XA7,
0XFE,0X65,0XFE,0X24,0XFD,0XE3,0XFD,0XA1,0XDC,0X61,0XD4,0XAA,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCB,0XC3,0XF6,0X67,0XFF,0X6A,0XF6,0X67,0XBB,0X41,0X8A,0X00,0X8A,0X40,
0XC4,0XA3,0XFE,0XE6,0XFE,0XE6,0XFE,0XA6,0XC4,0XA3,0X8A,0X40,0X8A,0X00,0XBB,0X41,
0XF6,0X67,0XFF,0X6A,0XF6,0X67,0XC3,0XC2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE7,0X3C,
0X39,0XC7,0X08,0X41,0X10,0X82,0X18,0XC3,0X29,0X45,0X29,0X45,0X31,0X86,0X31,0X86,
0X31,0X86,0X29,0X45,0X29,0X45,0X18,0XC3,0X10,0X82,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0X00,0X00,0X39,0XC7,0XE7,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0X9C,0XF3,0XBD,0XF7,0XDE,0XFB,0XE7,0X3C,0XDE,0XFB,0XDE,0XFB,0XDE,0XFB,0XD6,0XBA,
0XCE,0X79,0XC6,0X38,0XB5,0XB6,0XAD,0X75,0X9C,0XF3,0XA5,0X34,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,0XDC,0XA2,0XFE,0XA6,0XFE,0X64,
0XFE,0X65,0XFE,0XE6,0XFF,0X29,0XBB,0X41,0XDC,0XE3,0XFE,0XE7,0XFE,0XA6,0XFE,0X23,
0XFD,0XE2,0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,0XE5,0X24,0XFF,0XFF,0X9A,0X40,0X99,0XC0,
0XCA,0XC0,0XDB,0X00,0XFB,0X00,0XFB,0X00,0XFB,0X00,0XFB,0X00,0X9A,0X40,0XB3,0X40,
0XF5,0X63,0XCC,0X21,0XBC,0X29,0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XE5,0XEF,0XE5,0XA6,0XFF,0X6B,
0XFF,0X6B,0XEE,0X27,0XDC,0XA3,0XFE,0XA6,0XFE,0XE7,0XFE,0XE7,0XFE,0XE7,0XFF,0X6B,
0XA2,0X80,0XA2,0X80,0XFF,0X6B,0XFE,0X64,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XAB,0X01,
0XC4,0XEC,0XDE,0XFA,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCC,0X67,
0XDC,0XA2,0XFE,0X24,0XFE,0XA6,0XFE,0X65,0XFE,0XA5,0XBB,0X81,0XC4,0X64,0XFF,0XAF,
0XBB,0X81,0XED,0X63,0XFD,0XE3,0XFD,0XE2,0XFD,0X61,0XCC,0X21,0XBC,0X29,0XD6,0XB9,
0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XD4,0X22,0XE4,0XE3,0XFE,0X66,
0XFE,0X66,0XFE,0X66,0XFE,0XA7,0XFE,0XA7,0XB2,0XC1,0XB2,0XC1,0XFE,0XE8,0XFE,0X25,
0XFE,0X24,0XFD,0XE3,0XFD,0XA1,0XDC,0X21,0XC3,0X41,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XC3,0X81,0XFF,0X6A,0XFF,0X6B,0XFF,0X6A,0XC3,0X81,0XFE,0X64,0XFE,0X64,
0XFE,0X65,0XFE,0X65,0XFE,0X65,0XFE,0X64,0XFE,0X64,0XFE,0X23,0XFD,0XE3,0XC3,0X81,
0XFF,0X6A,0XFF,0X6B,0XFF,0X6A,0XC3,0X81,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0X7B,0XCF,0X00,0X00,0X08,0X41,0X10,0X82,0X18,0XC3,0X18,0XC3,0X21,0X04,0X21,0X04,
0X21,0X04,0X18,0XC3,0X18,0XC3,0X10,0X82,0X08,0X41,0XA5,0X34,0XFF,0XFF,0XA5,0X34,
0X00,0X00,0X7B,0XCF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0X9C,0XF3,0XB5,0XB6,0XD6,0XBA,0XD6,0XBA,0XCE,0X79,0XCE,0X79,0XC6,0X38,
0XBD,0XF7,0XB5,0XB6,0XAD,0X75,0X9C,0XF3,0X9C,0XF3,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XC4,0X67,0XD4,0X62,0XF6,0X25,
0XFE,0X23,0XDC,0XA2,0XBB,0X41,0XFE,0XE8,0XFE,0XE7,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,
0XE4,0XE1,0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDD,0X23,0XF5,0XA2,0XBB,0X81,
0XAB,0X00,0XFF,0XFF,0XA2,0X80,0XA2,0X80,0XAB,0X00,0XC3,0XC1,0XFD,0XA1,0XE4,0XE1,
0XFF,0XFF,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XD4,0X22,0XFF,0X6B,0XFF,0X6B,
0XFF,0X6B,0XFF,0X6B,0XCB,0XC2,0XFE,0X64,0XFE,0X65,0XFE,0XA5,0XFE,0XA5,0XFF,0X6B,
0XA2,0X80,0XA2,0X80,0XFF,0X6B,0XFE,0X23,0XFD,0XE2,0XFD,0X61,0XCC,0X21,0XBC,0X29,
0XD6,0XB9,0XEF,0X3C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XC4,0X67,0XD4,0X62,0XF5,0XA2,0XDD,0X23,0XBB,0X81,0XED,0X64,0XFF,0XAF,0XFF,0XAF,
0XED,0X23,0XBB,0X81,0XF5,0X62,0XE4,0XE1,0XAB,0X02,0XBC,0X6A,0XD6,0XB9,0XE6,0XFB,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCB,0XC1,0XE4,0XA2,
0XFE,0X25,0XFE,0X24,0XFE,0X24,0XFE,0X25,0XB2,0XC0,0X92,0X00,0XFE,0XE8,0XFD,0XE3,
0XFD,0XA2,0XFD,0X61,0XDC,0X20,0XBB,0X41,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XC3,0X81,0XFE,0XE8,0XFF,0X6A,0XFE,0XE8,0XC3,0X41,0XF5,0XE4,0XFE,0X23,
0XFE,0X23,0XFE,0X23,0XFE,0X23,0XFE,0X23,0XFD,0XE2,0XFD,0XE2,0XFD,0XA1,0XC3,0X81,
0XFE,0XE8,0XFF,0X6A,0XFE,0XE8,0XC3,0X41,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0X4A,0X49,0X00,0X00,0X00,0X00,0X08,0X41,0X08,0X41,0X10,0X82,0X10,0X82,
0X10,0X82,0X08,0X41,0X08,0X41,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X4A,0X49,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0X94,0XB2,0XAD,0X75,0X94,0XB2,0XD6,0XBA,0X94,0XB2,0XD6,0XBA,
0X94,0XB2,0XD6,0XBA,0X94,0XB2,0X94,0XB2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,0XB3,0X43,
0XCC,0X21,0XF6,0X26,0XFE,0XA7,0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XA1,0XCB,0XC1,
0XAB,0X85,0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCD,0X70,0XC4,0X64,0XCC,0X21,
0XED,0X65,0XFF,0XFF,0XFD,0XE2,0XFD,0XA2,0XF5,0X61,0XE4,0XA1,0XCB,0XC1,0XAB,0X85,
0XCD,0XB2,0XFF,0XFF,0XE7,0X3B,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XCB,0XC2,0XFF,0X29,0XFF,0X29,
0XFF,0X29,0XFF,0X29,0XCB,0X81,0XF5,0XE4,0XFE,0X23,0XFE,0X23,0XFE,0X24,0XFE,0X23,
0XFE,0X23,0XFE,0X23,0XFD,0XE2,0XFD,0XA1,0XE4,0XE1,0XAB,0X02,0XBC,0X6A,0XD6,0XB9,
0XE6,0XFB,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCD,0X70,0XB3,0X43,0XBB,0X81,0XED,0X64,0XFF,0XAF,0XFF,0XAF,0XFF,0X2B,
0XFE,0XA7,0XAB,0X41,0XD4,0X21,0XB3,0X85,0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XD4,0XEA,
0XCB,0XC2,0XEC,0XE2,0XF5,0X62,0XFD,0XA3,0XFE,0XE8,0XFE,0XE8,0XFD,0XA2,0XF5,0X21,
0XE4,0XA0,0XBB,0X41,0XD4,0XA9,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XCC,0X25,0XF5,0X63,0XFE,0X25,0XF5,0X63,0XBB,0X82,0XB3,0X43,0XCC,0X21,
0XE4,0XE1,0XF5,0X62,0XFD,0XA2,0XFD,0XA1,0XF5,0X61,0XE4,0XA1,0XCB,0XC1,0XBB,0X41,
0XF5,0X63,0XFE,0X25,0XF5,0X63,0XC3,0XC5,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XEF,0X7D,0X4A,0X49,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X4A,0X49,
0XEF,0X7D,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X9C,0XF3,0X94,0XB2,0XD6,0XBA,0X84,0X30,0XD6,0XBA,
0X84,0X30,0X9C,0XF3,0X94,0XB2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,
0XCD,0X70,0XBC,0X28,0XB3,0X85,0XAA,0XC1,0XAB,0X01,0XAB,0X44,0XB4,0X29,0XBC,0XAC,
0XD6,0XB9,0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XCD,0X70,
0XCD,0X2E,0XD5,0XB1,0XAA,0XC1,0XAB,0X01,0XAB,0X44,0XB4,0X29,0XBC,0XAC,0XD6,0XB9,
0XDE,0XFA,0XE7,0X7C,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XD4,0X65,0XED,0XA5,0XFE,0XA6,
0XFE,0XA6,0XFE,0XA6,0XC3,0X81,0XB3,0X43,0XCC,0X21,0XE4,0XE2,0XF5,0X62,0XFD,0XE2,
0XFD,0XA2,0XF5,0X61,0XE4,0XA1,0XCB,0XC1,0XAB,0X85,0XCD,0XB2,0XDE,0XB9,0XE7,0X3B,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XEF,0X3B,0XBB,0X81,0XFF,0XAF,0XFF,0XAF,0XFF,0X2B,0XFE,0XA7,
0XFD,0XE4,0XAB,0X41,0XCC,0XA7,0XD6,0X36,0XDE,0XFA,0XF7,0XBE,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XF7,0X39,0XDD,0X2C,0XCB,0XC5,0XC3,0X42,0XBB,0X41,0XBB,0X41,0XBB,0X41,0XC3,0XC5,
0XD4,0XEC,0XF7,0X3A,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XE6,0X33,0XBB,0X00,0XBB,0X00,0XBB,0X00,0XCD,0X70,0XD6,0X78,0XC5,0X30,
0XB4,0X28,0XB3,0X85,0XAA,0XC1,0XA3,0X01,0XAB,0X44,0XB4,0X28,0XBC,0XAD,0XDD,0XF2,
0XBB,0X00,0XBB,0X00,0XBB,0X00,0XE6,0X33,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0X7B,0XCF,0X39,0XC7,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X39,0XC7,0X7B,0XCF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XA5,0X34,0X84,0X30,0X73,0X8E,0X84,0X30,
0X73,0X8E,0X94,0XB2,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XDF,0X3E,0XFF,0XFF,0XFF,0XFF,
};