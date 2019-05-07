/*-----------------------------------------------
  名称：16x16汉字屏流动显示
  论坛：www.doflye.net
  编写：shifang
  日期：2009.5
  修改：无
  硬件：LY-51S V4.0 单片机开发板
  内容：
        P1.0-P1.3分别连接A、B、C、D，用于控制16行，通过2路HC138控制
		SER接P1.4 通过2片HC595控制
        CLK时钟连接P1.5
        LAT锁存连接P1.6
		OE接P1.7

  现象：使用普通速度51系列单片机。1个汉字显示屏，向左流动显示多个汉字，重复循环显示。
------------------------------------------------*/
#include<reg52.h>

sbit J32_LAT = P1^6;       //锁存引脚
sbit J32_SER = P1^4;       //数据引脚
sbit J32_CLK = P1^5;       //时钟引脚
sbit J32_OE  = P1^7;       //使能引脚
sbit J37_LAT = P1^3;       //锁存引脚
sbit J37_SER = P1^1;       //数据引脚
sbit J37_CLK = P1^2;       //时钟引脚
sbit KEY=P3^3;
unsigned char MoveBitNum,MoveBitNumtemp,IncNum;//移动位数,临时移动位数，大于一个字节增加字节数
unsigned int HzNum;//汉字个数
unsigned char buff[10];
unsigned int flag;
/*-----------------------------------------------
             16x16汉字取模数据
------------------------------------------------*/
unsigned char code hztest1[][32]=    //取模选择字节正序
{


	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //前面一屏大小的空字符，不显示，这里根据屏的大小修改，这个是1个汉字屏
	//所以写入1个汉字
	/*--  文字:  我  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x04,0x40,0x0E,0x50,0x78,0x48,0x08,0x48,0x08,0x40,0xFF,0xFE,0x08,0x40,0x08,0x44,
0x0A,0x44,0x0C,0x48,0x18,0x30,0x68,0x22,0x08,0x52,0x08,0x8A,0x2B,0x06,0x10,0x02,

/*--  文字:  爱  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x00,0x08,0x01,0xFC,0x7E,0x10,0x22,0x10,0x11,0x20,0x7F,0xFE,0x42,0x02,0x82,0x04,
0x7F,0xF8,0x04,0x00,0x07,0xF0,0x0A,0x10,0x11,0x20,0x20,0xC0,0x43,0x30,0x1C,0x0E,

/*--  文字:  你  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x08,0x80,0x08,0x80,0x08,0x80,0x11,0xFE,0x11,0x02,0x32,0x04,0x34,0x20,0x50,0x20,
0x91,0x28,0x11,0x24,0x12,0x24,0x12,0x22,0x14,0x22,0x10,0x20,0x10,0xA0,0x10,0x40,

/*--  文字:  三  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x00,0x00,0x00,0x00,0x7F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F,0xF8,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFE,0x00,0x00,0x00,0x00,

/*--  文字:  千  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x00,0x10,0x00,0xF8,0x3F,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0xFF,0xFE,
0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,

/*--  文字:  遍  --*/
/*--  宋体12;  此字体下对应的点阵为：宽x高=16x16   --*/
0x00,0x80,0x20,0x40,0x13,0xFC,0x12,0x04,0x02,0x04,0x03,0xFC,0xF2,0x00,0x13,0xFC,
0x13,0x54,0x15,0x54,0x15,0xFC,0x19,0x54,0x11,0x44,0x11,0x0C,0x28,0x00,0x47,0xFE,





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, //尾部的一屏的空字符，用于清屏，参数和屏大小有关。
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,


};
unsigned char code hztest2[][32]=    //取模选择字节正序
{


0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*" ",0*/
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*" ",1*/
	//所以写入1个汉字		0x
	0xBF,0xFF,0xBF,0xFF,0xDF,0xC0,0xDF,0xEE,
	0xEF,0xEE,0xD7,0xEE,0xDB,0xC0,0xDC,0xFF,
	0xDB,0x00,0xD7,0xFE,0xEF,0xEE,0xDF,0xDE,
	0xDF,0xE0,0xBF,0xFF,0xBF,0xFF,0xFF,0xFF,/*"命",0*/

	0xBF,0xFF,0xBF,0xBF,0xBD,0xDF,0x33,0xE0,
	0xFF,0xDF,0xDF,0xBB,0xDD,0xB1,0xDD,0xBA,
	0x5D,0xBB,0x9D,0xBB,0xDD,0xBB,0xDD,0xBA,
	0xDD,0xB9,0xDF,0xB3,0xFF,0xBF,0xFF,0xFF,/*"运",1*/

	0xFD,0xF7,0xFD,0xFB,0xFD,0xFD,0xFD,0xFE,
	0x3D,0x00,0x4D,0xBF,0x71,0xBF,0x7D,0xBF,
	0x7D,0xBF,0x7D,0xBF,0x7D,0xBF,0x7D,0xBF,
	0x7D,0x00,0xFD,0xFF,0xFD,0xFF,0xFF,0xFF,/*"石",2*/

	0xFF,0xFF,0xEF,0xBF,0xEF,0xDF,0xEF,0xEF,
	0xEF,0xEF,0xEF,0xD7,0xEE,0xBB,0xE9,0xBD,
	0xEF,0xBE,0x6F,0xBF,0xAF,0xBF,0xCF,0xBF,
	0xEF,0xBF,0xFF,0xBF,0xFF,0xBF,0xFF,0xFF,/*"之",3*/

	0xFF,0xFF,0x07,0x00,0xFE,0xFF,0xF9,0xFF,
	0xFF,0xFF,0xFF,0xFF,0xFD,0xFF,0xFD,0xFF,
	0xFD,0xFF,0xFD,0xFF,0xFD,0xFF,0xFD,0xBF,
	0xFD,0x7F,0x01,0x80,0xFF,0xFF,0xFF,0xFF,/*"门",4*/





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*" ",0*/
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,/*" ",1*/


};

unsigned char code a[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
unsigned char code b[]={0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};


/*-----------------------------------------------
             向595写入一个字节 单红色
------------------------------------------------*/
void InputByte1( unsigned  char DataR1) //写一个字节
{
	unsigned  char i;
	for(i=8; i>0; i--)
	{
		J32_SER = (DataR1&0x01);
		J32_CLK = 0;
		J32_CLK = 1;
		DataR1 = DataR1 >> 1;
	}
}
void InputByte2( unsigned  char DataR1) //写一个字节
{
	unsigned  char i;
	for(i=8; i>0; i--)
	{
		J37_SER = (DataR1&0x01);
		J37_CLK = 0;
		J37_CLK = 1;
		DataR1 = DataR1 >> 1;
	}
}

void Input2Byte1(unsigned char dat1,unsigned char dat2)
{
	unsigned  char i;
	for(i=8; i>0; i--)
	{
		J37_SER = (dat1&0x01);
		J37_CLK = 0;
		J37_CLK = 1;
		dat1 = dat1 >> 1;
	}
		for(i=8; i>0; i--)
	{
		J37_SER = (dat2&0x01);
		J37_CLK = 0;
		J37_CLK = 1;
		dat2 = dat2 >> 1;
	}


}
void Input2Byte2(unsigned char dat1,unsigned char dat2)
{
	unsigned  char i;
		for(i=8; i>0; i--)
	{
		J32_SER = (dat2&0x01);
		J32_CLK = 0;
		J32_CLK = 1;
		dat2 = dat2 >> 1;
	}
	for(i=8; i>0; i--)
	{
		J32_SER = (dat1&0x01);
		J32_CLK = 0;
		J32_CLK = 1;
		dat1 = dat1 >> 1;
	}

}

/*-----------------------------------------------
                    延时程序
------------------------------------------------*/
void Delay(unsigned int t)
{
	while(--t);
}
void DelayUs2x(unsigned char t)
{
 while(--t);
}
void DelayMs(unsigned char t)
{

 while(t--)
 {
     //大致延时1mS
     DelayUs2x(245);
	 DelayUs2x(245);
 }
}


/*-----------------------------------------------
                      主程序
------------------------------------------------*/
main()
{
	unsigned char count;//16行扫描数据，范围0-15
	unsigned int i, j;
	unsigned char temp;
	unsigned int inter;
	KEY=1; //按键输入端口电平置高
	inter=0;
	flag=1;
	//计算数组的元素个数，最终需要的是汉字的个数
	while(1){
	if(flag==1){
	int lenTotal = sizeof(hztest1) / sizeof(int);  //所有
	int lenLow = sizeof(hztest1[0]) / sizeof(int);  //低位
	int lenHigh = lenTotal / lenLow;  //高位
	while(1)
	{
		if(!KEY)  //如果检测到低电平，说明按键按下
 {
DelayMs(10); //延时去抖，一般10-20ms
	if(!KEY)     //再次确认按键是否按下，没有按下则退出
	{
		 while(!KEY);//如果确认按下按键等待按键释放，没有释放则一直等待
			{
			flag++;//释放则执行需要的程序
	 		inter=1;
	}
}}
 if(inter==1){
	 inter=0;
	 break;
 }
		i++;
		if(i==180)//更改流动速度,1T单片机和12T单片机速度大约5-8倍，注意更改参数
		{
			i=0;
			MoveBitNum++;
			if(MoveBitNum==16)//每次移动完一个汉字大小后循环
			{
				MoveBitNum=0;
				HzNum+=1;    //调用下一个汉字
				if(HzNum>=lenHigh-1)//需要显示的汉字个数，包括前面的一屏空字符的个数，后面清屏的空字符不包含在内,这里是(汉字个数+1)
					HzNum=0;   //完全显示完后循环调用
			}
		}


		Delay(1);//控制扫描频率
		//读取汉字对应屏幕缓冲区的数据，不同大小的屏幕不一样
		for(j=0; j<2; j++) //取每个汉字的前2个字节，
		{
			//汉字个数+1
			buff[j+j+1]=hztest1[HzNum+j][count+count+1];  //每次移动完一个汉字后，选择下一个汉字
			buff[j+j]=hztest1[HzNum+j][count+count];


		}
		//判断移动的位数
		if(MoveBitNum<8)                   //  判读移动距离是大于一个字节还是小于一个字节，因为一个字节左移右移最大只能8位
		{
			IncNum=0;
			MoveBitNumtemp=MoveBitNum;
		}
		else
		{
			IncNum=1;    //大于8就减去8得到的数值还是小于8
			MoveBitNumtemp=MoveBitNum-8;
		}
		J32_LAT=0;       //锁存无效
		J37_LAT=0;
		if(count<8)
			{
				if(count==0){Input2Byte1(0xff,a[7]);}
				else{Input2Byte1(a[count-1],0xff);}

			}
			else
			{
				if(count==8){Input2Byte1(a[7],0xff);}
				else{Input2Byte1(0xff,a[count-8-1]);}
			}
		J37_LAT = 1;
		J37_LAT=0;
		for(j=2; j>0; j--)        //按bit的方式移动缓冲区的内容，然后输出到595，即取出的数值每个字节左移一定的位数，
		{
			//后面左移出的数据整合到前面的字节中，保持数据的连续性
			temp=(buff[j-1+IncNum]<<MoveBitNumtemp)|(buff[j+IncNum]>>(8-MoveBitNumtemp));
			InputByte1(temp);//输出到595
		}//8个字节传输完锁存输出
		//控制使能端
		J32_OE  = 1;
		J32_LAT=1;      //锁存有效，此时一行的数据显示到屏上
		J32_OE = 0;
		count++;
		if(count==16)
			count=0;
	}}
	if(flag==2){
		int lenTotal = sizeof(hztest1) / sizeof(int);  //所有
int lenLow = sizeof(hztest1[0]) / sizeof(int);  //低位
int lenHigh = lenTotal / lenLow;  //高位

while(1)
{
	if(!KEY)  //如果检测到低电平，说明按键按下
{
DelayMs(10); //延时去抖，一般10-20ms
if(!KEY)     //再次确认按键是否按下，没有按下则退出
{
	 while(!KEY);//如果确认按下按键等待按键释放，没有释放则一直等待
		{
		flag++;//释放则执行需要的程序
 		inter=1;
}
}}
if(inter==1){
 inter=0;
 break;
}

	i++;
	if(i==180)//更改流动速度,1T单片机和12T单片机速度大约5-8倍，注意更改参数
	{
		i=0;
		MoveBitNum++;
		if(MoveBitNum==16)//每次移动完一个汉字大小后循环
		{
			MoveBitNum=0;
			HzNum+=1;    //调用下一个汉字
			if(HzNum>=lenHigh-1)//需要显示的汉字个数，包括前面的一屏空字符的个数，后面清屏的空字符不包含在内,这里是(汉字个数+1)
				HzNum=0;   //完全显示完后循环调用
		}
	}


	Delay(1);//控制扫描频率
	//读取汉字对应屏幕缓冲区的数据，不同大小的屏幕不一样
	for(j=0; j<2; j++) //取每个汉字的前2个字节，
	{
		//汉字个数+1
	//	buff[j+j+1]=hztest[HzNum+j][count+count+1];  //每次移动完一个汉字后，选择下一个汉字
		//buff[j+j]=hztest[HzNum+j][count+count];
		buff[j+j]=hztest1[HzNum+j][count+count+1];  //每次移动完一个汉字后，选择下一个汉字
		buff[j+j+1]=hztest1[HzNum+j][count+count];

	}
	//判断移动的位数
	if(MoveBitNum<8)                   //  判读移动距离是大于一个字节还是小于一个字节，因为一个字节左移右移最大只能8位
	{
		IncNum=0;
		MoveBitNumtemp=MoveBitNum;
	}
	else
	{
		IncNum=1;    //大于8就减去8得到的数值还是小于8
		MoveBitNumtemp=MoveBitNum-8;
	}
	J32_LAT=0;       //锁存无效
	J37_LAT=0;
			if(count<8)
				{
					if(count==0){Input2Byte1(0xff,a[7]);}
					else{Input2Byte1(a[count-1],0xff);}

				}
				else
				{
					if(count==8){Input2Byte1(a[7],0xff);}
					else{Input2Byte1(0xff,a[count-8-1]);}
				}
	J37_LAT = 1;
	J37_LAT=0;
	for(j=0; j<3; j++)        //按bit的方式移动缓冲区的内容，然后输出到595，即取出的数值每个字节左移一定的位数，
	{
		//后面左移出的数据整合到前面的字节中，保持数据的连续性
		//temp=(buff[j-1+IncNum]<<MoveBitNumtemp)|(buff[j+IncNum]>>(8-MoveBitNumtemp));
		temp = (buff[j-1+IncNum]>>MoveBitNumtemp|buff[j+IncNum]<<(8-MoveBitNumtemp));
		InputByte1(temp);//输出到595
	}//8个字节传输完锁存输出
	//控制使能端
	J32_OE  = 1;
	J32_LAT=1;      //锁存有效，此时一行的数据显示到屏上
	J32_OE = 0;
	count++;
	if(count==16)
		count=0;
}
}
  if(flag==3){
		int lenTotal = sizeof(hztest2) / sizeof(int);  //所有
int lenLow = sizeof(hztest2[0]) / sizeof(int);  //低位
int lenHigh = lenTotal / lenLow;  //高位


	while(1)
	{
		if(!KEY)  //如果检测到低电平，说明按键按下
	{
	DelayMs(10); //延时去抖，一般10-20ms
	if(!KEY)     //再次确认按键是否按下，没有按下则退出
	{
		 while(!KEY);//如果确认按下按键等待按键释放，没有释放则一直等待
			{
			flag++;//释放则执行需要的程序
	 		inter=1;
	}
	}}
	if(inter==1){
	 inter=0;
	 if(flag>3){flag=1;}
	 break;
	}

	i++;
	if(i==180)//更改流动速度,1T单片机和12T单片机速度大约5-8倍，注意更改参数
	{
		i=0;
		MoveBitNum++;
		if(MoveBitNum==16)//每次移动完一个汉字大小后循环
		{
			MoveBitNum=0;
			HzNum+=1;    //调用下一个汉字
			if(HzNum>=lenHigh-1)//需要显示的汉字个数，包括前面的一屏空字符的个数，后面清屏的空字符不包含在内,这里是(汉字个数+1)
				HzNum=0;   //完全显示完后循环调用
		}
	}


	Delay(1);//控制扫描频率
	//读取汉字对应屏幕缓冲区的数据，不同大小的屏幕不一样
	for(j=0; j<2; j++) //取每个汉字的前2个字节，
	{
		//汉字个数+1
		buff[j+j]=hztest2[HzNum+j][count+count+1];  //每次移动完一个汉字后，选择下一个汉字
		buff[j+j+1]=hztest2[HzNum+j][count+count];


	}
	//判断移动的位数
	if(MoveBitNum<8)                   //  判读移动距离是大于一个字节还是小于一个字节，因为一个字节左移右移最大只能8位
	{
		IncNum=0;
		MoveBitNumtemp=MoveBitNum;
	}
	else
	{
		IncNum=1;    //大于8就减去8得到的数值还是小于8
		MoveBitNumtemp=MoveBitNum-8;
	}
	J32_LAT=0;       //锁存无效
	J37_LAT=0;
			if(count<8)
				{
					Input2Byte2(b[count],0x00);

				}
				else
				{
					Input2Byte2(0x00,b[count-8]);
				}
	J37_LAT = 1;
	J37_LAT=0;
	for(j=2; j>0; j--)        //按bit的方式移动缓冲区的内容，然后输出到595，即取出的数值每个字节左移一定的位数，
	{
		//后面左移出的数据整合到前面的字节中，保持数据的连续性
		temp=(buff[j-1+IncNum]<<MoveBitNumtemp)|(buff[j+IncNum]>>(8-MoveBitNumtemp));
		InputByte2(temp);//输出到595
	}//8个字节传输完锁存输出
	//控制使能端
	J32_OE  = 1;
	J32_LAT=1;      //锁存有效，此时一行的数据显示到屏上
	J32_OE = 0;
	count++;
	if(count==16)
		count=0;
}
	}
}
}
