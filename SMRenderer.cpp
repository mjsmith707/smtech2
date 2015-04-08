//
//  SMRenderer.cpp
//  smtech1
//
//  Created by Matt on 4/2/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include "SMRenderer.h"
using namespace smtech1;

struct SMRenderer::SMVector {
    double x;
    double y;
    double z;
} SMVector;

SMRenderer::SMRenderer(Uint32 width, Uint32 height) : renderThread(), renderRunning(false), width(width), height(height) {

}

SMRenderer::~SMRenderer() {
    renderRunning = false;
    if (renderThread.joinable()) {
        renderThread.join();
    }
}

void SMRenderer::run() {
    renderRunning = true;
    renderThread = std::thread(&SMRenderer::threadinit, this);
}

void SMRenderer::threadinit() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "SDL Init Failed" << std::endl;
        return;
    }
    else {
        std::cout << "SDL Init Successful" << std::endl;
    }

    window = NULL;
    screen = NULL;

    window = SDL_CreateWindow("smtech1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cout << "Window not created: " << SDL_GetError() << std::endl;
        return;
    }
    else {
        screen = SDL_GetWindowSurface(window);
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0x0, 0x0, 0x0));
        SDL_UpdateWindowSurface(window);
        SDL_Delay(2000);
    }

    render();
}

void SMRenderer::render() {
    const int r_fps = 60;
    const int skip_ticks = 1000/r_fps;
    long next_tick = std::chrono::time_point_cast<lsec>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

    SMVector vecta { 50, 50, 0 };
    SMVector vectb { 200, 200, 0 };
    SMVector vectc { 300, 20, 0 };
    SMVector vectd { 500, 400, 0 };

    while (renderRunning) {
        long currtick = std::chrono::time_point_cast<lsec>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
        while (currtick > next_tick) {
            next_tick += skip_ticks;
            std::this_thread::sleep_for(std::chrono::milliseconds((skip_ticks-currtick) > 0 ? skip_ticks-currtick : 1));
        }

        //SDL_LockSurface(screen);

        drawLine(vecta, vectb, 0x00CC00);
        drawLine(vectc, vectd, 0xFFFF66);

        //SDL_UnlockSurface(screen);
        SDL_UpdateWindowSurface(window);
    }
}

void SMRenderer::drawLine(SMVector& a, SMVector& b, Uint32 color) {
    int dx = b.x - a.x;
    int dy = b.y - a.y;
    int D = 2*dy - dx;
    int y = b.y;
    for (int x=a.x+1; x<b.x; x++) {
        if (D > 0) {
            y = y+1;
            drawPixel(x, y, color);
            D = D + (2*dy-2*dx);
        }
        else {
            drawPixel(x, y, color);
            D = D + (2*dy);
        }
    }
}

void SMRenderer::drawPixel(int x, int y, Uint32 pixel) {
    // http://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
    int bpp = screen->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    switch(bpp) {
        case 1:
            *p = pixel;
            break;
        case 2:
            *(Uint16 *)p = pixel;
            break;
        case 3:
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;
        case 4:
            *(Uint32*)p = pixel;
            break;
    }
}

double SMRenderer::dotProduct(const SMVector& vecta, const SMVector& vectb) {
    double dot = vecta.x * vectb.x;
    dot += vecta.y * vectb.y;
    dot += vecta.z * vectb.z;
    return dot;
}

SMRenderer::SMVector SMRenderer::crossProduct(const SMVector& vecta, const SMVector& vectb) {
    double x = vecta.y*vectb.z - vecta.z*vectb.y;
    double y = vecta.z*vectb.x - vecta.x*vectb.z;
    double z = vecta.x*vectb.y - vecta.y*vectb.x;

    SMVector cross { x, y, z };
    return cross;
}
