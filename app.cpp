#include "app.h"

App::App()
{
}

void App::setup()
{
    int img_w = -1;
    int img_h = -1;

    std::string inp;
    std::getline(std::cin, inp);
    std::istringstream inp2;
    inp2.str(inp);
    for (std::string inp3; std::getline(inp2, inp3, ' '); )
    {
        int inp = std::stoi(inp3);
        if (img_w == -1)
        {
            img_w = inp;
        } else
        {
            img_h = inp;
        }
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    win = SDL_CreateWindow("Paint",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       window_w, window_h, 0);

    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	// TODO: load image
	working_image = new HImage(img_w, img_h);

    tool = Tool(working_image, win);

    assets = std::vector<HImage*>();
    assets.push_back(HImage::load_image("utils/assets/pencil"));
    assets.push_back(HImage::load_image("utils/assets/bucket"));
    assets.push_back(HImage::load_image("utils/assets/line"));
    assets.push_back(HImage::load_image("utils/assets/zoom"));

    assets.push_back(HImage::load_image("utils/assets/zero"));
    assets.push_back(HImage::load_image("utils/assets/one"));
    assets.push_back(HImage::load_image("utils/assets/two"));
    assets.push_back(HImage::load_image("utils/assets/three"));
    assets.push_back(HImage::load_image("utils/assets/four"));
    assets.push_back(HImage::load_image("utils/assets/five"));
    assets.push_back(HImage::load_image("utils/assets/six"));
    assets.push_back(HImage::load_image("utils/assets/seven"));
    assets.push_back(HImage::load_image("utils/assets/eight"));
    assets.push_back(HImage::load_image("utils/assets/nine"));

    for (HImage* himage : assets)
    {
        assets_textures.push_back(get_texture_from_himage(himage));
    }

}

void App::update()
{
}

void App::run()
{
	running = true;
	while(running)
	{
        SDL_Event event;
        while (SDL_PollEvent(&event))
		{
            switch (event.type)
			{
            	case SDL_QUIT:
					running = false;
                	break;

            	case SDL_KEYDOWN:
                	switch (event.key.keysym.scancode) {
                		case SDL_SCANCODE_A:
							tool.set_color(0);
							break;
                		case SDL_SCANCODE_S:
							tool.set_color(1);
							break;
                		case SDL_SCANCODE_D:
							tool.set_color(2);
							break;
                		case SDL_SCANCODE_F:
							tool.set_color(3);
							break;
                		case SDL_SCANCODE_G:
							tool.set_color(4);
							break;
                		case SDL_SCANCODE_H:
							tool.set_color(5);
							break;
                		case SDL_SCANCODE_J:
							tool.set_color(6);
							break;
                		case SDL_SCANCODE_K:
							tool.set_color(7);
							break;
                		case SDL_SCANCODE_Q:
							tool.set_color(8);
							break;
                		case SDL_SCANCODE_W:
							tool.set_color(9);
							break;
                		case SDL_SCANCODE_E:
							tool.set_color(10);
							break;
                		case SDL_SCANCODE_R:
							tool.set_color(11);
							break;
                		case SDL_SCANCODE_T:
							tool.set_color(12);
							break;
                		case SDL_SCANCODE_Y:
							tool.set_color(13);
							break;
                		case SDL_SCANCODE_U:
							tool.set_color(14);
							break;
                		case SDL_SCANCODE_I:
							tool.set_color(15);
							break;
                		case SDL_SCANCODE_0:
                            input_digit(0);
							break;
                		case SDL_SCANCODE_1:
                            input_digit(1);
							break;
                		case SDL_SCANCODE_2:
                            input_digit(2);
							break;
                		case SDL_SCANCODE_3:
                            input_digit(3);
							break;
                		case SDL_SCANCODE_4:
                            input_digit(4);
							break;
                		case SDL_SCANCODE_5:
                            input_digit(5);
							break;
                		case SDL_SCANCODE_6:
                            input_digit(6);
							break;
                		case SDL_SCANCODE_7:
                            input_digit(7);
							break;
                		case SDL_SCANCODE_8:
                            input_digit(8);
							break;
                		case SDL_SCANCODE_9:
                            input_digit(9);
							break;
                        case SDL_SCANCODE_RETURN:
                            fin_input();
                            break;
                        case SDL_SCANCODE_BACKSPACE:
                            del_input();
                            break;
                        case SDL_SCANCODE_ESCAPE:
                            cancel_input();
                            break;
                        case SDL_SCANCODE_P:
                            tool.set_tool_id(0);
                            break;
                        case SDL_SCANCODE_B:
                            tool.set_tool_id(1);
                            break;
                        case SDL_SCANCODE_L:
                            tool.set_tool_id(2);
                            break;
                        case SDL_SCANCODE_Z:
                            tool.set_tool_id(3);
                            break;
                		case SDL_SCANCODE_X:
                            tool.swap_color();
							break;
                        case SDL_SCANCODE_C:
                            load_image();
                            break;
                        case SDL_SCANCODE_V:
                            save_image();
                            break;
                        case SDL_SCANCODE_SPACE:
                            if (event.key.keysym.mod == KMOD_NONE)
                            {
                                working_image->undo();
                            } else if (event.key.keysym.mod & KMOD_LSHIFT)
                            {
                                working_image->redo();
                            }
                            break;
                		default:
                		    break;
                	}
					break;
				case SDL_MOUSEMOTION:
                    if (event.motion.state & SDL_BUTTON_LMASK)
                    {
                        tool.drag(event);
                    }
					break;
				case SDL_MOUSEBUTTONDOWN:
                    tool.click(event);
					break;
				case SDL_MOUSEBUTTONUP:
					switch (event.button.button)
					{
						case SDL_BUTTON_LEFT:
							break;
					}
					break;
            }
        }

		update();
		draw();

        SDL_Delay(1000 / 60);
	}
}

void App::input_digit(int n)
{
    if (input_number == -1)
    {
        input_number = n;
        return;
    }
    if (input_number > (2<<20))
    {
        return;
    }

    input_number *= 10;
    input_number += n;
}

void App::del_input()
{
    if (input_number == -1)
    {
        return;
    }
    if (input_number < 10)
    {
        cancel_input();
        return;
    }
    input_number /= 10;
}

void App::fin_input()
{
    if (input_number == -1)
    {
        return;
    }
    tool.set_tool_size(input_number);
    input_number = -1;
}

void App::cancel_input()
{
    input_number = -1;
}


void App::load_image()
{
    std::cout << "Load name: ";
    std::string input;
    std::getline(std::cin, input);
    if (input == "")
    {
        return;
    }
    HImage* new_image = HImage::load_image("savedata/"+input); 
    if (new_image)
    {
        delete working_image;
        working_image = new_image;
        tool.update_working_image(working_image);
        std::cout << "Image loaded." << std::endl;
    }

    tool.reset_camera();
}

void App::save_image()
{
    std::cout << "Save name: ";
    std::string input;
    std::getline(std::cin, input);
    if (input == "")
    {
        return;
    }

    files::Himage_file* saving_file = working_image->get_save_file();
    files::save_himage(saving_file, "savedata/"+input);

    std::cout << "Image saved." << std::endl;
}

void App::quit()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    delete working_image;
}

void App::draw()
{
	SDL_SetRenderDrawColor(renderer, 70, 70, 70, 255);
	SDL_RenderClear(renderer);


    if (working_image->data_changed)
    {
        if (himage_texture)
        {
            SDL_DestroyTexture(himage_texture);
        }
        himage_texture = get_texture_from_himage(working_image);
        working_image->data_changed = false;
    }

	SDL_Rect* drwrect = new SDL_Rect();
	drwrect->x = (int)(tool.display_cx-tool.display_w/2);
	drwrect->y = (int)(tool.display_cy-tool.display_h/2);
	drwrect->w = (int)tool.display_w;
	drwrect->h = (int)tool.display_h;

	SDL_RenderCopy(renderer, himage_texture, NULL, drwrect);


    drwrect->x = 0;
    drwrect->y = window_h-100;
    drwrect->w = window_w;
    drwrect->h = 100;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, drwrect);

	drwrect->x = 10;
	drwrect->y = window_h-90;
	drwrect->w = 80;
	drwrect->h = 80;
	SDL_SetRenderDrawColor(renderer, 100, 100, 0, 255);
    //SDL_RenderFillRect(renderer, drwrect);
	SDL_RenderCopy(renderer, assets_textures[tool.get_tool_id()], NULL, drwrect);
    
	drwrect->x = 180;
	drwrect->y = window_h-50;
	drwrect->w = 40;
	drwrect->h = 40;
    int tool_color = HImage::get_color(tool.get_other_color());
    if (tool.get_other_color() == 0)
    {
	    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    } else {
	    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
    SDL_RenderFillRect(renderer, drwrect);

	drwrect->x = 185;
	drwrect->y = window_h-45;
	drwrect->w = 30;
	drwrect->h = 30;
    tool_color = HImage::get_color(tool.get_other_color());
	SDL_SetRenderDrawColor(renderer, tool_color & 255, (tool_color & (255 << 8)) >> 8, (tool_color & (255 << 16)) >> 16, 255);
    SDL_RenderFillRect(renderer, drwrect);

	drwrect->x = 100;
	drwrect->y = window_h-90;
	drwrect->w = 80;
	drwrect->h = 80;
    tool_color = HImage::get_color(tool.get_current_color());
    if (tool.get_current_color() == 0)
    {
	    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    } else {
	    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    }
    SDL_RenderFillRect(renderer, drwrect);

	drwrect->x = 105;
	drwrect->y = window_h-85;
	drwrect->w = 70;
	drwrect->h = 70;
    tool_color = HImage::get_color(tool.get_current_color());
	SDL_SetRenderDrawColor(renderer, tool_color & 255, (tool_color & (255 << 8)) >> 8, (tool_color & (255 << 16)) >> 16, 255);
    SDL_RenderFillRect(renderer, drwrect);

    for (int i = 0; i < 16; i++)
    {
        int x = i%8;
        int y = i/8;
        drwrect->x = 230+x*40;
        drwrect->y = window_h-40*y-50;
        drwrect->w = 40;
        drwrect->h = 40;
        int color = HImage::get_color(i);
        SDL_SetRenderDrawColor(renderer, color & 255, (color & (255 << 8)) >> 8, (color & (255 << 16)) >> 16, 255);
        SDL_RenderFillRect(renderer, drwrect);

        if (i == tool.get_current_color())
        {
            drwrect->x = 230+x*40+17;
            drwrect->y = window_h-40*y-50+17;
            drwrect->w = 6;
            drwrect->h = 6;
            if (y == 1)
            {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            } else
            {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            }
            SDL_RenderFillRect(renderer, drwrect);
        }
    }

    if (tool.get_tool_size() != -1)
    {
        drwrect->x = window_w-50;
        drwrect->y = window_h-50;
        drwrect->w = 40;
        drwrect->h = 40;

        if (tool.get_tool_size() == 0)
        {
            SDL_RenderCopy(renderer, assets_textures[number_offset], NULL, drwrect);
        } else
        {
            int temp = tool.get_tool_size();
            int digit = -1;
            while (temp)
            {
                digit = temp%10;
                temp /= 10;

                SDL_RenderCopy(renderer, assets_textures[number_offset+digit], NULL, drwrect);

                drwrect->x -= drwrect->w;
            }
        }
    }

    if (input_number != -1)
    {
        drwrect->x = window_w-50;
        drwrect->y = window_h-90;
        drwrect->w = 40;
        drwrect->h = 40;

        if (input_number == 0)
        {
            SDL_RenderCopy(renderer, assets_textures[number_offset], NULL, drwrect);
        } else
        {
            int temp = input_number;
            int digit = -1;
            while (temp)
            {
                digit = temp%10;
                temp /= 10;

                SDL_RenderCopy(renderer, assets_textures[number_offset+digit], NULL, drwrect);

                drwrect->x -= drwrect->w;
            }
        }
    }


    

	// Present
	SDL_RenderPresent(renderer);

    delete drwrect;
}

SDL_Texture* App::get_texture_from_himage(HImage* himage)
{
	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void*)himage->get_display_data(),
                himage->get_width(),
                himage->get_height(),
                24,          // bits per pixel = 8*3 = 24
                himage->get_width() * 3,  // pitch
                0x0000FF,              // red mask
                0x00FF00,              // green mask
                0xFF0000,              // blue mask
                0);                    // alpha mask (none)

	return SDL_CreateTextureFromSurface(renderer, surface);
}
