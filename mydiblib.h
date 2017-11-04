#include "dibapi.h"
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#define pi 3.1415926
#define M 15
#define N 24
using namespace std;


typedef struct
{
	int	id;
	bool isWhite;
	bool isVisited;
	bool isEdge;
	bool isSum;
}PelsInfo;           //点的信息

typedef struct
{
	bool isCircularity;
	int area;
    double perimeter;
	
    int dec_code;
	CString *bin_code;
	
	int center_x;
	int center_y;
	int xsum;
	int ysum;
	int count;
}RegionInfo;           //区域信息

//声明一些全局变量
HDIB g_hDIB;
RegionInfo *g_RegionInfo;
PelsInfo *g_PelsInfo;
CString strPathName;
CString strFileTitle;
bool isFileOpen;
int region_id=0;               //区域编号
double average_radius=0;
bool is256=0;
int step=0;
int pic=0;
/********************************signction declaration*************************************/
void ClearAll(CDC* pDC);//清除屏幕
void DisplayDIB(CDC* pDC,HDIB hDIB);//在屏幕上显示位图

void GrayToWhiteBlack(HDIB hDIB);//将灰度图二值化
void Set256toGray(HDIB hDIB);//将256色位图转为灰度图
void RemoveNoise(HDIB hDIB);     //去除离散噪声点
void RegionSign(HDIB hDIB);    //图像分割
void EdgeIdentify(HDIB hDIB);     //提取边缘

void AreaandPerimeter(HDIB hDIB);   //求周长和面积
void GetCenter(HDIB hDIB);            //求出中心坐标点
void CircularityIdentify(HDIB hDIB);       //识别圆形标记并求圆平均半径
void DelErr(HDIB hDIB);            //去掉错误中心圆
void Codejiema(HDIB hDIB);         //编码点解码

void Sign(int i,int j,LONG lWidth,LONG lHeight);   //标记一个白色区域中所有的点
void sum(int i,int j,LONG lWidth,LONG lHeight);   //求椭圆边界所有像素点的x,y坐标和以及像素总数
int round(double f);          //取整（4舍5入）

void ImageProcess(HDIB hDIB);       //图像预处理
void CenterIdentify(HDIB hDIB);    //中心圆识别

void Codematch();              //编码点的匹配
void GetCrossRatio();          //求交比不变量
int zhongjian(int c1,int c2,int aa[100][6]);  //求2幅图片中匹配的非编码点，存于 aa[100][6]
int Match(int c1,int c2,int aa[100][6]);     //求2幅图片中匹配的编码点，存于 aa[100][6]
void Incodematch();      //非编码点的匹配
vector <string> Getsubstr(string str);    //分割字符串
/***********************************implementation*************************************/

void DisplayDIB(CDC* pDC,HDIB hDIB)
{
	BYTE* lpDIB=(BYTE*)::GlobalLock((HGLOBAL)hDIB);
	// 获取DIB宽度和高度
	int cxDIB =  ::DIBWidth((char*) lpDIB);
	int cyDIB =  ::DIBHeight((char*)lpDIB);
	CRect rcDIB,rcDest;
	rcDIB.top = rcDIB.left = 0;
	rcDIB.right = cxDIB;
	rcDIB.bottom = cyDIB;
	//设置目标客户区输出大小尺寸
	rcDest = rcDIB;
	//CDC* pDC=GetDC();
	ClearAll(pDC);
	//在客户区显示图像
	::PaintDIB(pDC->m_hDC,rcDest,hDIB,rcDIB,NULL);
	::GlobalUnlock((HGLOBAL)hDIB);
}

void ClearAll(CDC *pDC)
{
	CRect rect;
	rect.left =0;rect.top =0;rect.right =2000;rect.bottom =1000;
	CPen pen;
	pen.CreatePen (PS_SOLID,1,RGB(255,255,255));
	pDC->SelectObject (&pen);
	pDC->Rectangle (&rect);
	::DeleteObject (pen);
}

//  将256色位图转化为灰度图
void Set256toGray(HDIB hDIB)
{
	if(!is256)
	{
		AfxMessageBox("非256色位图!");
		return;
	}
	

	LPSTR	lpDIB;
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);// 由DIB句柄得到DIB指针并锁定DIB
	LPSTR   lpDIBBits;// 指向DIB象素数据区的指针
	BYTE *	lpSrc;// 指向DIB象素的指针
	LONG	lWidth;	// 图像宽度
	LONG  	lHeight;// 图像高度	
	LONG	lLineBytes;	// 图像每行的字节数
	LPBITMAPINFO lpbmi;	// 指向BITMAPINFO结构的指针（Win3.0）
	LPBITMAPCOREINFO lpbmc;// 指向BITMAPCOREINFO结构的指针
	lpbmi = (LPBITMAPINFO)lpDIB;// 获取指向BITMAPINFO结构的指针（Win3.0）
	lpbmc = (LPBITMAPCOREINFO)lpDIB;// 获取指向BITMAPCOREINFO结构的指针
	BYTE bMap[256];// 灰度映射表
	
	// 计算灰度映射表（保存各个颜色的灰度值），并更新DIB调色板
	int	i,j;
	for (i = 0; i < 256; i ++)
	{
		// 计算该颜色对应的灰度值
		bMap[i] = (BYTE)(0.299 * lpbmi->bmiColors[i].rgbRed +
			
			0.587 * lpbmi->bmiColors[i].rgbGreen +
			
			0.114 * lpbmi->bmiColors[i].rgbBlue + 0.5);			
		// 更新DIB调色板红色分量
		lpbmi->bmiColors[i].rgbRed = i;	
		
		// 更新DIB调色板绿色分量
		lpbmi->bmiColors[i].rgbGreen = i;	
		
		// 更新DIB调色板蓝色分量
		lpbmi->bmiColors[i].rgbBlue = i;
		
		// 更新DIB调色板保留位
		lpbmi->bmiColors[i].rgbReserved = 0;
		
	}
	// 找到DIB图像象素起始位置
	lpDIBBits = ::FindDIBBits(lpDIB);
	
	// 获取图像宽度
	lWidth = ::DIBWidth(lpDIB);	
	
	// 获取图像高度
	lHeight = ::DIBHeight(lpDIB);	
	
	// 计算图像每行的字节数
	lLineBytes = WIDTHBYTES(lWidth * 8);	
	
	// 更换每个象素的颜色索引（即按照灰度映射表换成灰度值）
	
	//逐行扫描
	for(i = 0; i < lHeight; i++)
	{
		
		//逐列扫描
		for(j = 0; j < lWidth; j++)
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - i) + j;
			
			// 变换
			*lpSrc = bMap[*lpSrc];
		}
	}
	
	//解除锁定
	::GlobalUnlock ((HGLOBAL)hDIB);
}

//二值化
void GrayToWhiteBlack(HDIB hDIB)
{


	LPSTR	lpDIB;// 指向DIB的指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);// 由DIB句柄得到DIB指针并锁定DIB
	LPSTR   lpDIBBits;// 指向DIB象素数据区的指针
	BYTE *	lpSrc;// 指向DIB象素的指针	
	LONG	lWidth;// 图像宽度
	LONG	lHeight;// 图像高度
	LONG	lLineBytes;	// 图像每行的字节数
	lpDIBBits = ::FindDIBBits(lpDIB);// 找到DIB图像象素起始位置
	lWidth = ::DIBWidth(lpDIB);	// 获取图像宽度
	lHeight = ::DIBHeight(lpDIB);// 获取图像高度	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数	
	
	
	int i,j;
    //二值化
	for(i = 0; i < lHeight; i++)
	{
		for(j = 0; j < lWidth; j++)
		{
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * i + j;
			
			// 二值化处理
			
			//大于90，设置为255，即白点
			if(*lpSrc>90) *lpSrc=255;
			
			//否则设置为0，即黑点
			else *lpSrc=0;	
		}	
	}
	
	//解除锁定
	::GlobalUnlock((HGLOBAL)hDIB);
}

void RemoveNoise(HDIB hDIB)
{


	LPSTR	lpDIB;// 指向DIB的指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);// 由DIB句柄得到DIB指针并锁定DIB
	LPSTR   lpDIBBits;// 指向DIB象素数据区的指针
	BYTE *	lpSrc;// 指向DIB象素的指针	
	LONG	lWidth;// 图像宽度
	LONG	lHeight;// 图像高度
	LONG	lLineBytes;	// 图像每行的字节数
	lpDIBBits = ::FindDIBBits(lpDIB);// 找到DIB图像象素起始位置
	lWidth = ::DIBWidth(lpDIB);	// 获取图像宽度
	lHeight = ::DIBHeight(lpDIB);// 获取图像高度	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数		
	
	int i,j;
	//去噪
	for(i = 0; i < lHeight; i++)
	{
		for(j = 0; j < lWidth; j++)
		{
			
			// 指向DIB第i行，第j个象素的指针
			if((i!=0)&&(j!=0)&&(i!=lHeight-1)&&(j!=lWidth-1))
			{
				int num=0;
				BYTE color=0;
				
				lpSrc=( unsigned char*)lpDIBBits + lLineBytes * i + j;
				color=*lpSrc;
				
				lpSrc=( unsigned char*)lpDIBBits + lLineBytes * (i+1) + j;
				if (*lpSrc != color)
				{
					num++; 
				}
				lpSrc=( unsigned char*)lpDIBBits + lLineBytes * (i-1) + j;
				if (*lpSrc != color)
				{
					num++; 
				}
				lpSrc=(unsigned char*)lpDIBBits + lLineBytes * i + (j+1);
				if (*lpSrc != color)
				{
					num++; 
				}
				lpSrc=(unsigned char*)lpDIBBits + lLineBytes * i + (j-1);
				if (*lpSrc != color)
				{
					num++; 
				}
				
				if(num>=3)
				{
					lpSrc=( unsigned char*)lpDIBBits + lLineBytes * i + j;
					if(color==0) *lpSrc=255;
					else *lpSrc=0;
				}
			}
			
		}
		
	}
	
	//解除锁定
	::GlobalUnlock((HGLOBAL)hDIB);	
}


void RegionSign(HDIB hDIB)
{

	LPSTR	lpDIB;// 指向DIB的指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);// 由DIB句柄得到DIB指针并锁定DIB
	LPSTR   lpDIBBits;// 指向DIB象素数据区的指针
	BYTE *	lpSrc;// 指向DIB象素的指针	
	LONG	lWidth;// 图像宽度
	LONG	lHeight;// 图像高度
	LONG	lLineBytes;	// 图像每行的字节数
	lpDIBBits = ::FindDIBBits(lpDIB);// 找到DIB图像象素起始位置
	lWidth = ::DIBWidth(lpDIB);	// 获取图像宽度
	lHeight = ::DIBHeight(lpDIB);// 获取图像高度	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数
	
	int i,j;
	
	//初始化g_PelsInfo
	g_PelsInfo = (PelsInfo*)malloc(lWidth*lHeight*sizeof(PelsInfo));
	for(i = 0; i < lHeight; i++)
	{
		for(j = 0; j < lWidth; j++)
		{
			
			// 指向DIB第i行，第j个象素的指针
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * i + j;
			if(*lpSrc==255)
			{
				g_PelsInfo[i*lWidth+j].isWhite=1;
			}
			else
			{
				g_PelsInfo[i*lWidth+j].isWhite=0;
			}
			g_PelsInfo[i*lWidth+j].id=0;
			g_PelsInfo[i*lWidth+j].isVisited=0;
			g_PelsInfo[i*lWidth+j].isEdge=0;
            g_PelsInfo[i*lWidth+j].isSum=0;
		} 
		
	}
	
	
	//图形分割
	for(i = 0; i < lHeight; i++)
	{
		for(j = 0; j < lWidth; j++)
		{
			// 指向DIB第i行，第j个象素的指针
	    	//	lpSrc = (unsigned char*)lpDIBBits + lLineBytes * i + j;
			if((g_PelsInfo[i*lWidth+j].isWhite)&&
				!(g_PelsInfo[i*lWidth+j].isVisited))
			{
				region_id++;
				Sign(i,j,lWidth,lHeight);	
			}
		} 	
	}
	
	//解除锁定
	::GlobalUnlock((HGLOBAL)hDIB);
}

void EdgeIdentify(HDIB hDIB)
{
	
	// TODO: Add your command handler code here



	LPSTR	lpDIB;// 指向DIB的指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);// 由DIB句柄得到DIB指针并锁定DIB
	LPSTR   lpDIBBits;// 指向DIB象素数据区的指针
	BYTE *	lpSrc;// 指向DIB象素的指针	
	LONG	lWidth;// 图像宽度
	LONG	lHeight;// 图像高度
	LONG	lLineBytes;	// 图像每行的字节数
	lpDIBBits = ::FindDIBBits(lpDIB);// 找到DIB图像象素起始位置
	lWidth = ::DIBWidth(lpDIB);	// 获取图像宽度
	lHeight = ::DIBHeight(lpDIB);// 获取图像高度	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数
	
	int i,j;
	
	//求边界
	for(i = 0; i < lHeight; i++)
	{
		for(j = 0; j < lWidth; j++)
		{
			if(g_PelsInfo[i*lWidth+j].isWhite)
			{
				bool isEdge=0;
				if(i!=0)
				{
					if(!g_PelsInfo[(i-1)*lWidth+j].isWhite)
					{
						isEdge=1;
					}
				}
				
				if(i!=lHeight-1)
				{
					if(!g_PelsInfo[(i+1)*lWidth+j].isWhite)
					{
						isEdge=1;
					}
				}
				
				if(j!=0)
				{
					if(!g_PelsInfo[i*lWidth+j-1].isWhite)
					{
						isEdge=1;
					}
				}
				
				if(j!=lHeight-1)
				{
					if(!g_PelsInfo[i*lWidth+j+1].isWhite)
					{
						isEdge=1;
					}
					
				}
				
				if(isEdge)
				{
					lpSrc=(unsigned char*)lpDIBBits + lLineBytes * i + j;
					*lpSrc=100;
					g_PelsInfo[i*lWidth+j].isEdge=1;
				}
			}
		}
	} 
	//解除锁定
	::GlobalUnlock((HGLOBAL)hDIB);
}



void GetCenter(HDIB hDIB)
{


	LPSTR	lpDIB;// 指向DIB的指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);// 由DIB句柄得到DIB指针并锁定DIB
	LPSTR   lpDIBBits;// 指向DIB象素数据区的指针
	BYTE *	lpSrc;// 指向DIB象素的指针	
	LONG	lWidth;// 图像宽度
	LONG	lHeight;// 图像高度
	LONG	lLineBytes;	// 图像每行的字节数
	lpDIBBits = ::FindDIBBits(lpDIB);// 找到DIB图像象素起始位置
	lWidth = ::DIBWidth(lpDIB);	// 获取图像宽度
	lHeight = ::DIBHeight(lpDIB);// 获取图像高度	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数	
	
	int i,j;

	//初始化g_RegionInfo
	g_RegionInfo = (RegionInfo*)malloc(region_id*sizeof(RegionInfo));
	for(i=0;i<region_id;i++)
	{
		g_RegionInfo[i].area=0;		
		g_RegionInfo[i].perimeter=0;
		g_RegionInfo[i].count=0;
		g_RegionInfo[i].isCircularity=0;
		g_RegionInfo[i].center_x=0;
		g_RegionInfo[i].center_y=0;
		g_RegionInfo[i].xsum=0;
		g_RegionInfo[i].ysum=0;
        
	}

	//求中心
	for(i = 0; i < lHeight; i++)
	{
		for(j = 0; j < lWidth; j++)
		{	 
			if(g_PelsInfo[i*lWidth+j].isEdge)
			{
			    sum(i,j,lWidth,lHeight);
				g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].center_x=round(g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].xsum/g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].count);
				g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].center_y=round(g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].ysum/g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].count);

			}
		}
	}	
	//解除锁定
	::GlobalUnlock ((HGLOBAL)hDIB);
}

void sum(int i,int j,LONG lWidth,LONG lHeight)
{ 
	g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].xsum=g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].xsum+j;
    g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].ysum=g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].ysum+i;
	g_PelsInfo[i*lWidth+j].isSum=1;
	g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].count++;
	bool b=0;
	if(i!=lHeight-1 && i!=0 && j!=lWidth-1 && j!=0)
	{
		if((g_PelsInfo[(i+1)*lWidth+j].isEdge)&&
			!(g_PelsInfo[(i+1)*lWidth+j].isSum))
		{
			b=1;
			sum(i+1,j,lWidth,lHeight);	
		}
		if((g_PelsInfo[(i-1)*lWidth+j].isEdge)&&
			!(g_PelsInfo[(i-1)*lWidth+j].isSum))
		{
			b=1;
			sum(i-1,j,lWidth,lHeight);	
		}
		if((g_PelsInfo[i*lWidth+j+1].isEdge)&&
			!(g_PelsInfo[i*lWidth+j+1].isSum))
		{
			b=1;
			sum(i,j+1,lWidth,lHeight);	
		}
		if((g_PelsInfo[i*lWidth+j-1].isEdge)&&
			!(g_PelsInfo[i*lWidth+j-1].isSum))
		{
			b=1;
			sum(i,j-1,lWidth,lHeight);	
		}
	}
	if(!b)
		return;
}

void AreaandPerimeter(HDIB hDIB)
{

	LPSTR	lpDIB;// 指向DIB的指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);// 由DIB句柄得到DIB指针并锁定DIB
	LPSTR   lpDIBBits;// 指向DIB象素数据区的指针	
	LONG	lWidth;// 图像宽度
	LONG	lHeight;// 图像高度
	LONG	lLineBytes;	// 图像每行的字节数
	lpDIBBits = ::FindDIBBits(lpDIB);// 找到DIB图像象素起始位置
	lWidth = ::DIBWidth(lpDIB);	// 获取图像宽度
	lHeight = ::DIBHeight(lpDIB);// 获取图像高度	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数	
	
	int i,j;
		
	for(i = 0; i < lHeight; i++)
	{
		for(j = 0; j < lWidth; j++)
		{	 
			if(g_PelsInfo[i*lWidth+j].isWhite)
			{
				//求面积
				g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].area++;
					//求周长
				if(g_PelsInfo[i*lWidth+j].isEdge)
				{
					int adjacentpoint_num=0;
					if(i!=0)
					{
						if(g_PelsInfo[(i-1)*lWidth+j].isEdge)
						{
							adjacentpoint_num++;
						}
					}
					
					if(i!=lHeight-1)
					{
						if(g_PelsInfo[(i+1)*lWidth+j].isEdge)
						{
							adjacentpoint_num++;
						}
					}
					
					if(j!=0)
					{
						if(g_PelsInfo[i*lWidth+j-1].isEdge)
						{
							adjacentpoint_num++;
						}
					}
					
					if(j!=lWidth-1)
					{
						if(g_PelsInfo[i*lWidth+j+1].isEdge)
						{
							adjacentpoint_num++;
						}
						
					}
					
					if(adjacentpoint_num==0)
					{
						g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].perimeter+=sqrt(2);
					}
					
					else if(adjacentpoint_num==1)
					{
						g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].perimeter+=(sqrt(2)+1)/2;
					}
					else if(adjacentpoint_num==2)
					{
						g_RegionInfo[g_PelsInfo[i*lWidth+j].id-1].perimeter+=1;
					}
				}
				
			}
		}
	}
	
	//解除锁定
	::GlobalUnlock ((HGLOBAL)hDIB);
}


void CircularityIdentify(HDIB hDIB)
{

    BYTE *	lpSrc;
	LPSTR	lpDIB;// 指向DIB的指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);// 由DIB句柄得到DIB指针并锁定DIB
	LPSTR   lpDIBBits;// 指向DIB象素数据区的指针
	LONG	lWidth;// 图像宽度
	LONG	lHeight;// 图像高度
	LONG	lLineBytes;	// 图像每行的字节数
	lpDIBBits = ::FindDIBBits(lpDIB);// 找到DIB图像象素起始位置
	lWidth = ::DIBWidth(lpDIB);	// 获取图像宽度
	lHeight = ::DIBHeight(lpDIB);// 获取图像高度	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数	
	
	int i;
	
	//识别圆形标记并求圆平均半径
	int circle_area=0;
	int circle_num=0;
	for(i=0;i<region_id;i++)
	{
		if(g_RegionInfo[i].perimeter*g_RegionInfo[i].perimeter/(pi*g_RegionInfo[i].area*4)<=1.3)
		{
			g_RegionInfo[i].isCircularity=1;
			circle_area+=g_RegionInfo[i].area;
			circle_num++;
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * g_RegionInfo[i].center_y + g_RegionInfo[i].center_x;
			*lpSrc=0;	
		 	lpSrc = (unsigned char*)lpDIBBits + lLineBytes * g_RegionInfo[i].center_y + g_RegionInfo[i].center_x+1;
			*lpSrc=0;
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * g_RegionInfo[i].center_y + g_RegionInfo[i].center_x-1;
			*lpSrc=0;
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (g_RegionInfo[i].center_y+1) + g_RegionInfo[i].center_x;
			*lpSrc=0;
			lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (g_RegionInfo[i].center_y-1) + g_RegionInfo[i].center_x;
			*lpSrc=0;
		}
	}
	average_radius=sqrt(circle_area/circle_num/pi);
	
	//解除锁定
	::GlobalUnlock ((HGLOBAL)hDIB);
}

void DelErr(HDIB hDIB)
{
 
    step=4;
	LPSTR	lpDIB;// 指向DIB的指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);// 由DIB句柄得到DIB指针并锁定DIB
	LPSTR   lpDIBBits;// 指向DIB象素数据区的指针
	LONG	lWidth;// 图像宽度
	BYTE *	lpSrc;
	LONG	lHeight;// 图像高度
	LONG	lLineBytes;	// 图像每行的字节数
	lpDIBBits = ::FindDIBBits(lpDIB);// 找到DIB图像象素起始位置
	lWidth = ::DIBWidth(lpDIB);	// 获取图像宽度
	lHeight = ::DIBHeight(lpDIB);// 获取图像高度	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数	
	
	int i,j;
	
	for(i=0;i<region_id;i++)
	{	
		if(g_RegionInfo[i].isCircularity==1)
		{
			for(j=0;j<region_id;j++)
			{
				if(g_RegionInfo[j].isCircularity==1&&i!=j)
				{

					double distance=sqrt(
						(g_RegionInfo[i].center_x-g_RegionInfo[j].center_x)*(g_RegionInfo[i].center_x-g_RegionInfo[j].center_x)+
						(g_RegionInfo[i].center_y-g_RegionInfo[j].center_y)*(g_RegionInfo[i].center_y-g_RegionInfo[j].center_y)
						);
					
					if(distance<5*average_radius)
					{
						if(g_RegionInfo[i].area<g_RegionInfo[j].area)
						{
							g_RegionInfo[i].isCircularity=0;
                            lpSrc = (unsigned char*)lpDIBBits + lLineBytes * g_RegionInfo[i].center_y + g_RegionInfo[i].center_x;
						    *lpSrc=255;
							lpSrc = (unsigned char*)lpDIBBits + lLineBytes * g_RegionInfo[i].center_y + g_RegionInfo[i].center_x+1;
			                *lpSrc=255;
			                lpSrc = (unsigned char*)lpDIBBits + lLineBytes * g_RegionInfo[i].center_y + g_RegionInfo[i].center_x-1;
			                *lpSrc=255;
			                lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (g_RegionInfo[i].center_y+1) + g_RegionInfo[i].center_x;
			                *lpSrc=255;
			                lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (g_RegionInfo[i].center_y-1) + g_RegionInfo[i].center_x;
			                *lpSrc=255;

						}
						else
						{
							g_RegionInfo[j].isCircularity=0;
                            lpSrc = (unsigned char*)lpDIBBits + lLineBytes * g_RegionInfo[j].center_y+ g_RegionInfo[j].center_x;
						    *lpSrc=255;
							lpSrc = (unsigned char*)lpDIBBits + lLineBytes * g_RegionInfo[j].center_y + g_RegionInfo[j].center_x+1;
			                *lpSrc=255;
			                lpSrc = (unsigned char*)lpDIBBits + lLineBytes * g_RegionInfo[j].center_y + g_RegionInfo[j].center_x-1;
			                *lpSrc=255;
			                lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (g_RegionInfo[j].center_y+1) + g_RegionInfo[j].center_x;
			                *lpSrc=255;
			                lpSrc = (unsigned char*)lpDIBBits + lLineBytes * (g_RegionInfo[j].center_y-1) + g_RegionInfo[j].center_x;
			                *lpSrc=255;
						}
					}
				}
			}
		}
	}

	//解除锁定
	::GlobalUnlock ((HGLOBAL)hDIB);
}


void ImageProcess(HDIB hDIB)
{
	step=2;
	Set256toGray(hDIB);         //将256色位图转为灰度图
	if(is256)
	{
	GrayToWhiteBlack(hDIB);  //将灰度图二值化
	RemoveNoise(hDIB);
	RegionSign(hDIB);
	EdgeIdentify(hDIB);
	}
}

void CenterIdentify(HDIB hDIB)
{
	step=3;
	GetCenter(hDIB);
	AreaandPerimeter(hDIB);	
	CircularityIdentify(hDIB);	    //识别圆形标记
}

void Codejiema(HDIB hDIB) 
{
	// TODO: Add your command handler code here

	step=5;

	LPSTR	lpDIB;// 指向DIB的指针
	lpDIB = (LPSTR) ::GlobalLock((HGLOBAL)hDIB);// 由DIB句柄得到DIB指针并锁定DIB
	LPSTR   lpDIBBits;// 指向DIB象素数据区的指针
	BYTE *	lpSrc;// 指向DIB象素的指针	
	LONG	lWidth;// 图像宽度
	LONG	lHeight;// 图像高度
	LONG	lLineBytes;	// 图像每行的字节数
	lpDIBBits = ::FindDIBBits(lpDIB);// 找到DIB图像象素起始位置
	lWidth = ::DIBWidth(lpDIB);	// 获取图像宽度
	lHeight = ::DIBHeight(lpDIB);// 获取图像高度	
	lLineBytes = WIDTHBYTES(lWidth * 8);// 计算图像每行的字节数		
	
	// 更换每个象素的颜色索引（即按照灰度映射表换成灰度值）
	int k;
	int i,j;
	
	for(k=0;k<region_id;k++)
	{	
		if(g_RegionInfo[k].isCircularity==1)
		{
			int  radius;
			int  required_angle;
			int  angle;
			int angle_n=0;
			int cross_n=0;
			bool isfind=0;
			
			//寻找大概起始角度
			for(angle=0;angle<360/N;angle++)
			{
				cross_n=0;				
				for(radius=round(1.5*sqrt(g_RegionInfo[k].area/pi));radius<4*average_radius;radius++)
				{
					j=round(radius*cos(N*angle*pi/180.0))+g_RegionInfo[k].center_x;           //隔24度扫一次，水平朝右为0度
					i=round(radius*sin(N*angle*pi/180.0))+g_RegionInfo[k].center_y;
					if(i>0&&i<lHeight&&j>=0&&j<lWidth)
					{
					  
							if((g_PelsInfo[(i+1)*lWidth+j].isEdge&&angle<180)||
								(g_PelsInfo[(i-1)*lWidth+j].isEdge&&angle>180)||
								g_PelsInfo[i*lWidth+j].isEdge)
							{
								cross_n++;
								if(cross_n>0)
								{
									required_angle=N*angle;
									isfind=1;
									break;
								}
							}
					}
					else
					{
						break;
					}
				}	
				if(isfind)
				{
					break;
				}
			}
			int bin_code[M]=                //找到扇环开始记为1（白）
			{
				1,0,0,0,0,  
					0,0,0,0,0,
					0,0,0,0,0 
			};
			if(angle==360/N)        //没有找到扇环
	    	bin_code[0]=0;
			
			//寻找精确起始角度	
			for(angle=1;angle<360;angle++)
			{
				cross_n=0;
				isfind=0;
				for(radius=round(1.5*sqrt(g_RegionInfo[k].area/pi));radius<4*average_radius;radius++)
				{
					j=round(radius*cos((required_angle+angle)*pi/180.0))+g_RegionInfo[k].center_x;
					i=round(radius*sin((required_angle+angle)*pi/180.0))+g_RegionInfo[k].center_y;
					if(i>0&&i<lHeight-1&&j>0&&j<lWidth-1)
					{
						if((g_PelsInfo[(i+1)*lWidth+j].isEdge&&angle<180)||
							(g_PelsInfo[(i-1)*lWidth+j].isEdge&&angle>180)||
							g_PelsInfo[i*lWidth+j].isEdge)
						{
							cross_n++;
							if(cross_n>0)
							{
								isfind=1;
								break;
							}
						}
					}
					else
					{
						break;
					}
				}	
				if(!isfind)       //找到扇环的边界
				{
					required_angle=required_angle+angle-1;
					break;
				}
			}
			
			
			//求二进制编码
			
			for(angle=1;angle<M;angle++)
			{
				isfind=0;
				cross_n=0;
				for(radius=round(1.5*sqrt(g_RegionInfo[k].area/pi));radius<4*average_radius;radius++)
				{
					j=round(radius*cos((required_angle-angle*(360.0/M)-(180.0/M))*pi/180))+g_RegionInfo[k].center_x;
					i=round(radius*sin((required_angle-angle*(360.0/M)-(180.0/M))*pi/180))+g_RegionInfo[k].center_y;
					if(i>0&&i<lHeight-1&&j>0&&j<lWidth-1)
					{
					    lpSrc = (unsigned char*)lpDIBBits + lLineBytes * i + j;
				        *lpSrc=255;
						if((g_PelsInfo[(i+1)*lWidth+j].isEdge&&angle<180)||
							(g_PelsInfo[(i-1)*lWidth+j].isEdge&&angle>180)||
							g_PelsInfo[i*lWidth+j].isEdge)
						{
							cross_n++;
							if(cross_n>0)
							{
								isfind=1;
								break;
							}
						}
					}
					else
					{
						break;
					}
				}	
				if(isfind)
				{
					bin_code[angle]=1;
				}
			}
			
			//求出最小十进制编码
			int dec_code[M]=
			{
				0,0,0,0,0,  
					0,0,0,0,0,
					0,0,0,0,0 
			};
			bool b=1;
			for(i=0;i<M;i++)
			{
				for(j=0;j<M;j++)
				{
					dec_code[i]+=bin_code[(i+j)%M]*round(pow(2,M-j-1));
				}
			}
			int min=32767;
			int n;
			for(i=0;i<M;i++)
			{
				if(dec_code[i]<min)
				{
					min=dec_code[i];
					n=i;
				}
			}
			
			//把编码存放到g_RegionInfo中
			CString str;
			CString coding;
			for(i=0;i<M;i++)
			{
				str.Format("%d",bin_code[(i+n)%M]);
				coding+=str;
			}
			g_RegionInfo[k].bin_code=new CString(coding);
			g_RegionInfo[k].dec_code=min;
		}
	}
	
	//将结果写入文件
	CString str;
	CStdioFile   file;  
	int n=0; 	
	for(k=0;k<region_id;k++)
	{
		if(g_RegionInfo[k].isCircularity==1)
		{
			n++;
		}
	}
	str.Format("%s.txt",strFileTitle);
	file.Open(str,CFile::modeWrite|CFile::modeCreate); 
	str.Format( "共有%d个编码点:\n",n); 
	file.WriteString(str);
	str.Format( "编号:		 坐标:			十进制编码:\n"); 
	file.WriteString(str);
	n=0;
	j=1000;
	for(k=0;k<region_id;k++)
	{
		if(g_RegionInfo[k].isCircularity==1)
		{
			
		
			if(g_RegionInfo[k].dec_code==0)
			{	
				j++;
			    str.Format("%d",j); 	
			    file.WriteString(str);
		    	file.WriteString("		");				
			}
			else
			{
				n++;
			    str.Format("%d",n); 	
			    file.WriteString(str);
			    file.WriteString("		");
			}

            str.Format("%d",g_RegionInfo[k].center_x); 
			file.WriteString(str);
			file.WriteString("	");
			str.Format("%d",g_RegionInfo[k].center_y); 
			file.WriteString(str);
            file.WriteString("		");

		/*	str=*g_RegionInfo[k].bin_code; 
			file.WriteString(str);
            file.WriteString("		");*/

            
			str.Format("%d",g_RegionInfo[k].dec_code); 	
			file.WriteString(str);
			
			file.WriteString("\n");
		}
	}
	file.Close();
	//解除锁定
	::GlobalUnlock ((HGLOBAL)hDIB);
}


void Sign(int i,int j,LONG lWidth,LONG lHeight)
{ 
	g_PelsInfo[i*lWidth+j].id=region_id;
	g_PelsInfo[i*lWidth+j].isVisited=1;
	
	bool b=0;
	if(i!=lHeight-1 && i!=0 && j!=lWidth-1 && j!=0)
	{
		if((g_PelsInfo[(i+1)*lWidth+j].isWhite)&&
			!(g_PelsInfo[(i+1)*lWidth+j].isVisited))
		{
			b=1;
			Sign(i+1,j,lWidth,lHeight);	
		}
		if((g_PelsInfo[(i-1)*lWidth+j].isWhite)&&
			!(g_PelsInfo[(i-1)*lWidth+j].isVisited))
		{
			b=1;
			Sign(i-1,j,lWidth,lHeight);	
		}
		if((g_PelsInfo[i*lWidth+j+1].isWhite)&&
			!(g_PelsInfo[i*lWidth+j+1].isVisited))
		{
			b=1;
			Sign(i,j+1,lWidth,lHeight);	
		}
		if((g_PelsInfo[i*lWidth+j-1].isWhite)&&
			!(g_PelsInfo[i*lWidth+j-1].isVisited))
		{
			b=1;
			Sign(i,j-1,lWidth,lHeight);	
		}
	}
	if(!b)
		return;
}

int round(double f)
{
	if(f-int(f)>=0.5)
		return int(f)+1;
	else
		return int(f);
}

int Match(int c1,int c2,int aa[100][6])
{
	
    char*  strc;
	bool prime=false;
	vector <string> sub_str;
	int a1[80][4],a2[80][4],i=0,j=0,k,m,p=0;
	for(k=0;k<80;k++)
		for(m=0;m<5;m++)
			a2[k][m]=0;
    CStdioFile f1,f2;
	CString str;
	string ss;
	str.Format("%d.txt",c1);
	f1.Open(str,CFile::modeRead);
    f1.ReadString(str);
	f1.ReadString(str);
	while(f1.ReadString(str))
	{
	          ss=(LPCTSTR)str;
              //取出这一行的各部分数据
              sub_str=Getsubstr(ss);	
			  strc=const_cast <char   *>(sub_str[0].c_str());
			  a1[i][0]=atoi(strc);
              strc=const_cast <char   *>(sub_str[1].c_str());
			  a1[i][1]=atoi(strc);
			  strc=const_cast <char   *>(sub_str[2].c_str());
			  a1[i][2]=atoi(strc);
              strc=const_cast <char   *>(sub_str[3].c_str());
			  a1[i][3]=atoi(strc);
              i++;
	}
    f1.Close();
	str.Format("%d.txt",c2);
	f2.Open(str,CFile::modeRead);
    f2.ReadString(str);
	f2.ReadString(str);
	while(f2.ReadString(str))
	{
	          ss=(LPCTSTR)str;
              //取出这一行的各部分数据
              sub_str=Getsubstr(ss);	
			  strc=const_cast <char   *>(sub_str[0].c_str());
			  a2[j][0]=atoi(strc);
              strc=const_cast <char   *>(sub_str[1].c_str());
			  a2[j][1]=atoi(strc);
			  strc=const_cast <char   *>(sub_str[2].c_str());
			  a2[j][2]=atoi(strc);
              strc=const_cast <char   *>(sub_str[3].c_str());
			  a2[j][3]=atoi(strc);
              j++;
	}
    f2.Close();
   
     for(k=0;k<i;k++)
		for(m=0;m<j;m++)
		{
		   if(a1[k][3]==a2[m][3]&&a1[k][3]!=0)  
		   {
            aa[p][0]=a1[k][1];
			aa[p][1]=a1[k][2];
		    aa[p][2]=a2[m][1];
			aa[p][3]=a2[m][2];
			p++;
			break;
		   }
		}
		i=p;
		 for(k=0;k<j;k++)  
		 {	
			 for(m=0;m<p;m++)
			 {
				 if(aa[m][2]==a2[k][1]&&aa[m][3]==a2[k][2]) 
				 {
					 prime=true;
					 break;
				 }
				 			 
			 } 
            if(prime==false&&a2[k][3]!=0)
			{
				aa[i][2]=a2[k][1]; 
			    aa[i][3]=a2[k][2]; 
			    i++;              
			}
			prime=false;
		 }
	return i;
}


void Codematch()
{  
	step=6;
    CStdioFile file;
	CString str;
	bool prime=false;
	int c1,c2,a[100][6],b[100][6],n,i,j,k,m;
     //
    for(i=0;i<100;i++)
		for(j=0;j<6;j++)
			a[i][j]=0;
	for(i=0;i<100;i++)
		for(j=0;j<6;j++)
			b[i][j]=0;		
    c1=1,c2=2;
    i=Match(c1,c2,a);
	c1=2,c2=3;
    j=Match(c1,c2,b);
	n=i;
    for(k=0;k<j;k++)
	{	for(m=0;m<i;m++)
		{
		     if(b[k][0]==a[m][2]&&b[k][1]==a[m][3])
			 {   
				 prime=true;
				 break;
			 }			 
		
		}
	    if(prime)
		{
		 a[m][4]=b[k][2];
		 a[m][5]=b[k][3];
		}
		else
		{
		 a[n][4]=b[k][2];
		 a[n][5]=b[k][3];
		 n++;
		}
		prime=false;
    }

	file.Open("pipei.txt",CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate);
	file.SeekToEnd();
	str.Format("num		x1		y1		x2		y2		x3		y3\n");
    file.WriteString(str);
	j=0;
	for(i=0;i<n;i++)
	{    j++;
	     str.Format("%d",j);
		 file.WriteString(str);
		 file.WriteString("		");

         str.Format("%d",a[i][0]);
		 file.WriteString(str);
		 file.WriteString("		");

		 str.Format("%d",a[i][1]);
		 file.WriteString(str);
		 file.WriteString("		");

		 str.Format("%d",a[i][2]);
		 file.WriteString(str);
		 file.WriteString("		");

		 str.Format("%d",a[i][3]);
		 file.WriteString(str);
		 file.WriteString("		");

         str.Format("%d",a[i][4]);
		 file.WriteString(str);
		 file.WriteString("		");

		 str.Format("%d",a[i][5]);
		 file.WriteString(str);
		 file.WriteString("\n");
	}
   file.Close();  
	//	
}

void GetCrossRatio()
{   
	step=7;
	float k1,k2,k3,k4,I;
	int i,j,x[5][4],xx,yy,deccode;
	char*  strc;
	vector <string> sub_str;
    CStdioFile file,f1,f2,f3;  
	CString str[100],VI,str1;	
	string ss,W;
	file.Open("pipei.txt",CFile::modeRead);  
	file.ReadString(str1);
    for(i=0;i<5;i++)
    {
	          file.ReadString(str1);
	          ss=(LPCTSTR)str1;
			  sub_str=Getsubstr(ss);  		              
              strc=const_cast <char   *>(sub_str[1].c_str());
			  x[i][0]=atoi(strc);
			  strc=const_cast <char   *>(sub_str[2].c_str());
			  x[i][1]=atoi(strc);
              strc=const_cast <char   *>(sub_str[3].c_str());
			  x[i][2]=atoi(strc);
			  strc=const_cast <char   *>(sub_str[4].c_str());
			  x[i][3]=atoi(strc);
             

	}
	file.Close();

    f1.Open("1.txt",CFile::modeRead); 
    f1.ReadString(str[0]);
	f1.ReadString(str[1]);
    j=0;
    while(f1.ReadString(str[j]))
	{
	       
              ss=(LPCTSTR)str[j];
              //分开几部分，保留x,y坐标和十进制数的值
              sub_str=Getsubstr(ss);	       
              strc=const_cast <char   *>(sub_str[1].c_str());
			  xx=atoi(strc);
			  strc=const_cast <char   *>(sub_str[2].c_str());
			  yy=atoi(strc);
              strc=const_cast <char   *>(sub_str[3].c_str());
			  deccode=atoi(strc);

	         if(deccode==0)
			 {
		          k1=float(yy-x[0][1])/float(xx-x[0][0]);
                  k2=float(yy-x[2][1])/float(xx-x[2][0]);
                  k3=float(yy-x[1][1])/float(xx-x[1][0]);
                  k4=float(yy-x[3][1])/float(xx-x[3][0]);				 
                  I=(k1-k3)*(k2-k4)/(k2-k3)*(k1-k4); 
				  //求小数点后2位

		          VI.Format("%.2f",I); 
                  W=(LPCTSTR)VI;
                  ss=sub_str[0]+"		"+sub_str[1]+"	"+sub_str[2]+"		"+W;
		          str[j]=ss.c_str();
                  j++;
			 }
                  

	}
    f1.Close();
	//在1.txt中重新写入
    f1.Open("6.txt",CFile::modeCreate|CFile::modeWrite);
    for(i=0;i<j;i++)
	{   
		f1.WriteString(str[i]);
	    f1.WriteString("\n");
	}
    f1.Close();
	   
    f2.Open("2.txt",CFile::modeRead); 
    f2.ReadString(str[0]);
	f2.ReadString(str[1]);

    j=0;
	while(f2.ReadString(str[j]))
	{
	          
              ss=(LPCTSTR)str[j];
              //取出这一行的各部分数据
              sub_str=Getsubstr(ss);
	       
              strc=const_cast <char   *>(sub_str[1].c_str());
			  xx=atoi(strc);
			  strc=const_cast <char   *>(sub_str[2].c_str());
			  yy=atoi(strc);
              strc=const_cast <char   *>(sub_str[3].c_str());
			  deccode=atoi(strc);

	         if(deccode==0)
			 {
		          k1=float(yy-x[0][3])/float(xx-x[0][2]);
                  k2=float(yy-x[2][3])/float(xx-x[2][2]);
                  k3=float(yy-x[1][3])/float(xx-x[1][2]);
                  k4=float(yy-x[3][3])/float(xx-x[3][2]);               
                  I=(k1-k3)*(k2-k4)/(k2-k3)*(k1-k4); 	       
		          VI.Format("%.2f",I);
		          W=(LPCTSTR)VI;                 
                  ss=sub_str[0]+"		"+sub_str[1]+"	"+sub_str[2]+"		"+W;
		          str[j]=ss.c_str();
                j++;
			 }
             
	}
	   f2.Close();

	   f2.Open("7.txt",CFile::modeCreate|CFile::modeWrite);
         for(i=0;i<j;i++)
		 {   
	     	f2.WriteString(str[i]);
	        f2.WriteString("\n");
		 }
       f2.Close();

     file.Open("pipei.txt",CFile::modeRead);  
	 i=0;
     file.ReadString(str1);
     while(i<5)
	 {
	          file.ReadString(str1);
	          ss=(LPCTSTR)str1;
              //分开几部分，保留x,y坐标
			  sub_str=Getsubstr(ss);  	              
              strc=const_cast <char   *>(sub_str[3].c_str());
			  x[i][0]=atoi(strc);
			  strc=const_cast <char   *>(sub_str[4].c_str());
			  x[i][1]=atoi(strc);
              strc=const_cast <char   *>(sub_str[5].c_str());
			  int temp=atoi(strc);
			  if(temp!=0)
			  {
				  x[i][2]=temp;
			      strc=const_cast <char   *>(sub_str[6].c_str());
			      x[i][3]=atoi(strc);
				  i++;
			  }
    
	 }
    file.Close();

    f2.Open("2.txt",CFile::modeRead); 
    f2.ReadString(str[0]);
	f2.ReadString(str[1]);

    j=0;
	while(f2.ReadString(str[j]))
	{
	          
              ss=(LPCTSTR)str[j];
              //取出这一行的各部分数据
              sub_str=Getsubstr(ss);
	       
              strc=const_cast <char   *>(sub_str[1].c_str());
			  xx=atoi(strc);
			  strc=const_cast <char   *>(sub_str[2].c_str());
			  yy=atoi(strc);
              strc=const_cast <char   *>(sub_str[3].c_str());
			  deccode=atoi(strc);

	         if(deccode==0)
			 {
		          k1=float(yy-x[0][1])/float(xx-x[0][0]);
                  k2=float(yy-x[2][1])/float(xx-x[2][0]);
                  k3=float(yy-x[1][1])/float(xx-x[1][0]);
                  k4=float(yy-x[3][1])/float(xx-x[3][0]);
                  I=(k1-k3)*(k2-k4)/(k2-k3)*(k1-k4); 	       
		          VI.Format("%.2f",I);
		          W=(LPCTSTR)VI;                 
                  ss=sub_str[0]+"		"+sub_str[1]+"	"+sub_str[2]+"		"+W;
		          str[j]=ss.c_str();
                j++;
			 }
             
	}
	   f2.Close();

	   f2.Open("8.txt",CFile::modeCreate|CFile::modeWrite);
         for(i=0;i<j;i++)
		 {   
	     	f2.WriteString(str[i]);
	        f2.WriteString("\n");
		 }
       f2.Close();

    f3.Open("3.txt",CFile::modeRead); 
    f3.ReadString(str[0]);
	f3.ReadString(str[1]);
	j=0;
	while(f3.ReadString(str[j]))
	{
	     
              ss=(LPCTSTR)str[j];
              //取出这一行的各部分数据
			   sub_str=Getsubstr(ss);	        	                 
              strc=const_cast <char   *>(sub_str[1].c_str());
			  xx=atoi(strc);
			  strc=const_cast <char   *>(sub_str[2].c_str());
			  yy=atoi(strc);
              strc=const_cast <char   *>(sub_str[3].c_str());
			  deccode=atoi(strc);
       
	          if(deccode==0)
			  {
		           k1=float(yy-x[0][3])/float(xx-x[0][2]);
                   k2=float(yy-x[1][3])/float(xx-x[1][2]);
                   k3=float(yy-x[2][3])/float(xx-x[2][2]);
                   k4=float(yy-x[3][3])/float(xx-x[3][2]);                 
                   I=(k1-k3)*(k2-k4)/(k2-k3)*(k1-k4); 		   
		           VI.Format("%.2f",I);
		           W=(LPCTSTR)VI;
                   ss=sub_str[0]+"		"+sub_str[1]+"	"+sub_str[2]+"		"+W;
		           str[j]=ss.c_str();
				    j++;
			  }
             

	}
	f3.Close();
	f3.Open("9.txt",CFile::modeCreate|CFile::modeWrite);
    for(i=0;i<j;i++)
	{   
		f3.WriteString(str[i]);
	    f3.WriteString("\n");
	}

    f3.Close();
   
}


int zhongjian(int c1,int c2,int aa[100][6])
{
	
    char*  strc;
	bool prime=false;
	vector <string> sub_str;
	int a1[50][3],a2[50][4],i=0,j=0,k,m,p=0;
	double a11[50],a22[50];
	for(k=0;k<50;k++)
		for(m=0;m<4;m++)
			a2[k][m]=0;
    CStdioFile f1,f2;
	CString str;
	string ss;
	str.Format("%d.txt",c1);
	f1.Open(str,CFile::modeRead);
	while(f1.ReadString(str))
	{
	          ss=(LPCTSTR)str;
              //取出这一行的各部分数据
              sub_str=Getsubstr(ss);	
			  strc=const_cast <char   *>(sub_str[0].c_str());
			  a1[i][0]=atoi(strc);
              strc=const_cast <char   *>(sub_str[1].c_str());
			  a1[i][1]=atoi(strc);
			  strc=const_cast <char   *>(sub_str[2].c_str());
			  a1[i][2]=atoi(strc);
              strc=const_cast <char   *>(sub_str[3].c_str());
			  a11[i]=atof(strc);
              i++;
	}
    f1.Close();
	str.Format("%d.txt",c2);
	f2.Open(str,CFile::modeRead);
	while(f2.ReadString(str))
	{
	          ss=(LPCTSTR)str;
              //取出这一行的各部分数据
              sub_str=Getsubstr(ss);	
			  strc=const_cast <char   *>(sub_str[0].c_str());
			  a2[j][0]=atoi(strc);
              strc=const_cast <char   *>(sub_str[1].c_str());
			  a2[j][1]=atoi(strc);
			  strc=const_cast <char   *>(sub_str[2].c_str());
			  a2[j][2]=atoi(strc);
              strc=const_cast <char   *>(sub_str[3].c_str());
			  a22[j]=atof(strc);
              j++;
	}
    f2.Close();
   
     for(k=0;k<i;k++)
		for(m=0;m<j;m++)
		{
		   if(fabs(a11[k]-a22[m])<0.2&&a2[m][3]==0)
		   {
            aa[p][0]=a1[k][1];
			aa[p][1]=a1[k][2];
		    aa[p][2]=a2[m][1];
			aa[p][3]=a2[m][2];
			a2[m][3]=1;
			p++;
			break;
		   }
		}
		i=p;
		 for(k=0;k<j;k++)  
		 {	
			 for(m=0;m<p;m++)
			 {
				 if(aa[m][2]==a2[k][1]&&aa[m][3]==a2[k][2]) 
				 {
					 prime=true;
					 break;
				 }
				 			 
			 } 
            if(prime==false)
			{
				aa[i][2]=a2[k][1]; 
			    aa[i][3]=a2[k][2]; 
			    i++;              
			}
			prime=false;
		 }
	return i;
}



void Incodematch()
{
	step=8;
	CStdioFile file;
	CString str;
	bool prime=false;
	int c1,c2,a[100][6],b[100][6],n,i,j,k,m;
 
	for(i=0;i<100;i++)
		for(j=0;j<6;j++)
			a[i][j]=0;
	for(i=0;i<100;i++)
		for(j=0;j<6;j++)
			b[i][j]=0;		
    c1=6,c2=7;
    i=zhongjian(c1,c2,a);
	 c1=8,c2=9;
    j=zhongjian(c1,c2,b);
	n=i;
    for(k=0;k<j;k++)
	{	for(m=0;m<i;m++)
		{
		     if(b[k][0]==a[m][2]&&b[k][1]==a[m][3])
			 {   
				 prime=true;
				 break;
			 }			 
		
		}
	    if(prime)
		{
		 a[m][4]=b[k][2];
		 a[m][5]=b[k][3];
		}
		else
		{
		 a[n][4]=b[k][2];
		 a[n][5]=b[k][3];
		 n++;
		}
		prime=false;
    }

	file.Open("pipei.txt",CFile::modeCreate|CFile::modeReadWrite|CFile::modeNoTruncate);
	file.SeekToEnd();

	j=1000;
	for(i=0;i<n;i++)
	{    j++;
	     str.Format("%d",j);
		 file.WriteString(str);
		 file.WriteString("		");

         str.Format("%d",a[i][0]);
		 file.WriteString(str);
		 file.WriteString("		");

		 str.Format("%d",a[i][1]);
		 file.WriteString(str);
		 file.WriteString("		");

		 str.Format("%d",a[i][2]);
		 file.WriteString(str);
		 file.WriteString("		");

		 str.Format("%d",a[i][3]);
		 file.WriteString(str);
		 file.WriteString("		");

         str.Format("%d",a[i][4]);
		 file.WriteString(str);
		 file.WriteString("		");

		 str.Format("%d",a[i][5]);
		 file.WriteString(str);
		 file.WriteString("		");

		 file.WriteString("\n");
	}
   file.Close();  
    

}

vector<string> Getsubstr(string str)
{
          
		 vector <string> sub_str;
	         int length=str.length();
	         int start=0;
	         int size=0;
			 int i=0;
	        while(i<length)
			{

				
		        while(str.at(start+size)!='	')
				{
			        if(start+size==length-1) break;
			        size++;
				}
				if(start+size!=length-1)
		        sub_str.push_back(str.substr(start,size));
				else
				{
					sub_str.push_back(str.substr(start,size+1));
					break;
				}
		     	
		
		        i=start+size;
	        	i++;
	        	while(str.at(i)=='	') i++;
		
		        start=i;
		        size=0;

			}
			return sub_str;

}