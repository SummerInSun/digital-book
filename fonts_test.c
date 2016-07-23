#include <dis_manag.h>
#include <common_st.h>
#include <font_manag.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <string.h>

struct DisOpr *g_ptFBTest;
struct FontOpr *g_ptFTFontOpr;
struct FontBitMap g_ptFTFontBitMap;

void put_ascii_byte(unsigned int x, unsigned int y, unsigned char byte)
{
	int i = 0, j = 0;
	unsigned char *dots;

	dots = (unsigned char *)g_ptFTFontBitMap.pucBuffer;
	
	for(i = 0; i < 16; i ++){
		for(j = 7; j >= 0; j--){
			if((1 << j) & dots[i]){
				g_ptFBTest->ShowPiexl(x + 7 - j, y + i, 0xffffff);
			}else{
				g_ptFBTest->ShowPiexl(x + 7 - j, y + i, 0);
			}
		}
	}
}


void put_chinese_word(unsigned int x, unsigned int y, unsigned char *word)
{
	unsigned char *dots;
	unsigned char byte;
	int i, j, k;

	dots = (unsigned char *)g_ptFTFontBitMap.pucBuffer;

	for(i = 0; i < 16; i ++){
		for(j = 0; j < 2; j ++){
			byte = dots[j + i*2];
			for(k = 7; k >= 0; k --){
				(!!((1 << k) & byte)) ? g_ptFBTest->ShowPiexl(x + j*8 + 7 - k, y + i, 0xffffff)
				: g_ptFBTest->ShowPiexl(x + j*8 + 7 - k, y + i, 0);
			}
		}
	}
	
}



/* Replace this function with something useful. */

void
draw_bitmap( struct FontBitMap *bitmap,
             FT_Int      x,
             FT_Int      y)
{
  FT_Int  i, j, p, q, k;
  FT_Int  x_max = bitmap->iXMax;
  FT_Int  y_max = bitmap->iYMax;
  char bit;
  unsigned char ucByte;

#if 0
  for ( i = x, p = 0; i < x_max; i++, p++ )
  {
    for ( j = y, q = 0; j < y_max; j++, q++ )
    {
      if ( i < 0      || j < 0       ||
           i >= 480 || j >= 272 )
        continue;

      //image[j][i] |= bitmap->buffer[q * bitmap->width + p];
      g_ptFBTest->ShowPiexl(i, j, bitmap->pucBuffer[q * (bitmap->iXMax - bitmap->iXLeft) + p]);
    }
  }
#else
#if 0
	j = 0; 

	for(p = y; p < y_max; p ++){
		j = (p - y)*bitmap->iPatch;
		printf("%d\n", j);
		for(q = x, i = 7; q < x_max; q ++){
			if(bitmap->pucBuffer[j] & (1 << i)){
				g_ptFBTest->ShowPiexl(q, p, 0xffffff);
			}else{
				g_ptFBTest->ShowPiexl(q, p, 0x0);
			}
			i --;
			if(i == -1){
				i = 7;
				j ++;
			}
		}
		printf("%d\n\n", j);
	}
#else

	j = 0; 

	for(p = y; p < y_max; p ++){
//		j = (p - y)*bitmap->iPatch;
		printf("%d\n", j);
		for(q = x, i = 7; q < bitmap->iNextOriginX; q ++){
			if(bitmap->pucBuffer[j] & (1 << i)){
				g_ptFBTest->ShowPiexl(q, p, 0xffffff);
			}else{
				g_ptFBTest->ShowPiexl(q, p, 0x0);
			}
			i --;
			if(i == -1){
				i = 7;
				j ++;
			}
		}
		j ++;
		printf("%d\n\n", j);
	}
	
#endif

/*
	for (y = bitmap->iYTop; y < bitmap->iYMax; y++)
	{
		i = (y - bitmap->iYTop) * bitmap->iPatch;
		for (x = bitmap->iXLeft,bit = 7;; x < bitmap->iXMax; x++)
		{
			if (bitmap->pucBuffer[i] & (1<<bit))
			{
				g_ptFBTest->ShowPiexl(x, y, 0xffffffff);
			}
			else
			{
				/* 使用背景色, 不用描画 */
				// g_ptDispOpr->ShowPixel(x, y, 0); /* 黑 */
/*			}
			bit--;
			if (bit == -1)
			{
				bit = 7;
				i ++;
			}
		}
	}
*/
#endif
}

int main(int argc, char *argv[])  
{  
	int i;
	int iError = 0;

	FBInit();
	FontInit();

	printf("\nSupported fonts\n");
	ShowFontOpr();

	printf("\nSupported devices\n");
	ShowDisOpr();

	g_ptFBTest = GetDisOpr("fb");

	g_ptFBTest->DisDeviceInit();
	g_ptFBTest->CleanScreen(0x123456);

	for(i = 0; i < 480; i ++)
		g_ptFBTest->ShowPiexl(i, 20, 0xffffff);

	/* display a freetype word */
	g_ptFTFontBitMap.iCurOriginX = 240;
	g_ptFTFontBitMap.iCurOriginY = 136;
	
	g_ptFTFontOpr = GetFontOpr("freetype");
	if(g_ptFTFontOpr == NULL){
		printf("GetFontOpr error\n");
	}
	g_ptFTFontOpr->FontInit("./simfang.ttf", atoi(argv[1]));
	g_ptFTFontOpr->GetFontBitMap(0x7FF0, &g_ptFTFontBitMap);
/*
	for(i = 0; i < 240; i ++){
		for(iError = 0; iError < 8; iError ++){
			if(g_ptFTFontBitMap.pucBuffer[iError+i*8]){
				printf("1 ");
			}else{
				printf("0 ");
			}
		}
			
		printf("\n");
	}
*/
	/* now, draw to our target surface (convert position) */
    draw_bitmap(&g_ptFTFontBitMap, g_ptFTFontBitMap.iXLeft, g_ptFTFontBitMap.iYTop);

	/* display a ascii word */
	g_ptFTFontOpr = GetFontOpr("ascii");
	if(g_ptFTFontOpr == NULL){
		printf("GetFontOpr error\n");
	}
	g_ptFTFontOpr->FontInit("./simfang.ttf", 16);
	g_ptFTFontOpr->GetFontBitMap(0x41, &g_ptFTFontBitMap);
	put_ascii_byte(0, 50, 'A');

	/* display a gbk word */
	g_ptFTFontOpr = GetFontOpr("gbk");
	if(g_ptFTFontOpr == NULL){
		printf("GetFontOpr error\n");
	}
	g_ptFTFontOpr->FontInit("./HZK16", 16);
	g_ptFTFontOpr->GetFontBitMap(0xd0d6, &g_ptFTFontBitMap);
	put_chinese_word(0, 60, "b");
	
    return 0;  
}  


