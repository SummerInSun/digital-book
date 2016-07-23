#include <dis_manag.h>
#include <stdio.h>
#include <string.h>

DECLARE_HEAD(g_tDisOprHead);

int RegisterDispOpr(struct DisOpr *pt_DisOpr)
{
	ListAddTail(&pt_DisOpr->tDisOpr, &g_tDisOprHead);

	return 0;
}

void ShowDisOpr(void)
{
	int iPTNum = 0;
	struct list_head *ptLHTmpPos;	//LH = lis_head
	struct DisOpr *ptDOTmpPos;
	
	LIST_FOR_EACH_ENTRY_H(ptLHTmpPos, &g_tDisOprHead){
		ptDOTmpPos = LIST_ENTRY(ptLHTmpPos, struct DisOpr, tDisOpr);

		printf("%d <---> %s\n", iPTNum++, ptDOTmpPos->name);
	}
}

struct DisOpr *GetDisOpr(char *pcName)
{
	struct list_head *ptLHTmpPos;	//LH = lis_head
	struct DisOpr *ptDOTmpPos;
	
	LIST_FOR_EACH_ENTRY_H(ptLHTmpPos, &g_tDisOprHead){
		ptDOTmpPos = LIST_ENTRY(ptLHTmpPos, struct DisOpr, tDisOpr);

		if(0 == strcmp(pcName, ptDOTmpPos->name)){
			return ptDOTmpPos;
		}
	}

	return NULL;
}

int DisplayInit(void)
{
	int iError;

	iError = FBInit();

	return iError;
}

