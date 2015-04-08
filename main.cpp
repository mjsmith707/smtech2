//
//  main.cpp
//  smtech1
//
//  Created by Matt on 4/1/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include "SMRenderer.h"

using namespace smtech1;

int main(int argc, const char * argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "SDL Init Failed" << std::endl;
        return -1;
    }
    else {
        std::cout << "SDL Init Successful" << std::endl;
    }

    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;

    window = SDL_CreateWindow("smtech1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cout << "Window not created: " << SDL_GetError() << std::endl;
        return -2;
    }
    else {
        screenSurface = SDL_GetWindowSurface(window);
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x0, 0x0, 0x0));
        SDL_UpdateWindowSurface(window);
        SDL_Delay(2000);
    }

    SMRenderer renderer(window, screenSurface, 640, 480);;
    renderer.render();


    std::string whatever;
    std::cin >> whatever;
    return 0;
}


