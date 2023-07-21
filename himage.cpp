#include "himage.h"
#include <iostream>
#include <fstream>
#include <math.h>
#include <cstring>

HImage::HImage(int w, int h)
{
	width = w;
	height = h;

	image_data_size = std::ceil(1.0 * width * height / 2);
	image_data = new unsigned char[image_data_size];
	display_data = new unsigned char[width * height * 3];
    working_data = new unsigned char[width * height];
	memset(image_data, 0, image_data_size);
	memset(display_data, 0, width * height * 3);
    memset(working_data, 0, width * height);

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			set_pixel_index(i + j * width, 15);
		}
	}
}

int HImage::get_width() {return width;}
int HImage::get_height() {return height;}
unsigned char* HImage::get_display_data() {return display_data;}

HImage* HImage::load_image(std::string image_path)
{
	HImage* loaded_image = nullptr;

	std::ifstream myfile;
	myfile.open(image_path);
	if (myfile.is_open())
	{
		int byte_index = 0;
		int structure_index = 0;
		unsigned char mychar;

		int new_width = 0;
		int new_height = 0;
		int new_image_data_size = 0;
		while (myfile)
		{
			mychar = myfile.get();
			switch(structure_index)
			{
				case 0:
					// HI
					if (mychar != "HI"[byte_index])
					{
						// HI identifier not found
						return nullptr;
					}
					break;
				case 1:
					// Image data size
					new_image_data_size += (1 << (byte_index*8)) * (int)mychar;
					break;
				case 2:
					// Width
					new_width += (1 << (byte_index*8)) * (int)mychar;
					break;
				case 3:
					// Height
					new_height += (1 << (byte_index*8)) * (int)mychar;
					break;
				default:
					// Image data
					if (byte_index == 0)
					{
						loaded_image = new HImage(new_width, new_height);
					}
                    if (byte_index >= new_image_data_size)
                    {
                        break;
                    }
					loaded_image->image_data[byte_index] = mychar;
					break;
			}
			
			byte_index++;
			if (structure_index >= (int)(sizeof(structure_sizes)/sizeof(int)))
			{
				continue;
			}
			if (byte_index >= structure_sizes[structure_index])
			{
				byte_index = 0;
				structure_index++;
			}
		}
	}
    if (loaded_image)
    {
        loaded_image->remake_display_data();
    }
	return loaded_image;
}

void HImage::on_operation()
{
    if (undo_level != -1)
    {
        for (int i = 0; i <= undo_level; i++)
        {
            undo_data.pop_back();
        }
        undo_level = -1;
    }

    unsigned char* undo1 = new unsigned char[image_data_size];
    memcpy(undo1, image_data, image_data_size);
    undo_data.push_back(undo1);
    if (undo_data.size() > 20)
    {
        undo_data.erase(undo_data.begin());
    }
}

void HImage::undo()
{
    if (undo_level >= (int)undo_data.size()-1)
    {
        return;
    }
    if (undo_level == -1)
    {
        on_operation();
        undo_level++;
    }
    undo_level++;
    image_data = undo_data.at(undo_data.size()-1-undo_level);
    remake_display_data();
}

void HImage::redo()
{
    if (undo_level <= 0)
    {
        return;
    }
    undo_level--;
    image_data = undo_data.at(undo_data.size()-1-undo_level);
    remake_display_data();
}

bool HImage::valid_index(int index)
{
    return (index >= 0 && index < width * height);
}

bool HImage::valid_coords(int x, int y)
{
    return (x >= 0 && x < width && y >= 0 && y < height);
}

files::Himage_file* HImage::get_save_file()
{
	// 2 BYTES HI
	// 4 BYTES Image data size
	// 4 BYTES Width
	// 4 BYTES Height
	// IMAGE DATA
	unsigned char* file_data = (unsigned char*)malloc(2+4+4+4+image_data_size);
	memset(file_data, 0, 2+4+4+4+image_data_size);
	
	// HI
	unsigned char temp[4] = "HI";
	memcpy(file_data, temp, 2);
	
	// Image data size
	unsigned char* temp2 = files::int_to_bytes_little(image_data_size, 4);
	memcpy(file_data+2, temp2, 4);
	free(temp2);
	
	// Width
	temp2 = files::int_to_bytes_little(width, 4);
	memcpy(file_data+6, temp2, 4);
	free(temp2);

	// Height
	temp2 = files::int_to_bytes_little(height, 4);
	memcpy(file_data+10, temp2, 4);
	free(temp2);
	
	// Image data
	memcpy(file_data+14, image_data, std::ceil(1.0*width*height/2)); 

	
	files::Himage_file* ret = new files::Himage_file();
	ret->file_data = file_data;
	ret->file_size = 2+4+4+4+image_data_size;
	return ret;
}

void HImage::set_pixel(int x, int y, int color)
{
    if (!valid_coords(x, y))
    {
        return;
    }
    set_pixel_index(x + y * width, color);
}

void HImage::set_pixel_index(int index, int color)
{
    if (!valid_index(index))
    {
        return;
    }
	int color_data = get_color(color);
	memcpy(display_data+(index*3), &color_data, 3);
	int mem_byte = std::floor((float)index/2);
	int other_byte_half = 1-(index%2);
	unsigned char current_byte = image_data[mem_byte];
	int mask = 15<<(other_byte_half*4);
	current_byte = current_byte & mask;
	current_byte = current_byte | (color << ((index%2)*4));
	memcpy(image_data+mem_byte, &current_byte, 1);
    working_data[index] = color;
    data_changed = true;
}

void HImage::remake_display_data()
{
	for (int i = 0; i < width*height; i++)
	{
		int byte_half = i%2;
		int mask = 15 << (byte_half*4);
		int byte_index = std::floor(1.0*i/2);
		int color_index = (image_data[byte_index] & mask) >> (byte_half*4);
		int color_data = get_color(color_index);
		memcpy(display_data+(i*3), &color_data, 3);
        working_data[i] = color_index;
	}
    data_changed=true;
}

int HImage::get_color(int num)
{
	// if brown, 1/2, else 1
	float brown_correction = 1-(num == 6)*1.0/2;
	//char color_value[3];
	float r = (2.0/3*(num & 4)/4 + 1.0/3*(num & 8)/8)*255;
	float g = (2.0/3*(num & 2)/2*brown_correction + 1.0/3*(num & 8)/8)*255;
	float b = (2.0/3*(num & 1)/1 + 1.0/3*(num & 8)/8)*255;
	//return r*256*256+g*256+b;
	return r+g*256+b*256*256;
}

