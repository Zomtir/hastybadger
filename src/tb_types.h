// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#ifndef TB_TYPES_H
#define TB_TYPES_H

// Include <tb_config.h> so it may be overridden in application directory.
// The default "tb_config.h" (local) will be used if there is no other match.

#include <string.h>
#include <cstdint>

namespace tb {

template <class T>
T Max(const T& left, const T& right) { return left > right ? left : right; }

template <class T>
T Min(const T& left, const T& right) { return left < right ? left : right; }

template <class T>
T Abs(const T& value) { return value < 0 ? -value : value; }

template <class T>
T Clamp(const T& value, const T& min, const T& max)
	{ return (value > max) ? max : ((value < min) ? min : value); }

/** Returns value clamped to min and max. If max is greater than min,
	max will be clipped to min. */
template <class T>
T ClampClipMax(const T& value, const T& min, const T& max)
{
	return (value > max)
		? (max > min ? max : min)
		: ((value < min) ? min : value);
}

#ifndef MAX
/** This is deprecated! Use Max(a, b)! */
#define MAX(a, b) Max(a, b)
#endif

#ifndef MIN
/** This is deprecated! Use Min(a, b)! */
#define MIN(a, b) Min(a, b)
#endif

#ifndef ABS
/** This is deprecated! Use Abs(value)! */
#define ABS(value) Abs(value)
#endif

#ifndef _WIN32
/** Mark a function as taking printf-style arguments */
#define TB_POST_FORMAT(a,b) __attribute__ ((format (printf, a, b)))
#else
/** Mark a function as taking printf-style arguments */
#define TB_POST_FORMAT(a,b)
#endif

#ifndef CLAMP
/** This is deprecated! Use Clamp(value, min, max)! */
#define CLAMP(value, min, max) Clamp(value, min, max)
#endif

/** Makes it possible to use the given enum types as flag combinations.
	That will catch use of incorrect type during compilation, that wouldn't be caught
	using a uint32_t flag. */
#define MAKE_ENUM_FLAG_COMBO(Enum) \
	inline Enum operator | (Enum a, Enum b)  { return static_cast<Enum>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b)); } \
	inline Enum operator & (Enum a, Enum b)  { return static_cast<Enum>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b)); } \
	inline Enum operator ^ (Enum a, Enum b)  { return static_cast<Enum>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b)); } \
	inline void operator |= (Enum &a, Enum b) { a = static_cast<Enum>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b)); } \
	inline void operator &= (Enum &a, Enum b) { a = static_cast<Enum>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b)); } \
	inline void operator ^= (Enum &a, Enum b) { a = static_cast<Enum>(static_cast<uint32_t>(a) ^ static_cast<uint32_t>(b)); } \
	inline Enum operator ~ (Enum a)  { return static_cast<Enum>(~static_cast<uint32_t>(a)); }

} // namespace tb

#endif // TB_TYPES_H
