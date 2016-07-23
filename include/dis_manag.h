/* LCD 显示控制管理头文件
 * 包括显示模块的结构体以及相应的操作函数
 *
 */

#ifndef __DIS_MANAG_H__
#define __DIS_MANAG_H__

#include <common_st.h>
#include <common_config.h>

struct DisOpr{
	char *name;
	int iXres;
	int iYres;
	int iBpp;
	int (*DisDeviceInit)(void);
	int (*ShowPiexl)(int iPenX, int iPeny, unsigned int dwColor);
	int (*CleanScreen)(unsigned int dwBackColor);
	struct list_head tDisOpr;
};

int RegisterDispOpr(struct DisOpr *pt_DisOpr);
void ShowDisOpr(void);
struct DisOpr *GetDisOpr(char *pcName);
int DisplayInit(void);
int FBInit(void);

#endif

