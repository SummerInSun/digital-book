#include <dis_manag.h>
#include <common_st.h>
#include <stdio.h>
#include <string.h>

int main(int argv, char *argc[])  
{  
	struct DisOpr *ptFBTest;
	int i, j;

	FBInit();	

	ShowDisOpr();

	ptFBTest = GetDisOpr("fb");

	ptFBTest->DisDeviceInit();
	ptFBTest->CleanScreen(0x123456);

	for(i = 0; i < 480; i ++)
		ptFBTest->ShowPiexl(i, 20, 0xffffff);
	  
    return 0;  
}  


