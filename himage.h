#pragma once

#include "utils/files.h"
#include <vector>

class HImage
{
public:
	HImage(int w, int h);


	static HImage* load_image(std::string image_path);
	static inline int structure_sizes[4] = {2, 4, 4, 4};
	files::Himage_file* get_save_file();
    unsigned char* working_data;
	unsigned char* image_data;

	void remake_display_data();
    void set_pixel(int x, int y, int color);
	void set_pixel_index(int index, int color);

    bool valid_index(int index);
    bool valid_coords(int x, int y);

    int get_width();
    int get_height();

	static int get_color(int num);

    bool data_changed = true;

    unsigned char* get_display_data();

    void on_operation();
    void undo();
    void redo();
    int undo_level = -1;

private:
    int width;
    int height;

    std::vector<unsigned char*> undo_data;

	unsigned char* display_data;
	int image_data_size;
};
