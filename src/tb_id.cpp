// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#ifdef TB_RUNTIME_DEBUG_INFO

#include "tb_id.h"
#include "tb_system.h"
#include "tb_hashtable.h"

namespace tb {

// == TBID ==============================================================================

// Hash table for checking if we get any collisions (same hash value for TBID's created
// from different strings)
static TBHashTableAutoDeleteOf<TBID> all_id_hash;
static bool is_adding = false;

void TBID::Set(uint32_t newid)
{
	id = newid;
	debug_string.Clear();
	if (!is_adding && tb_core_is_initialized())
	{
		if (!all_id_hash.Get(id) && debug_string.Length())
		{
			is_adding = true;
			all_id_hash.Add(id, new TBID(*this));
			is_adding = false;
		}
		if (TBID *other_id = all_id_hash.Get(id))
			if (!debug_string.Length())
				debug_string = other_id->debug_string;
	}
}

void TBID::Set(const TBID &newid)
{
	id = newid;
#ifdef TB_RUNTIME_DEBUG_INFO
	debug_string.Set(newid.debug_string);
#endif
	if (!is_adding && tb_core_is_initialized())
	{
		if (TBID *other_id = all_id_hash.Get(id))
		{
			// If this happens, 2 different strings result in the same hash.
			// It might be a good idea to change one of them, but it might not matter.
			if (!debug_string.IsEmpty() && !other_id->debug_string.IsEmpty())
				assert(other_id->debug_string == debug_string);
			// Otherwise copy the debug string
			if (debug_string.IsEmpty() && !other_id->debug_string.IsEmpty())
				debug_string = other_id->debug_string;
		}
		else if (debug_string.Length())
		{
			is_adding = true;
			all_id_hash.Add(id, new TBID(*this));
			is_adding = false;
		}
	}
}

void TBID::Set(const char *string)
{
	id = TBGetHash(string);
#ifdef TB_RUNTIME_DEBUG_INFO
	debug_string.Set(string);
#endif
	if (!is_adding && tb_core_is_initialized())
	{
		if (TBID *other_id = all_id_hash.Get(id))
		{
#ifdef TB_RUNTIME_DEBUG_INFO
			if (other_id->debug_string != debug_string) {
				TBDebugPrint("ID collision btw '%s' and '%s'\n",
							 other_id->debug_string.CStr(), debug_string.CStr());
			}
#endif
			assert(other_id->debug_string == debug_string);
		}
		else if (debug_string.Length())
		{
			assert(debug_string.Length());
			is_adding = true;
			all_id_hash.Add(id, new TBID(*this));
			is_adding = false;
		}
	}
}

const char * TBID::c_str() const
{
	if (debug_string.IsEmpty())
		debug_string.SetFormatted("%d",id);
	return debug_string.CStr();
}

} // namespace tb

#endif // TB_RUNTIME_DEBUG_INFO
