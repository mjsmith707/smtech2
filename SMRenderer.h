//
//  SMRenderer.h
//  smtech1
//
//  Created by Matt on 4/2/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef __smtech1__SMRenderer__
#define __smtech1__SMRenderer__

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>

namespace smtech1 {
    class SMRenderer {
        private:
            typedef std::chrono::duration<long> lsec;
            typedef struct SMVector {
                double x;
                double y;
                double z;
                double w;
            } SMVector;

            std::thread renderThread;
            std::atomic<bool> renderRunning;

            SDL_Window* window;
            SDL_Surface* screen;
            Uint32 width;
            Uint32 height;
            SMVector camera { 0, 0, 0 };
            double angle = 0;

            void threadinit();
            void render();
            void drawPixel(int x, int y, Uint32 color);
            void drawLine(SMVector& a, SMVector& b, Uint32 color);
            double dotProduct(const SMVector& vecta, const SMVector& vectb);
            SMVector crossProduct(const SMVector& vecta, const SMVector& vectb);
            SMVector normalize(const SMVector& vecta);
            SMVector project(const SMVector& vecta);
        public:
            SMRenderer(Uint32 width, Uint32 height);
            ~SMRenderer();
            void run();
    };
}

#endif /* defined(__smtech1__SMRenderer__) */
