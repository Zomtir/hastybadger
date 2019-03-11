// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#ifdef TB_SYSTEM_ANDROID

#ifndef TB_SUBSYSTEM_SDL2
#error "ANDROID CAN NOT BE USED WITHOUT SDL2"
#endif

#include "tb_system.h"
#include "tb_debug.h"
#include "tb_str.h"

#include <android/log.h>
#include <sys/time.h>
#include <stdio.h>

// for native asset manager
#include <sys/types.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/configuration.h>

#if defined TB_RUNTIME_DEBUG_INFO

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, "TB", __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, "TB", __VA_ARGS__)

// Used with and without SDL2
void TBDebugOut(const tb::TBStr & str)
{
	LOGI("%s", str.CStr());
}

#endif // TB_RUNTIME_DEBUG_INFO

AAssetManager *g_pManager = NULL;

void SetAssetManager(AAssetManager *pManager)
{
	g_pManager = pManager;
}

namespace tb {

// == TBSystem ========================================

// Well, one day it Android might be usable without SDL??
#if !defined TB_SUBSYSTEM_SDL2 && !defined TB_SUBSYSTEM_GLFW
double TBSystem::GetTimeMS()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return now.tv_usec / 1000 + now.tv_sec * 1000;
}

// FIXME Needs implementation
void TBSystem:::RescheduleTimer(double fire_time)
{
}

// FIXME Needs correct implementation
char * TBSystem::GetRoot()
{
	return "./";
}
#endif // !TB_SUBSYSTEM

int TBSystem::GetLongClickDelayMS()
{
	return 500;
}

int TBSystem::GetPanThreshold()
{
	return 5 * GetDPI() / 120; // Potentially only 96!
}

int TBSystem::GetPixelsPerLine()
{
	return 40 * GetDPI() / 120; // Potentially only 96!
}

int TBSystem::_dpi = 96;

int TBSystem::GetDPI()
{
	AConfiguration *config = AConfiguration_new();
	AConfiguration_fromAssetManager(config, g_pManager);
	_dpi = AConfiguration_getDensity(config);
	AConfiguration_delete(config);
	if (_dpi == 0 || _dpi == ACONFIGURATION_DENSITY_NONE)
		_dpi = 120;
	return _dpi;
}

void TBSystem::SetDPI(int dpi)
{
	// FIXME This does nothing, should it overrule GetDPI?
	_dpi = dpi;
}

#if defined TB_SUBSYSTEM_SDL2
const char * TBSystem::GetRoot()
{
	static char * basepath = NULL;
	TBStr ExtPath(SDL_AndroidGetExternalStoragePath());
	ExtPath.Append("/");
	basepath = strdup(ExtPath.CStr());
	return basepath;
}
#endif // TB_SUBSYSTEM_SDL2

} // namespace tb

#endif // TB_SYSTEM_ANDROID
