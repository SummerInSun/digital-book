#include <input_manag.h>
#include <draw.h>

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

static struct tsdev *g_ptTouchScreenFd;
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
	ts_close(g_ptTouchScreenFd);

	return 0;
}

static int TSInputDeviceInit(void)
{
	char *pcTSName = NULL;
	int iError = 0;

	if((pcTSName = getenv("TSLIB_TSDEVICE"))){
		g_ptTouchScreenFd = ts_open(pcTSName, 1);	
	}else{
		g_ptTouchScreenFd = ts_open("/dev/event0", 1);
	}

	if(NULL == g_ptTouchScreenFd){
		printf("Open ts device error\n");
		return -1;
	}

	iError = ts_config(g_ptTouchScreenFd);
	if(iError){
		printf("Config ts device error\n");
		return -1;
	}

	g_tTSInputOpr.iInputDeviceFd = ts_fd(g_ptTouchScreenFd);
	
	return GetDisDeviceSize(&g_iTSXres, &g_iTSYres);
}

static int OutOfTimeToSet(struct timeval *ptPreTime, struct timeval *ptCurTime)
{
	int iPreTime, iCurTime;

	iPreTime = ptPreTime->tv_sec * 1000 + ptPreTime->tv_usec / 1000;
	iCurTime = ptCurTime->tv_sec * 1000 + ptCurTime->tv_usec / 1000;

	if((iCurTime - iPreTime > 500)){
		return -1;
	}

	return 0;
}

static int TSGetInputEvent(struct InputEvent *ptInputEvent)
{
	struct ts_sample tTSSample;
	int iError = 0;
	int iTimeDistance;

	iError = ts_read(g_ptTouchScreenFd, &tTSSample, 1);
	if(iError != 1){
		DEBUG_PRINTF("TS read error\n");
		return -1;
	}

	/* 可能是驱动程序的问题，这里不能使用ts_read出来的时间数据，否则的话会进不去下面的
	 * if 判断，并且读取数据有bug，有时候会出现按了好几次但是翻页没有那么多次
	 * 然后再往后按下一次的时候突然多翻几次页
	 */
	gettimeofday(&g_tCurTimeVal, NULL);
#if 0
	printf("tTSSample.sec = %d, tTSSample.usec = %d\n", tTSSample.tv.tv_sec, tTSSample.tv.tv_usec);
	printf("g_tPreTimeVal.sec = %d, g_tPreTimeVal.usec = %d\n", g_tPreTimeVal.tv_sec, g_tPreTimeVal.tv_usec);
	printf("xres = %d yres = %d\n", tTSSample.x, tTSSample.y);

	printf("touchscreen get event\n");
	printf("xres = %d yres = %d\n", tTSSample.x, tTSSample.y);
#endif	
	if(OutOfTimeToSet(&g_tPreTimeVal, &g_tCurTimeVal)){
		
		g_tPreTimeVal = g_tCurTimeVal;
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
		DEBUG_PRINTF("%s %d %s\n", __FILE__, __LINE__, __FUNCTION__);
		return 0;
	}

	return -1;
}

int TouchScreenInit(void)
{
	TSInputDeviceInit();
	return RegisterInputOpr(&g_tTSInputOpr);
}

