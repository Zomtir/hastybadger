// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#include "tb_value.h"
#include "tb_object.h"
#include "tb_str.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

namespace tb {

// FIX: ## Floating point string conversions might be locale dependant. Force "." as decimal!

// == Helper functions ============================

char *next_token(char *&str, const char *delim) {
	str += strspn(str, delim);
	if (!*str)
		return nullptr;
	char *token = str;
	str += strcspn(str, delim);
	if (*str)
		*str++ = '\0';
	return token;
}

bool is_start_of_number(const char *str)
{
	if (*str == '-')
		str++;
	if (*str == '.')
		str++;
	return *str >= '0' && *str <= '9';
}

bool contains_non_trailing_space(const char *str)
{
	if (const char *p = strstr(str, " "))
	{
		while (*p == ' ')
			p++;
		return *p != '\0';
	}
	return false;
}

bool is_number_only(const char *s)
{
	if (!s || *s == 0 || *s == ' ')
		return 0;
	char *p;
	strtod(s, &p);
	while (*p == ' ')
		p++;
	return *p == '\0';
}

bool is_number_float(const char *str)
{
	while (*str) if (*str++ == '.') return true;
	return false;
}

// == TBValueArray ==================================

TBValueArray::TBValueArray()
{
}

TBValueArray::~TBValueArray()
{
}

TBValue *TBValueArray::AddValue()
{
	TBValue *v;
	if ((v = new TBValue()) && m_list.Add(v))
		return v;
	delete v;
	return nullptr;
}

TBValue *TBValueArray::GetValue(int index)
{
	if (index >= 0 && index < m_list.GetNumItems())
		return m_list[index];
	return nullptr;
}

TBValueArray *TBValueArray::Clone(TBValueArray *source)
{
	TBValueArray *new_arr = new TBValueArray;
	if (!new_arr)
		return nullptr;
	for (int i = 0; i < source->m_list.GetNumItems(); i++)
	{
		TBValue *new_val = new_arr->AddValue();
		if (!new_val)
		{
			delete new_arr;
			return nullptr;
		}
		new_val->Copy(*source->GetValue(i));
	}
	return new_arr;
}

// == TBValue =======================================

TBValue::TBValue()
	: m_packed_init(0)
{
}

TBValue::TBValue(const TBValue &value)
	: m_packed_init(0)
{
	Copy(value);
}

TBValue::TBValue(TYPE type)
	: m_packed_init(0)
{
	switch (type)
	{
	case TYPE_NULL:
		SetNull();
		break;
	case TYPE_STRING:
		SetString("", SET_AS_STATIC);
		break;
	case TYPE_FLOAT:
		SetFloat(0);
		break;
	case TYPE_INT:
		SetInt(0);
		break;
	case TYPE_OBJECT:
		SetObject(nullptr, SET_AS_STATIC);
		break;
	case TYPE_ARRAY:
		if (TBValueArray *arr = new TBValueArray())
			SetArray(arr, SET_TAKE_OWNERSHIP);
		break;
	default:
		assert(!"Not implemented!");
	}
}

TBValue::TBValue(long value)
	: m_packed_init(0)
{
	SetInt(value);
}

TBValue::TBValue(double value)
	: m_packed_init(0)
{
	SetFloat(value);
}

TBValue::TBValue(const char *value, SET_MODE set)
	: m_packed_init(0)
{
	SetString(value, set);
}

TBValue::TBValue(TBStr && str)
{
	m_packed.type = TYPE_STRING;
	m_packed.allocated = 1;
	val_str = str.s;
	str.s = nullptr; // kinda bogus, str.s should never be nullptr,
					 // but it's about to be deleted, and the
					 // destructor checks for null, so it's ok here.
}

TBValue::TBValue(TBTypedObject *object, SET_MODE set)
	: m_packed_init(0)
{
	SetObject(object, set);
}

TBValue::~TBValue()
{
	SetNull();
}

void TBValue::TakeOver(TBValue &source_value)
{
	if (source_value.m_packed.type == TYPE_STRING)
		SetString(source_value.val_str, source_value.m_packed.allocated ? SET_TAKE_OWNERSHIP : SET_NEW_COPY);
	else if (source_value.m_packed.type == TYPE_ARRAY)
		SetArray(source_value.val_arr, source_value.m_packed.allocated ? SET_TAKE_OWNERSHIP : SET_NEW_COPY);
	else
		*this = source_value;
	source_value.m_packed.type = TYPE_NULL;
}

void TBValue::Copy(const TBValue &source_value)
{
	if (source_value.m_packed.type == TYPE_STRING)
		SetString(source_value.val_str, SET_NEW_COPY);
	else if (source_value.m_packed.type == TYPE_ARRAY)
		SetArray(source_value.val_arr, SET_NEW_COPY);
	else if (source_value.m_packed.type == TYPE_OBJECT)
	{
		assert(!"We can't copy objects! The value will be nulled!");
		SetObject(nullptr, SET_TAKE_OWNERSHIP);
	}
	else
	{
		SetNull();
		//memcpy(this, &source_value, sizeof(TBValue));
		//*this = source_value;
		val_float = source_value.val_float;
		m_packed_init = source_value.m_packed_init;
	}
}

void TBValue::SetNull()
{
	if (m_packed.allocated)
	{
		if (m_packed.type == TYPE_STRING)
			free(val_str);
		else if (m_packed.type == TYPE_OBJECT)
			delete val_obj;
		else if (m_packed.type == TYPE_ARRAY)
			delete val_arr;
	}
	m_packed.type = TYPE_NULL;
}

void TBValue::SetInt(long val)
{
	SetNull();
	m_packed.type = TYPE_INT;
	val_int = val;
}

void TBValue::SetFloat(double val)
{
	SetNull();
	m_packed.type = TYPE_FLOAT;
	val_float = val;
}

void TBValue::SetString(const char *val, SET_MODE set)
{
	SetNull();
	m_packed.allocated = (set == SET_NEW_COPY || set == SET_TAKE_OWNERSHIP);
	if (set != SET_NEW_COPY)
	{
		val_str = const_cast<char *>(val);
		m_packed.type = TYPE_STRING;
	}
	else if ((val_str = strdup(val)))
		m_packed.type = TYPE_STRING;
}

void TBValue::SetObject(TBTypedObject *object, SET_MODE set)
{
	if (set == SET_NEW_COPY) {
		assert(!"Can't copy objects");
	}
	SetNull();
	m_packed.allocated = set == SET_TAKE_OWNERSHIP;
	val_obj = object;
	m_packed.type = TYPE_OBJECT;
}

void TBValue::SetArray(TBValueArray *arr, SET_MODE set)
{
	SetNull();
	m_packed.allocated = (set == SET_NEW_COPY || set == SET_TAKE_OWNERSHIP);
	if (set != SET_NEW_COPY)
	{
		val_arr = arr;
		m_packed.type = TYPE_ARRAY;
	}
	else if ((val_arr = TBValueArray::Clone(arr)))
		m_packed.type = TYPE_ARRAY;
}

void TBValue::SetFromStringAuto(const char *str, SET_MODE set)
{
	if (!str)
		SetNull();
	else if (is_number_only(str))
	{
		if (is_number_float(str))
			SetFloat(atof(str));
		else
			SetInt(atol(str));
	}
	else if (is_start_of_number(str) && contains_non_trailing_space(str))
	{
		// If the number has nontrailing space, we'll assume a list of numbers (example: "10 -4 3.5")
		SetNull();
		if (TBValueArray *arr = new TBValueArray)
		{
			TBStr tmpstr;
			if (tmpstr.Set(str))
			{
				char *str_next = tmpstr.CStr();
				while (char *token = next_token(str_next, ", "))
				{
					if (TBValue *new_val = arr->AddValue())
						new_val->SetFromStringAuto(token, SET_NEW_COPY);
				}
			}
			SetArray(arr, SET_TAKE_OWNERSHIP);
		}
	}
	else if (*str == '[')
	{
		SetNull();
		if (TBValueArray *arr = new TBValueArray)
		{
			assert(!"not implemented! Split out the tokenizer code above!");
			SetArray(arr, SET_TAKE_OWNERSHIP);
		}
	}
	else
	{
		SetString(str, set);
		return;
	}
	// We didn't set as string, so we might need to deal with the passed in string data.
	if (set == SET_TAKE_OWNERSHIP)
	{
		// Delete the passed in data
		TBValue tmp;
		tmp.SetString(str, SET_TAKE_OWNERSHIP);
	}
}

long TBValue::GetInt() const
{
	if (m_packed.type == TYPE_STRING)
		return atol(val_str);
	else if (m_packed.type == TYPE_FLOAT)
		return (long) val_float;
	return m_packed.type == TYPE_INT ? val_int : 0;
}

double TBValue::GetFloat() const
{
	if (m_packed.type == TYPE_STRING)
		return atof(val_str);
	else if (m_packed.type == TYPE_INT)
		return (double) val_int;
	return m_packed.type == TYPE_FLOAT ? val_float : 0;
}

TBStr TBValue::GetString() const
{
	TBStr text;
	switch (m_packed.type) {
	case TYPE_INT:
		text.SetFormatted("%ld", val_int);
		break;
	case TYPE_FLOAT:
		text.SetFormatted("%lf", val_float);
		break;
	case TYPE_OBJECT:
		return val_obj ? val_obj->GetClassName() : "";
	case TYPE_STRING:
		return val_str;
	case TYPE_NULL:
		return "";
	}
	return text;
}

bool TBValue::Equals(long value) const
{
	if (m_packed.type == TYPE_INT)
		return val_int == value;
	if (m_packed.type == TYPE_FLOAT)
		return val_float == value;
	if (m_packed.type == TYPE_STRING)
		return GetInt() == value;
	return false;
}

bool TBValue::Equals(double value) const
{
	if (m_packed.type == TYPE_INT)
		return val_int == value;
	if (m_packed.type == TYPE_FLOAT)
		return val_float == value;
	if (m_packed.type == TYPE_STRING)
		return GetFloat() == value;
	return false;
}

bool TBValue::Equals(const char * value) const
{
	errno = 0;
	char *endptr = nullptr;
	if (m_packed.type == TYPE_INT)
		return val_int == strtol(value, &endptr, 10) && !errno && value != endptr;
	if (m_packed.type == TYPE_FLOAT)
		return val_float == strtod(value, &endptr) && !errno && value != endptr;
	if (m_packed.type == TYPE_STRING)
		return !strcmp(val_str, value);
	return false;
}

bool TBValue::Equals(const TBValue & value) const
{
	switch (m_packed.type) {
	case TYPE_NULL:		return value.m_packed.type == TYPE_NULL;
	case TYPE_STRING:	return value.m_packed.type == TYPE_STRING && !strcmp(val_str, value.val_str);
	case TYPE_FLOAT:	return Equals(value.GetFloat());
	case TYPE_INT:		return Equals(value.GetFloat());
	case TYPE_OBJECT://	return value.m_packed.type == TYPE_OBJECT && val_obj == value.val_obj;
	case TYPE_ARRAY:
	default:
		return false;
	}
}


} // namespace tb
