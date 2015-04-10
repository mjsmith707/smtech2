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
            // Probably useless
            typedef std::chrono::duration<long> lsec;

            // Vector struct
            typedef struct SMVector {
                double x;
                double y;
                double z;
            } SMVector;

            // Render thread and control
            std::thread renderThread;
            std::atomic<bool> renderRunning;

            // SDL pointers
            SDL_Window* window;
            SDL_Surface* screen;

            // Screen width/height
            const Uint32 width;
            const Uint32 height;

            // Camera vector centered
            SMVector camera { width/2, height/2, 0 };
            // Camera orientation angles
            SMVector orientation { 0, 0, 0};
            // 2D angle
            double angle = 0;

            // Thread functions
            void threadinit();
            void render();

            // Render functions
            inline void drawBlank();
            inline void drawPixel(int x, int y, Uint32 color);
            inline void drawLine(const SMVector& pt0, const SMVector& pt1, Uint32 color);

            // Vector functions
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
