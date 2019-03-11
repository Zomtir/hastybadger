// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#ifdef TB_SYSTEM_LINUX

#include "tb_system.h"
#include "tb_str.h"
#include <sys/time.h>
#include <stdio.h>

#if defined TB_RUNTIME_DEBUG_INFO && !defined TB_SUBSYSTEM_SDL2

void TBDebugOut(const tb::TBStr & str)
{
	printf("%s", (const char *)str);
}

#endif // TB_RUNTIME_DEBUG_INFO

namespace tb {

// == TBSystem ========================================

#if !defined TB_SUBSYSTEM_SDL2 && !defined TB_SUBSYSTEM_GLFW

double TBSystem::GetTimeMS()
{
	struct timeval now;
	gettimeofday( &now, NULL );
	return now.tv_usec/1000 + now.tv_sec*1000;
}

// Implementation currently done in port_glfw.cpp.
// FIXME: Implement here for linux-desktop/macos?
void TBSystem::RescheduleTimer(double fire_time)
{
}

#endif // !TB_SUBSYSTEM

int TBSystem::GetLongClickDelayMS()
{
	return 500;
}

int TBSystem::GetPanThreshold()
{
	return 5 * GetDPI() / 96;
}

int TBSystem::GetPixelsPerLine()
{
	return 40 * GetDPI() / 96;
}

int TBSystem::_dpi = 96;

// FIXME! Implement!
int TBSystem::GetDPI()
{
	return _dpi;
}

void TBSystem::SetDPI(int dpi)
{
	_dpi = dpi;
}

const char * TBSystem::GetRoot()
{
	return "./";
}

} // namespace tb

#endif // TB_SYSTEM_LINUX
