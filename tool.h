#pragma once

#include "himage.h"
#include <queue>
#include <utility>
#include <cmath>
#include <SDL2/SDL.h>

class Tool
{
public:
    Tool();
    Tool(HImage* _working_image, SDL_Window* _win);
    
    void click(SDL_Event event);
    void drag(SDL_Event event);

    void set_tool_id(int id);
    void set_color(int color);
    void set_tool_size(int size);
    void swap_color();
    int get_tool_id();
    int get_current_color();
    int get_other_color();
    int get_tool_size();

    void update_working_image(HImage* _working_image);

	float display_cx;
	float display_cy;
	float display_w;
	float display_h;

    void reset_camera();

private:
    SDL_Window* win;

    int tool_id = 0;

    int current_color = 0;
    int other_color = 15;
    int tool_size = 0;

    bool last_valid = false;
    int last_x;
    int last_y;

    bool line_other_set = false;
    int line_x;
    int line_y;


    void line(int x1, int y1, int x2, int y2);
    void line_s(int x1, int y1, int x2, int y2, bool steps);
    void pencil_click(int x, int y);
    void bucket_click(int x, int y);
    void line_click(int x, int y);
    void point(int x, int y);
    void pencil_drag(int x, int y);
    void zoom(int x, int y, float zoom_scale);

    HImage* working_image;
};
