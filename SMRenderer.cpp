//
//  SMRenderer.cpp
//  smtech1
//
//  Created by Matt on 4/2/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#include "SMRenderer.h"
using namespace smtech1;

// Initialize thread controls, width, height
SMRenderer::SMRenderer(Uint32 width, Uint32 height) : renderThread(), renderRunning(false), width(width), height(height) {

}

// Destructor shuts down thread before rejoining
// Beautiful RAII at work
SMRenderer::~SMRenderer() {
    renderRunning = false;
    if (renderThread.joinable()) {
        renderThread.join();
    }
    SDL_Quit;
}

// Public run call for render thread
void SMRenderer::run() {
    renderRunning = true;
    renderThread = std::thread(&SMRenderer::threadinit, this);
}

// Render thread entry point
// Initialize SDL window and surface
void SMRenderer::threadinit() {
    // 100 years from now we'll return these errors to a Game class or something
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "SDL Init Failed" << std::endl;
        return;
    }
    else {
        std::cout << "SDL Init Successful" << std::endl;
    }

    window = NULL;
    screen = NULL;

    // Initialize the SDL Window
    window = SDL_CreateWindow("smtech1", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cout << "Window not created: " << SDL_GetError() << std::endl;
        return;
    }
    else {
        // Grab SDL_Surface, draw initial blank screen
        screen = SDL_GetWindowSurface(window);
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0x0, 0x0, 0x0));
        SDL_UpdateWindowSurface(window);
    }

    // Enter render loop
    render();
}

// Main render thread loop function
void SMRenderer::render() {
    // Frame capping still TODO
    //const int r_fps = 60;
    //const int framerate = 1000/r_fps;
    //long currtime = std::chrono::time_point_cast<lsec>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();
    SDL_Event event;

    // Some sample vectors
    SMVector vecta { 0, 0, 0 };
    SMVector vectb { width, height, 0 };
    SMVector vectc { 0, height, 0 };
    SMVector vectd { width, 0, 0 };

    // Main render loop
    // Checks atomic renderRunning bool to see if we should quit
    while (renderRunning) {
        // Frame capping still TODO


        // Erase framebuffer
        drawBlank();

        // Do some fancy vector math (does nothing)
        SMVector tvecta = project(vecta);
        SMVector tvectb = project(vectb);
        SMVector tvectc = project(vectc);
        SMVector tvectd = project(vectd);

        // Render lines
        drawLine(tvecta, tvectb, 0x0000FF);
        drawLine(tvectc, tvectd, 0xFFCC00);
        drawPixel(camera.x, camera.y, 0x00FF00);

        // Flip buffer
        SDL_UpdateWindowSurface(window);

        // Check for input
        // Will be spun off into a different class/thread someday
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                    std::cout << "keydown" << std::endl;
                    switch (event.key.keysym.sym) {
                        case SDLK_w:
                            camera.y -= 2;
                            break;
                        case SDLK_a:
                            camera.x -= 2;
                            break;
                        case SDLK_s:
                            camera.y += 2;
                            break;
                        case SDLK_d:
                            camera.x += 2;
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

// Blanks surface
inline void SMRenderer::drawBlank() {
    for (unsigned int y=0; y<height; y++) {
        for (unsigned int x=0; x<width; x++) {
            drawPixel(x,y,0x0);
        }
    }
}

// Bresenham line algorithm for _all_ octants
// http://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C.2B.2B
// Wasn't for lack of trying..
inline void SMRenderer::drawLine(const SMVector& pt0, const SMVector& pt1, Uint32 color) {
    int x1 = pt0.x;
    int y1 = pt0.y;
    int x2 = pt1.x;
    int y2 = pt1.y;

    const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
    if (steep) {
        std::swap(x1, y1);
        std::swap(x2, y2);
    }

    if (x1 > x2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    const float dx = x2 - x1;
    const float dy = fabs(y2 - y1);

    float error = dx / 2.0f;
    const int ystep = (y1 < y2) ? 1 : -1;
    int y = (int)y1;

    const int maxX = (int)x2;

    for (int x=(int)x1; x<maxX; x++) {
        if(steep) {
            drawPixel(y,x, color);
        }
        else {
            drawPixel(x,y, color);
        }

        error -= dy;
        if (error < 0) {
            y += ystep;
            error += dx;
        }
    }
}

// Draw pixel to x,y coordinate
inline void SMRenderer::drawPixel(int x, int y, Uint32 pixel) {
    // Debugging bounds checking
    if ((x > width) || (x < 0) || (y > height) || (y < 0)) {
        return;
    }

    // http://www.libsdl.org/release/SDL-1.2.15/docs/html/guidevideo.html
    int bpp = screen->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    // Could probably do away with all of this and use 4
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

// Some useless vector math functions
double SMRenderer::dotProduct(const SMVector& vecta, const SMVector& vectb) {
    double dot = vecta.x * vectb.x;
    dot += vecta.y * vectb.y;
    dot += vecta.z * vectb.z;
    return dot;
}

// Black magic
SMRenderer::SMVector SMRenderer::crossProduct(const SMVector& vecta, const SMVector& vectb) {
    double x = vecta.y*vectb.z - vecta.z*vectb.y;
    double y = vecta.z*vectb.x - vecta.x*vectb.z;
    double z = vecta.x*vectb.y - vecta.y*vectb.x;

    SMVector cross { x, y, z };
    return cross;
}

// Convert to normal vector
SMRenderer::SMVector SMRenderer::normalize(const SMVector& vecta) {
    double length = sqrt(vecta.x*vecta.x + vecta.y*vecta.y + vecta.z*vecta.z);
    SMVector result;
    result.x = vecta.x/length;
    result.y = vecta.y/length;
    result.z = vecta.z/length;
    return result;
}

// Do some fancy vector math (i.e. nothing now)
SMRenderer::SMVector SMRenderer::project(const SMVector& vect) {
    SMVector result;

    // Translate about camera
    // Why? IDK
    //result.x = vect.x - camera.x;
    //result.y = vect.y - camera.y;

    // Rotate about camera
    // Doesnt really work either
    //result.x = cos(angle)*result.x - sin(angle)*result.y;
    //result.y = sin(angle)*result.x + cos(angle)*result.y;

    return vect;
}
