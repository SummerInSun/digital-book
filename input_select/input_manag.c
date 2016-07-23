#include <input_manag.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

DECLARE_HEAD(g_tInputOprHead);

static fd_set g_tFdSet;
static int g_iMaxFd = -1;

int RegisterInputOpr(struct InputOpr *ptInputOpr)
{	
	ListAddTail(&ptInputOpr->tInputOpr, &g_tInputOprHead);

	return 0;
}

struct InputOpr *GetInputOpr(char *pcName)
{
	
	struct list_head *ptLHTmpPos;	//LH = lis_head
	struct InputOpr *ptIOTmpPos;
	
	LIST_FOR_EACH_ENTRY_H(ptLHTmpPos, &g_tInputOprHead){
		ptIOTmpPos = LIST_ENTRY(ptLHTmpPos, struct InputOpr, tInputOpr);

		if(0 == strcmp(pcName, ptIOTmpPos->name)){
			return ptIOTmpPos;
		}
	}

	return NULL;
}

void ShowInputOpr(void)
{	
	int iPTNum = 0;
	struct list_head *ptLHTmpPos;	//LH = lis_head
	struct InputOpr *ptIOTmpPos;
	
	LIST_FOR_EACH_ENTRY_H(ptLHTmpPos, &g_tInputOprHead){
		ptIOTmpPos = LIST_ENTRY(ptLHTmpPos, struct InputOpr, tInputOpr);

		printf("%d <---> %s\n", iPTNum++, ptIOTmpPos->name);
	}
}

int AllInputFdInit()
{
	int iError = 0;
	struct list_head *ptLHTmpPos;	//LH = lis_head
	struct InputOpr *ptIOTmpPos;

	FD_ZERO(&g_tFdSet);
	
	LIST_FOR_EACH_ENTRY_H(ptLHTmpPos, &g_tInputOprHead){
		ptIOTmpPos = LIST_ENTRY(ptLHTmpPos, struct InputOpr, tInputOpr);

		FD_SET(ptIOTmpPos->iInputDeviceFd, &g_tFdSet);
		if(ptIOTmpPos->iInputDeviceFd > g_iMaxFd){
			g_iMaxFd = ptIOTmpPos->iInputDeviceFd;
		}	
	}

	g_iMaxFd ++;

	return 0;
}

int GetInputEvent(struct InputEvent *ptInputEvent)
{
	int iError = 0;
	struct list_head *ptLHTmpPos;	//LH = lis_head
	struct InputOpr *ptIOTmpPos;

	/* 等待无限长的时间 */
	iError = select(g_iMaxFd, &g_tFdSet, NULL, NULL, NULL);

	if(iError > 0){
		LIST_FOR_EACH_ENTRY_H(ptLHTmpPos, &g_tInputOprHead){
			ptIOTmpPos = LIST_ENTRY(ptLHTmpPos, struct InputOpr, tInputOpr);

			if(FD_SET(ptIOTmpPos->iInputDeviceFd, &g_tFdSet)){
				iError = ptIOTmpPos->GetInputEvent(ptInputEvent);
				if(!iError){
					/* 一旦有一个事件发生就返回为0 */
//					printf("return %s\n", ptIOTmpPos->name);
					return 0;
				}
			}
		}
	}

	return -1;
}

int InputDeviceInit(void)
{
	int iError = 0;

	iError = TouchScreenInit();
	if(iError){
		DEBUG_PRINTF("ConsoleInit error\n");
		return -1;
	}

	iError = ConsoleInit();
	if(iError){
		DEBUG_PRINTF("ConsoleInit error\n");
		return -1;
	}

	return 0;
}

int InputDeviceExit(void)
{
	struct list_head *ptLHTmpPos;	//LH = lis_head
	struct InputOpr *ptIOTmpPos;
	
	LIST_FOR_EACH_ENTRY_H(ptLHTmpPos, &g_tInputOprHead){
		ptIOTmpPos = LIST_ENTRY(ptLHTmpPos, struct InputOpr, tInputOpr);

		ptIOTmpPos->InputDeviceExit();
	}

	return 0;
}

