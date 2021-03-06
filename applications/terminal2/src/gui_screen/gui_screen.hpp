/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2015, Max Schlüssel <lokoxe@gmail.com>                     *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "screen.hpp"

#include <map>
#include <list>
#include <cairo/cairo.h>

#include <ghostuser/ui/window.hpp>
#include <ghostuser/ui/canvas.hpp>
#include <ghostuser/ui/ui.hpp>
#include <ghostuser/ui/label.hpp>
#include <ghostuser/ui/button.hpp>
#include <ghostuser/ui/textfield.hpp>
#include <ghostuser/ui/action_listener.hpp>
#include <ghostuser/ui/focus_listener.hpp>
#include <ghostuser/ui/key_listener.hpp>
#include <ghostuser/utils/utils.hpp>
#include <ghostuser/tasking/lock.hpp>
#include <ghostuser/graphics/text/font_loader.hpp>
#include <ghostuser/graphics/text/font.hpp>
#include <ghostuser/graphics/text/text_layouter.hpp>

/**
 * Used to cache information about the layout of a specific character.
 */
struct char_layout_t {
	cairo_glyph_t* glyph_buffer = nullptr;
	int glyph_count;
	cairo_text_cluster_t* cluster_buffer = nullptr;
	int cluster_count;
};

/**
 *
 */
class gui_screen_t: public screen_t {
private:
	/**
	 * UI related properties
	 */
	g_window* window;
	g_canvas* canvas;

	g_font* font;

	cairo_surface_t* existingSurface = 0;
	uint8_t* existingSurfaceBuffer = 0;
	g_dimension bufferSize;
	cairo_t* existingContext = 0;

	uint8_t paint_uptodate = false;
	bool cursorBlink = false;

	std::list<g_key_info> input_buffer;
	uint8_t input_buffer_empty = true;
	g_lock input_buffer_lock;

	bool focused = false;
	uint64_t last_input_time = 0;

	/**
	 * Screen buffer
	 */
	uint8_t* raster_buffer = 0;
	g_dimension raster_size;
	g_lock raster_lock;
	int cursor_x = 0;
	int cursor_y = 0;

	int char_width = 8;
	int char_height = 12;

	std::map<char, char_layout_t*> char_layout_cache;

	/**
	 * Prepares the canvas buffer for painting.
	 *
	 * @return the cairo instance to paint with
	 */
	cairo_t* getGraphics();

	/**
	 * Thread that is responsible for making the cursor
	 * blink in specific intervals.
	 *
	 * @param screen
	 * 		instance of the screen
	 */
	static void blink_cursor_entry(gui_screen_t* screen);
	void blink_cursor();

	/**
	 *
	 */
	static void paint_entry(gui_screen_t* screen);
	void paint();

	/**
	 *
	 */
	char_layout_t* get_char_layout(cairo_scaled_font_t* scaled_face, char c);

public:
	/**
	 * Initializes the UI components for the screen.
	 *
	 * @return whether initialization was successful
	 */
	bool initialize();

	g_key_info readInput();
	void clean();
	void writeChar(char c);
	void moveCursor(int x, int y);
	int getCursorX();
	int getCursorY();
	void backspace();

	void buffer_input(const g_key_info& info);
	void repaint();
	void set_focused(bool focused);

	void update_visible_buffer_size();
	virtual int getWidth();
	virtual int getHeight();

	void setScrollAreaScreen();
	void setScrollArea(int start, int end);
	void scroll(int value);

	void setCursorVisible(bool visible);
};
