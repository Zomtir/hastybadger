// ================================================================================
// ==      This file is a part of Turbo Badger. (C) 2011-2014, Emil Segerås      ==
// ==                     See tb_core.h for more information.                    ==
// ================================================================================

#ifdef TB_CLIPBOARD_GLFW

#include "tb_system.h"

#include "GLFW/glfw3.h"

namespace tb {

// == TBClipboard =====================================

void TBClipboard::Empty()
{
	SetText("");
}

bool TBClipboard::HasText()
{
	if (GLFWwindow *window = glfwGetCurrentContext())
	{
		const char *str = glfwGetClipboardString(window);
		if (str && *str)
			return true;
	}
	return false;
}

bool TBClipboard::SetText(const TBStr & text)
{
	if (GLFWwindow *window = glfwGetCurrentContext())
	{
		glfwSetClipboardString(window, (const char *)text);
		return true;
	}
	return false;
}

bool TBClipboard::GetText(TBStr &text)
{
	if (GLFWwindow *window = glfwGetCurrentContext())
	{
		if (const char *str = glfwGetClipboardString(window))
			return text.Set(str);
	}
	return false;
}

} // namespace tb

#endif // TB_CLIPBOARD_GLFW
