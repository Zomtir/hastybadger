// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#ifdef TB_SYSTEM_EMSCRIPTEN

#ifndef TB_SUBSYSTEM_SDL2
#error "EMSCRIPTEN CAN NOT BE USED WITHOUT SDL2"
#endif

#include "tb_system.h"
#include <iostream>

#include "tb_msg.h"
#include "tb_types.h"
#include <stdio.h>
#include <strings.h>

#include <emscripten.h>
#include "SDL.h"

#if defined(TB_RUNTIME_DEBUG_INFO)

void TBDebugOut(const tb::TBStr & str)
{
	std::cerr << str.CStr();
	SDL_Log("%s", str.CStr());
	SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "%s", str.CStr());
}

#endif // TB_RUNTIME_DEBUG_INFO

namespace tb {

// == TBSystem ========================================

double TBSystem::GetTimeMS()
{
	return emscripten_get_now();
}

static SDL_TimerID tb_sdl_timer_id = 0;
static void tb_sdl_timer_callback(void *param)
{

	double next_fire_time = TBMessageHandler::GetNextMessageFireTime();
	double now = TBSystem::GetTimeMS();
	if (next_fire_time != TB_NOT_SOON && (next_fire_time - now) > 1.0)
	{
		// We timed out *before* we were supposed to (the OS is not playing nice).
		// Calling ProcessMessages now won't achieve a thing so force a reschedule
		// of the platform timer again with the same time.
		emscripten_async_call(tb_sdl_timer_callback, param, int(next_fire_time - now));
		return;
	}

	TBMessageHandler::ProcessMessages();

	// If we still have things to do (because we didn't process all messages,
	// or because there are new messages), we need to rescedule, so call RescheduleTimer.
	next_fire_time = TBMessageHandler::GetNextMessageFireTime();
	if (next_fire_time == TB_NOT_SOON)
	{
		tb_sdl_timer_id = 0;
		return;
	}
	next_fire_time -= TBSystem::GetTimeMS();
	emscripten_async_call(tb_sdl_timer_callback, param, next_fire_time - now);
	return;
}

/** Reschedule the platform timer, or cancel it if fire_time is TB_NOT_SOON.
	If fire_time is 0, it should be fired ASAP.
	If force is true, it will ask the platform to schedule it again, even if
	the fire_time is the same as last time. */
void TBSystem::RescheduleTimer(double fire_time)
{
	// cancel existing timer
	if (tb_sdl_timer_id)
	{
		return;
	}
	// set new timer
	if (fire_time != TB_NOT_SOON && !tb_sdl_timer_id)
	{
		double now = TBSystem::GetTimeMS();
		double delay = fire_time - now;
		tb_sdl_timer_id = 1;
		emscripten_async_call(tb_sdl_timer_callback, NULL, (int)delay);
	}
}

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

int TBSystem::GetDPI()
{
	return _dpi;
}

void TBSystem::SetDPI(int dpi)
{
	_dpi = dpi;
}

#include "tb_subsystem_sdl.cpp"

} // namespace tb

#endif // TB_SYSTEM_EMSCRIPTEN
