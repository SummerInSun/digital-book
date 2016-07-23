#include <dis_manag.h>
#include <common_st.h>
#include <common_config.h>
#include <font_manag.h>
#include <encoding_manag.h>
#include <print_manag.h>
#include <draw.h>
#include <input_manag.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdio.h>
#include <string.h>
#include <termios.h>

struct FontDisOpt *g_tMainFontDisOpt;
struct InputEvent g_ptInputEvent;

struct termios g_tOldTermIos;

void ResetTermSetting(void)
{
	tcsetattr(0, TCSANOW, &g_tOldTermIos);
}

/* 设置控制台不需要按下回车即可接受并处理字符
 * 在windows里面可以使用 _getch()；但是linux里面没有这个函数
 */
void DisableTermReturn(void)
{
    struct termios tNewTermIos;
	
    /* read terminal settings */
    tcgetattr(0, &g_tOldTermIos); 

	/* init new settings */
    tNewTermIos = g_tOldTermIos;  
    /* change settings */
    tNewTermIos.c_lflag &= ~(ICANON | ECHO);
    /* apply */
    tcsetattr(0, TCSANOW, &tNewTermIos);
}

int main(int argc, char *argv[])  
{  
	int iError = 0;

	if(argc != 4){
		DebugPrint("exe <fontfile> <text> <size>\n");

		goto error_exit;
	}

	iError = PrintDeviceInit();
	if(iError){
		goto error_exit;
	}

	iError = DisplayInit();
	if(iError){
		
		goto error_exit;
	}
	
	iError = FontInit();
	if(iError){
		goto error_exit;
	}
	iError = EncodingInit();
	if(iError){
		goto error_exit;
	}
	
	iError = OpenTextFile(argv[2]);
	if(iError){
		goto error_exit;
	}
	iError = SetTextDetail("HZK16", argv[1], atoi(argv[3]));
	if(iError){
		DebugPrint("This font can't be support. check the file format or file size\n");
		goto error_exit;
	}

	iError = SelectAndInitDisplay("fb");
	if(iError){
		goto error_exit;
	}
	ShowNextPage();

	DebugPrint("\nUse the \"n\" to turn down\n");
	DebugPrint("Use the \"u\" to turn up\n");
	DebugPrint("Use the \"q\" to exit\n\n");

	DebugPrint("Use the \"l\" to lean the font\n");
	DebugPrint("Use the \"o\" to overstriking the font\n");
	DebugPrint("Use the \"e\" to add underline\n");
	DebugPrint("Use the \"r\" to recovery the font\n");

	iError = InputDeviceInit();
	if(iError){
		DEBUG_PRINTF("InputDeviceInit failed\n");
		goto error_exit;
	}
	
	iError = AllInputFdInit();
	if(iError){
		DEBUG_PRINTF("InputDeviceInit failed\n");
		goto error_exit;
	}
#if 1
	DebugPrint("\nSupported display device\n");
	ShowDisOpr();
	DebugPrint("\nSupported encoding\n");
	ShowEncodingOpr();
	DebugPrint("\nSupported font\n");
	ShowFontOpr();
	DebugPrint("\nSupported print device\n");
	ShowPrintOpr();
	DebugPrint("\nSupported input device\n");
	ShowInputOpr();
#endif

	while(1){
			GetInputEvent(&g_ptInputEvent);
			DEBUG_PRINTF("InputCode = %d\n", g_ptInputEvent.iInputCode);
			if(g_ptInputEvent.iInputCode == INPUT_CODE_DOWN){
				ShowNextPage();
			}
			if(g_ptInputEvent.iInputCode == INPUT_CODE_UP){
				ShowPrePage();
			}
			if(g_ptInputEvent.iInputCode == INPUT_CTRL_LEAN){
				g_tMainFontDisOpt = GetFontDisOpt();
				g_tMainFontDisOpt->ucFontMode |= LEAN_FONT;
				ReShowCurPage();	/* 字体倾斜 */
			}
			if(g_ptInputEvent.iInputCode == INPUT_CTRL_OVERSTRIKING){
				g_tMainFontDisOpt = GetFontDisOpt();
				g_tMainFontDisOpt->ucFontMode |= OVERSTRIKING_FONT;
				ReShowCurPage();	/* 字体加粗 */
			}
			if(g_ptInputEvent.iInputCode == INPUT_CTRL_UNDERLINE){
				g_tMainFontDisOpt = GetFontDisOpt();
				g_tMainFontDisOpt->ucFontMode |= UNDERLINE_FONT;
				ReShowCurPage();	/* 字体加下划线 */
			}
			if(g_ptInputEvent.iInputCode == INPUT_CTRL_RECOVERY){
				g_tMainFontDisOpt = GetFontDisOpt();
				g_tMainFontDisOpt->ucFontMode = NORMAL_FONT;
				ReShowCurPage();	/* 字体恢复正常 */
			}
			if(g_ptInputEvent.iInputCode == INPUT_CODE_UNKNOW){

				;
			}
			if(g_ptInputEvent.iInputCode == INPUT_CODE_EXIT){
				goto exit;
			}
	}

exit:
	InputDeviceExit();
    return 0;
error_exit:
	InputDeviceExit();
    return -1;
}  


