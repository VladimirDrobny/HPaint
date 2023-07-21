#include "tool.h"

Tool::Tool()
{
}

Tool::Tool(HImage* _working_image, SDL_Window* _win)
{
    working_image = _working_image;
    win = _win;

    reset_camera();
}

void Tool::reset_camera()
{
    int window_w;
    int window_h;
    SDL_GetWindowSize(win, &window_w, &window_h);

    display_cx = window_w/2;
    display_cy = (window_h-100)/2;

    display_h = window_h-300;
    display_w = working_image->get_width()*display_h*1.0/working_image->get_height();
    if (display_w > window_w-200)
    {
        display_w = window_w-200;
        display_h = working_image->get_height()*display_w*1.0/working_image->get_width();
    }
}

void Tool::set_tool_id(int id)
{
    tool_id = id;
    line_other_set = false;
}

void Tool::set_tool_size(int size)
{
    tool_size = size;
}

int Tool::get_tool_size()
{
    return tool_size;
}

void Tool::set_color(int color)
{
    current_color = color;
}

int Tool::get_tool_id()
{
    return tool_id;
}

int Tool::get_current_color()
{
    return current_color;
}

int Tool::get_other_color()
{
    return other_color;
}

void Tool::swap_color()
{
    int temp_color = current_color;
    current_color = other_color;
    other_color = temp_color;
}

void Tool::update_working_image(HImage* _working_image)
{
    working_image = _working_image;
}

void Tool::click(SDL_Event event)
{
    int x = event.button.x;
    int y = event.button.y;
    int img_x = std::floor((x - display_cx + display_w/2)*1.0/display_w*working_image->get_width());
    int img_y = std::floor((y - display_cy + display_h/2)*1.0/display_h*working_image->get_height());

	if (tool_id == 0)
	{
        pencil_click(img_x, img_y);
	} else if (tool_id == 1)
    {
        bucket_click(img_x, img_y);
    } else if (tool_id == 2)
    {
        line_click(img_x, img_y);
    } else if (tool_id == 3)
    {
        float zoom_scale = 1.2;
        if (event.button.button == SDL_BUTTON_RIGHT)
        {
            zoom_scale = 1/zoom_scale;
        }
        zoom(x, y, zoom_scale);
    }

    last_x = img_x;
    last_y = img_y;
    last_valid = true;
}

void Tool::zoom(int x, int y, float zoom_scale)
{
    display_cx = zoom_scale*display_cx-zoom_scale*x+x;
    display_cy = zoom_scale*display_cy-zoom_scale*y+y;
    display_w *= zoom_scale;
    display_h *= zoom_scale;
}

void Tool::pencil_click(int x, int y)
{
    working_image->on_operation();
    if (tool_size <= 1)
    {
        working_image->set_pixel(x, y, current_color);
    } else
    {
        point(x, y);
    }
}

void Tool::point(int x, int y)
{
    int off = -(tool_size-1)/2;
    for (int i = 0; i < tool_size; i++)
    {
        for (int j = 0; j < tool_size; j++)
        {
            int fin_x = x+off+i;
            int fin_y = y+off+j;
            if (!working_image->valid_coords(fin_x, fin_y))
            {
                continue;
            }
            float nx = i-tool_size/2.0+0.5;
            float ny = j-tool_size/2.0+0.5;
            if (nx*nx+ny*ny <= tool_size*tool_size/4.0)
            {
                working_image->set_pixel(fin_x, fin_y, current_color);
            }
        }
    }
}

void Tool::pencil_drag(int x, int y)
{
    if (!last_valid)
    {
        pencil_click(x, y);
    } else {
        line(last_x, last_y, x, y);
    }
}

void Tool::line_click(int x, int y)
{
    if (!line_other_set)
    {
        line_x = x;
        line_y = y;
        line_other_set = true;
        return;
    }

    working_image->on_operation();

    line(line_x, line_y, x, y);

    line_other_set = false;
}

void Tool::line(int x1, int y1, int x2, int y2)
{
    if (tool_size == 0)
    {
        line_s(x1, y1, x2, y2, 0);
        return;
    } else if (tool_size == 1)
    {
        line_s(x1, y1, x2, y2, 1);
        return;
    }

    int xmin = x1;
    int xmax = x2;
    int ymin = y1;
    int ymax = y2;
    if (x1 > x2)
    {
        xmin = x2;
        xmax = x1;
    }
    if (y1 > y2)
    {
        ymin = y2;
        ymax = y1;
    }
    xmin-=(tool_size+1)/2;
    xmax+=(tool_size+1)/2;
    ymin-=(tool_size+1)/2;
    ymax+=(tool_size+1)/2;
    

    float bot = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
    if (bot != 0)
    {
        for (int i = xmin; i <= xmax; i++)
        {
            for (int j = ymin; j <= ymax; j++)
            {
                if (!working_image->valid_coords(i,j))
                {
                    continue;
                }
                float t = (float)((x2-x1)*(i-x1)+(y2-y1)*(j-y1))/bot;
                if (t < 0 || t > 1)
                {
                    continue;
                }
                float top = (x2-x1)*(y1-j)-(x1-i)*(y2-y1);
                float dist_squared = top*top/bot;
                if (dist_squared <= tool_size*tool_size/4.0)
                {
                    working_image->set_pixel(i, j, current_color);
                }
            }
        }
    }

    point(x1, y1);
    point(x2, y2);
}

void Tool::line_s(int x1, int y1, int x2, int y2, bool steps)
{
    int dx = x2 - x1;
    int dy = y2 - y1;
    int scale_x = (dx >= 0 ? 1 : -1);
    int scale_y = (dy >= 0 ? 1 : -1);

    bool dir_x = std::abs(dx) > std::abs(dy);
    int n = std::max(std::abs(dx), std::abs(dy));
    if (n == 0)
    {
		working_image->set_pixel(x1, y1, current_color);
        return;
    }
    int other_d = (dir_x ? dy : dx);

    int last_other = (dir_x ? y1 : x1);
    bool step = false;
    for (int i = 0; i <= n; i++)
    {
        int dir_pos = (dir_x ? x1 : y1) + i * (dir_x ? scale_x : scale_y);
        int other_pos = (dir_x ? y1 : x1) + std::floor(1.0*i*other_d/n+0.5);
        if (steps && other_pos != last_other)
        {
            step = true; 
        }

        int x = (dir_x ? dir_pos : other_pos);
        int y = (dir_x ? other_pos : dir_pos);

		working_image->set_pixel(x, y, current_color);

        if (step)
        {
            x = (dir_x ? dir_pos : last_other);
            y = (dir_x ? last_other : dir_pos);

            working_image->set_pixel(x, y, current_color);
            step = false;
        }

        last_other = other_pos;
    }
}

void Tool::bucket_click(int x, int y)
{
    if (!working_image->valid_coords(x, y))
    {
        return;
    }

    working_image->on_operation();

    int index = x + y * working_image->get_width();

    int color = working_image->working_data[index];
    if (color == current_color)
    {
        return;
    }
    std::queue<std::pair<int, int>> cells = std::queue<std::pair<int, int>>();
    cells.push(std::pair<int, int>(x, y));
    bool visited[working_image->get_width()*working_image->get_height()] = {0};
    
    while(!cells.empty())
    {
        std::pair<int,int> current_cell = cells.front();
        cells.pop();

        int current_index = current_cell.first + current_cell.second * working_image->get_width();

        if (visited[current_index])
        {
            continue;
        }

        if (!working_image->valid_coords(current_cell.first, current_cell.second))
        {
            continue;
        }

        visited[current_index] = true;

        if (working_image->working_data[current_index] == color)
        {
            working_image->set_pixel_index(current_index, current_color);
            cells.push(std::pair<int, int>(current_cell.first+1, current_cell.second));
            cells.push(std::pair<int, int>(current_cell.first, current_cell.second+1));
            cells.push(std::pair<int, int>(current_cell.first-1, current_cell.second));
            cells.push(std::pair<int, int>(current_cell.first, current_cell.second-1));
        }
    }
}

void Tool::drag(SDL_Event event)
{
    int x = event.button.x;
    int y = event.button.y;
    int img_x = std::floor((x - display_cx + display_w/2)*1.0/display_w*working_image->get_width());
    int img_y = std::floor((y - display_cy + display_h/2)*1.0/display_h*working_image->get_height());

	if (tool_id == 0)
	{
        pencil_drag(img_x, img_y);
	}

    last_x = img_x;
    last_y = img_y;
    last_valid = true;
}
