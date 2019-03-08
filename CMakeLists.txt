cmake_minimum_required(VERSION 3.4.3)
cmake_policy(SET CMP0048 NEW)
project(HastyBadger LANGUAGES CXX VERSION 2.0)

include(CheckCXXCompilerFlag)
set(CMAKE_CXX_STANDARD 14)
#add_definitions(
#    -Wfatal-errors
#)
#set(CMAKE_CXX_FLAGS "-W1 -Wfatal-errors" CACHE INTERNAL "" FORCE)

option(TB_FONT_RENDERER_FREETYPE "Enable FreeType Font. Requires FreeType" OFF)
option(TB_FONT_RENDERER_FREETYPEX "Enable FreeTypeX Font. Requires FreeType?" OFF)
option(TB_FONT_RENDERER_STB "Enable to support truetype fonts using stb_truetype.h WARNING VERY UNSAFE" OFF)
option(TB_FONT_RENDERER_TBBF "Enable to support TBBF fonts (Turbo Badger Bitmap Fonts)" ON)

# This is not safe to turn off at the moment
option(TB_IMAGE "Enable support for TBImage, TBImageManager, TBImageWidget." ON)

# This is not safe to turn off at the moment
option (TB_IMAGE_LOADER_STB "Enable to support image loading using stb_image.c" ON)

option(TB_SYSTEM_ANDROID "Use Android system calls" OFF)
option(TB_SYSTEM_LINUX "Use Linux system calls" OFF)
option(TB_SYSTEM_SDL2 "Use SDL2 system calls" OFF)
option(TB_SYSTEM_WINDOWS "Use Windows system calls" ON)

option(TB_CLIPBOARD_DUMMY "Use a dummy clipboard" OFF)
option(TB_CLIPBOARD_GLFW "Use the GLFW clipboard" OFF)
option(TB_CLIPBOARD_SDL2 "Use the SDL2 clipboard" OFF)
option(TB_CLIPBOARD_WINDOWS "Use the Windows clipboard" ON)

option(TB_RUNTIME_DEBUG_INFO "Print more debug info during runtime" OFF)

# SOURCE FILES
add_library(hblib SHARED
	src/tb_animation.cpp
	src/tb_bitmap_fragment.cpp
	src/tb_clipboard_dummy.cpp
	src/tb_clipboard_glfw.cpp
	src/tb_clipboard_sdl2.cpp
	src/tb_system_win.cpp
	src/tb_color.cpp
	src/tb_core.cpp
	src/tb_debug.cpp
	src/tb_dimension.cpp
	src/tb_editfield.cpp
	src/tb_file_android.cpp
	src/tb_file_posix.cpp
	src/tb_font_renderer.cpp
	src/tb_font_renderer_freetype.cpp
	src/tb_font_renderer_freetypeX.cpp
	src/tb_font_renderer_stb.cpp
	src/tb_font_renderer_tbbf.cpp
	src/tb_geometry.cpp
	src/tb_hash.cpp
	src/tb_hashtable.cpp
	src/tb_id.cpp
	src/tb_image_loader_stb.cpp
	src/tb_image_manager.cpp
	src/tb_image_widget.cpp
	src/tb_inline_select.cpp
	src/tb_language.cpp
	src/tb_layout.cpp
	src/tb_linklist.cpp
	src/tb_list.cpp
	src/tb_menu_window.cpp
	src/tb_message_window.cpp
	src/tb_msg.cpp
	src/tb_node_ref_tree.cpp
	src/tb_node_tree.cpp
	src/tb_object.cpp
	src/tb_parser.cpp
	src/tb_popup_window.cpp
	src/tb_renderer.cpp
	src/tb_renderer_batcher.cpp
	src/tb_renderer_gl.cpp
	src/tb_scroll_container.cpp
	src/tb_scroller.cpp
	src/tb_select.cpp
	src/tb_select_item.cpp
	src/tb_skin.cpp
	src/tb_skin_util.cpp
	src/tb_str.cpp
	src/tb_style_edit.cpp
	src/tb_style_edit_content.cpp
	src/tb_system_android.cpp
	src/tb_system_linux.cpp
	src/tb_system_sdl.cpp
	src/tb_system_win.cpp
	src/tb_tab_container.cpp
	src/tb_tempbuffer.cpp
	src/tb_toggle_container.cpp
	src/tb_value.cpp
	src/tb_widget_animation.cpp
	src/tb_widget_skin_condition_context.cpp
	src/tb_widget_value.cpp
	src/tb_widgets.cpp
	src/tb_widgets_common.cpp
	src/tb_widgets_listener.cpp
	src/tb_widgets_reader.cpp
	src/tb_window.cpp
	src/thirdparty/utf8.cpp
)

# FONT RENDERER
if(TB_FONT_RENDERER_FREETYPE)
	add_compile_definitions(TB_FONT_RENDERER_FREETYPE=ON)
endif()
if(TB_FONT_RENDERER_FREETYPEX)
  	add_compile_definitions(TB_FONT_RENDERER_FREETYPEX=ON)
endif()
if(TB_FONT_RENDERER_STB)
  	add_compile_definitions(TB_FONT_RENDERER_STB=ON)
endif()
if(TB_FONT_RENDERER_TBBF)
  	add_compile_definitions(TB_FONT_RENDERER_TBBF=ON)
endif()

if(TB_IMAGE_LOADER_STB)
 	add_compile_definitions(TB_IMAGE_LOADER_STB=ON)
endif()

add_compile_definitions(
	TB_GLYPH_CACHE_WIDTH=512
	TB_GLYPH_CACHE_HEIGHT=512
)

# SYSTEM
if(TB_SYSTEM_ANDROID)
  	add_compile_definitions(TB_SYSTEM_ANDROID=ON)
  	add_compile_definitions(TB_FILE_ANDROID=ON)
endif()
if(TB_SYSTEM_LINUX)
  	add_compile_definitions(TB_SYSTEM_LINUX=ON)
  	add_compile_definitions(TB_FILE_POSIX=ON)
endif()
if(TB_SYSTEM_SDL2)
  	add_compile_definitions(TB_SYSTEM_SDL2=ON)
  	add_compile_definitions(TB_FILE_POSIX=ON)
endif()
if(TB_SYSTEM_WINDOWS)
  	add_compile_definitions(TB_SYSTEM_WINDOWS=ON)
  	add_compile_definitions(TB_FILE_POSIX=ON)
  	target_link_libraries(hblib winmm.lib)
endif()

# CLIPBOARD
if(TB_CLIPBOARD_DUMMY)
  	add_compile_definitions(TB_CLIPBOARD_DUMMY=ON)
endif()
if(TB_CLIPBOARD_GLFW)
  	add_compile_definitions(TB_CLIPBOARD_GLFW=ON)
endif()
if(TB_CLIPBOARD_SDL2)
  	add_compile_definitions(TB_CLIPBOARD_SDL2=ON)
endif()
if(TB_CLIPBOARD_WINDOWS)
  	add_compile_definitions(TB_CLIPBOARD_WINDOWS=ON)
endif()

# DEBUG
if(TB_RUNTIME_DEBUG_INFO)
  	add_compile_definitions(TB_RUNTIME_DEBUG_INFO=ON)
endif()

set_target_properties(hblib PROPERTIES VERSION ${PROJECT_VERSION})

set(CMAKE_INSTALL_LIBDIR "Build/lib")
set(CMAKE_INSTALL_INCLUDEDIR "Build/include")

install(
	TARGETS hblib
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION src
    PUBLIC_HEADER DESTINATION include
)
