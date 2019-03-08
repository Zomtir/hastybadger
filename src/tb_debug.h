// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#ifdef TB_RUNTIME_DEBUG_INFO

#ifndef TB_DEBUG_H
#define TB_DEBUG_H

#include "tb_types.h"

namespace tb {

class TBDebugInfo
{
public:
	TBDebugInfo();

	enum SETTING {
		/** Show widgets bounds */
		LAYOUT_BOUNDS,
		/** Show child widget clipping set by some widgets. */
		LAYOUT_CLIPPING,
		/** Show highlights on widgets that recalculate their preferred
			size, and those who recalculate their layout. */
		LAYOUT_PS_DEBUGGING,
		/** Show render batch info and log batch info in the debug
			output. It depends on the renderer backend if this is available. */
		RENDER_BATCHES,
		/** Render the bitmap fragments of the skin. */
		RENDER_SKIN_BITMAP_FRAGMENTS,
		/** Render the bitmap fragments of the font that's set on the hovered
			or focused widget. */
		RENDER_FONT_BITMAP_FRAGMENTS,

		NUM_SETTINGS
	};
	int settings[NUM_SETTINGS];
};

extern TBDebugInfo g_tb_debug;

/** Show a window containing runtime debugging settings. */
void ShowDebugInfoSettingsWindow(class TBWidget *root);

} // namespace tb

namespace tb { class TBStr; }
void TBDebugOut(const tb::TBStr & str);

// Cannot be turned into a proper function at the moment
// Variable arguments cannot be passed easily without wrapping
#define TBDebugPrint(...) \
{ \
	tb::TBStr tmp; \
	tmp.SetFormatted(__VA_ARGS__); \
	TBDebugOut(tmp); \
}

#if defined TB_FONT_RENDERER_FREETYPE || defined TB_FONT_RENDERER_FREETYPEX
class FT_Error;
inline void TBDebugError(FT_Error err)
{
	TBDebugPrint("%s %s:%d = %d\n", (err ? "Error" : "OK" ), __FILE__, __LINE__, err);
}
#endif // TB_FONT_RENDERER_FREETYPE(X)

#endif // TB_DEBUG_H

#else // TB_RUNTIME_DEBUG_INFO

// If this is used with empty functions, the TB_RUNTIME_DEBUG_INFO checks can be removed from the code

#endif // TB_RUNTIME_DEBUG_INFO
