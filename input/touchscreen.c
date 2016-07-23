#include <input_manag.h>
#include <draw.h>
#include <print_manag.h>

#include <stdlib.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <tslib.h>

static int TSInputDeviceExit(void);
static int TSInputDeviceInit(void);
static int TSGetInputEvent(struct InputEvent *ptInputEvent);

static struct tsdev *g_ptTouchScreenFd = NULL;
static int g_iTSXres;
static int g_iTSYres;
static struct timeval g_tPreTimeVal;
static struct timeval g_tCurTimeVal;

struct InputOpr g_tTSInputOpr = {
	.name = "touchsrceen-input",
	.InputDeviceExit = TSInputDeviceExit,
	.InputDeviceInit = TSInputDeviceInit,
	.GetInputEvent   = TSGetInputEvent,
};

static int TSInputDeviceExit(void)
{
	if(NULL != g_ptTouchScreenFd){
		ts_close(g_ptTouchScreenFd);
	}

	return 0;
}

static int TSInputDeviceInit(void)
{
	char *pcTSName = NULL;
	int iError = 0;

	if((pcTSName = getenv("TSLIB_TSDEVICE"))){
		g_ptTouchScreenFd = ts_open(pcTSName, 0);	
	}else{
		g_ptTouchScreenFd = ts_open("/dev/event0", 0);
	}

	if(NULL == g_ptTouchScreenFd){
		DebugPrint(DEBUG_NOTICE"Open ts device error\n");
		return -1;
	}
	
	iError = ts_config(g_ptTouchScreenFd);
	if(iError){
		DebugPrint(DEBUG_NOTICE"Config ts device error\n");
		return -1;
	}
	
	g_tTSInputOpr.iInputDeviceFd = ts_fd(g_ptTouchScreenFd);

	return GetDisDeviceSize(&g_iTSXres, &g_iTSYres);;
}

static int OutOfTimeToSet(struct timeval *ptPreTime, struct timeval *ptCurTime)
{
	int iPreTime, iCurTime;

	iPreTime = ptPreTime->tv_sec * 1000 + ptPreTime->tv_usec / 1000;
	iCurTime = ptCurTime->tv_sec * 1000 + ptCurTime->tv_usec / 1000;

	if((iCurTime - iPreTime > 200)){
		return -1;
	}

	return 0;
}

static int TSGetInputEvent(struct InputEvent *ptInputEvent)
{
	static struct ts_sample tTSPress;
	static struct ts_sample tTSRelease;
	static int iPressFlag = 0;
	
	struct ts_sample tTSSample;
	int iError = 0;

	DebugPrint(DEBUG_DEBUG"TS read data\n");

	iError = ts_read(g_ptTouchScreenFd, &tTSSample, 1);
	if(iError != 1){
		DebugPrint(DEBUG_WARNING"TS read error\n");
		return -1;
	}

	/* 可能是驱动程序的问题，这里不能使用ts_read出来的时间数据，否则的话会进不去下面的
	 * if 判断，并且读取数据有bug，有时候会出现按了好几次但是翻页没有那么多次
	 * 然后再往后按下一次的时候突然多翻几次页
	 * 加上多线程之后就没问题了，应该是读取的问题
	 */
	gettimeofday(&g_tCurTimeVal, NULL);
#if 0
	printf("tTSSample.sec = %d, tTSSample.usec = %d\n", tTSSample.tv.tv_sec, tTSSample.tv.tv_usec);
	printf("g_tPreTimeVal.sec = %d, g_tPreTimeVal.usec = %d\n", g_tPreTimeVal.tv_sec, g_tPreTimeVal.tv_usec);
	printf("xres = %d yres = %d\n", tTSSample.x, tTSSample.y);

	printf("touchscreen get event\n");
	printf("xres = %d yres = %d\n", tTSSample.x, tTSSample.y);
#endif	
#if 1
	if(OutOfTimeToSet(&g_tPreTimeVal, &tTSSample.tv)){
		
		g_tPreTimeVal = tTSSample.tv;
		ptInputEvent->iInputType = INPUT_TYPE_TS;
		ptInputEvent->iInputCode = INPUT_CODE_UNKNOW;

		if((tTSSample.x > g_iTSXres / 3) && (tTSSample.x < g_iTSXres * 2 /3) && 
			(tTSSample.y > g_iTSYres * 2 / 3))
		{
			ptInputEvent->iInputCode = INPUT_CODE_DOWN;
		}

		if((tTSSample.x > g_iTSXres / 3) && (tTSSample.x < g_iTSXres * 2 /3) && 
			(tTSSample.y < g_iTSYres / 3))
		{
			ptInputEvent->iInputCode = INPUT_CODE_UP;
		}

		if((tTSSample.x > g_iTSXres * 2 /3) && 
			(tTSSample.y > g_iTSYres / 3) && (tTSSample.y < g_iTSYres * 2 / 3))
		{
			ptInputEvent->iInputCode = INPUT_CODE_EXIT;
		}
		DebugPrint(DEBUG_DEBUG"InputCode = %d\n", ptInputEvent->iInputCode);
		return 0;
	}

	if(tTSSample.pressure == 1){
		if(0 == iPressFlag){
			DebugPrint(DEBUG_DEBUG"you press\n");
			DebugPrint(DEBUG_DEBUG"yres = %d, xres = %d\n", tTSSample.y, tTSSample.x);
			tTSPress = tTSSample;
			iPressFlag = 1;
		}
	}else if(tTSSample.pressure == 0){
		if(1 == iPressFlag){
			tTSRelease = tTSSample;
			iPressFlag = 0;

			DebugPrint(DEBUG_DEBUG"you release\n");

			ptInputEvent->iInputType = INPUT_TYPE_TS;
			ptInputEvent->iInputCode = INPUT_CODE_UNKNOW;

			if((tTSRelease.y - tTSPress.y) > g_iTSYres / 3){
				ptInputEvent->iInputCode = INPUT_CODE_DOWN;
			}
			if((tTSRelease.y - tTSPress.y) < 0 - g_iTSYres / 3){
				ptInputEvent->iInputCode = INPUT_CODE_UP;
			}
			if((tTSRelease.x - tTSPress.x) > g_iTSXres * 2 / 3){
				ptInputEvent->iInputCode = INPUT_CODE_EXIT;
			}
			DebugPrint(DEBUG_DEBUG"yres = %d, xres = %d\n", tTSRelease.y, tTSRelease.x);
			DebugPrint(DEBUG_DEBUG"ydis = %d, xdis = %d\n", tTSRelease.y - tTSPress.y, tTSRelease.x - tTSPress.x);

			DebugPrint(DEBUG_DEBUG"Input code %d\n", ptInputEvent->iInputCode);

			return 0;
		}
	}
	
#endif
	return -1;
}

int TouchScreenInit(void)
{
	int iError = 0;
	
	iError = TSInputDeviceInit();
	iError |= RegisterInputOpr(&g_tTSInputOpr);
	
	return iError;
}

void TouchScreenExit(void)
{
	TSInputDeviceExit();
	UnregisterInputOpr(&g_tTSInputOpr);
}


