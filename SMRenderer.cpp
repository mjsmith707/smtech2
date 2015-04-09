//
//  SMRenderer.cpp
//  smtech1
//
//  Created by Matt on 4/2/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include "SMRenderer.h"
using namespace smtech1;



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
    //const int r_fps = 60;
    //const int framerate = 1000/r_fps;
    //long currtime = std::chrono::time_point_cast<lsec>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
    SDL_Event event;

    SMVector vecta { 0, 0, 200, 0 };
    SMVector vectb { 0, 200, 200, 0 };
    SMVector vectc { 200, 200, 200, 0 };
    SMVector vectd { 200, 0, 200, 0 };

    while (renderRunning) {
        /*long newtime = std::chrono::time_point_cast<lsec>(std::chrono::high_resolution_clock::now()).time_since_epoch().count() - currtime;
        while (newtime > framerate) {
            currtime += newtime;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        } */

        //SDL_LockSurface(screen);
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0x0, 0x0, 0x0));
        if (SDL_UpdateWindowSurface(window) != 0) {
            std::cout << "UpdateWindowSurface Failed: " << SDL_GetError() << std::endl;
        }

        SMVector tvecta = project(vecta);
        SMVector tvectb = project(vectb);
        SMVector tvectc = project(vectc);
        SMVector tvectd = project(vectd);

        drawLine(tvecta, tvectb, 0x0000FF);
        drawLine(tvectb, tvectc, 0xCC0000);
        drawLine(tvectc, tvectd, 0xFFFF00);
        drawLine(tvectd, tvecta, 0x33CC33);

        //SDL_UnlockSurface(screen);
        if (SDL_UpdateWindowSurface(window) != 0) {
            std::cout << "UpdateWindowSurface Failed: " << SDL_GetError() << std::endl;
        }

        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                    std::cout << "keydown" << std::endl;
                    switch (event.key.keysym.sym) {
                        case SDLK_w:
                            camera.x += cos(angle);
                            camera.y -= sin(angle);
                            break;
                        case SDLK_a:
                            camera.x += sin(angle);
                            camera.y -= cos(angle);
                            break;
                        case SDLK_s:
                            camera.x -= cos(angle);
                            camera.y += cos(angle);
                            break;
                        case SDLK_d:
                            camera.x -= sin(angle);
                            camera.y += cos(angle);
                            break;
                        case SDLK_q:
                            angle += 0.1;
                            break;
                        case SDLK_e:
                            angle -= 0.1;
                            break;
                    }
                    std::cout << "camera: <" << camera.x << ", " << camera.y << ", " << camera.z << "> angle: " << angle << std::endl;
            }
        }

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
    // Debugging bounds checking
    if ((x > width) || (x < 0) || (y > height) || (y < 0)) {
        return;
    }
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

SMRenderer::SMVector SMRenderer::normalize(const SMVector& vecta) {
    double length = sqrt(vecta.x*vecta.x + vecta.y*vecta.y + vecta.z*vecta.z);
    SMVector result;
    result.x = vecta.x/length;
    result.y = vecta.y/length;
    result.z = vecta.z/length;
    return result;
}

SMRenderer::SMVector SMRenderer::project(const SMVector& vect) {
    SMVector result;

    // Transform about camera
    result.x = vect.x - camera.x;
    result.y = vect.y - camera.y;

    // Rotate about camera
    result.z = result.x * cos(angle) + result.y * sin(angle);
    result.x = result.x * sin(angle) - result.y * cos(angle);

    // Perspective Projection


    return result;
}
