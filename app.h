#pragma once
#include "himage.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include "tool.h"
#include <iostream>
#include <sstream>
#include <vector>

class App
{
public:
	App();
	void setup();
	void run();
	void quit();

private:
	bool running = false;
	SDL_Window* win;
	SDL_Renderer* renderer;
	
	void draw();
	void update();

    SDL_Texture* get_texture_from_himage(HImage* himage);
    SDL_Texture* himage_texture;

    void input_digit(int n);
    int input_number = -1;
    void fin_input();
    void cancel_input();
    void del_input();


    void load_image();
    void save_image();

    int window_w = 1000;
    int window_h = 1000;

    int number_offset = 4;

    void reset_camera();

    Tool tool;

	HImage* working_image; 
    std::vector<HImage*> assets;
    std::vector<SDL_Texture*> assets_textures;

};
