/* LCD ��ʾ���ƹ���ͷ�ļ�
 * ������ʾģ��Ľṹ���Լ���Ӧ�Ĳ�������
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

