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
#include "SMVector.h"
#include "SMMesh.h"
#include "MeshLoader.h"

namespace smtech1 {
    class SMRenderer {
        private:
            // Probably useless
            typedef std::chrono::duration<long> lsec;

            // Render thread and control
            std::thread renderThread;
            std::atomic<bool> renderRunning;

            // SDL pointers
            SDL_Window* window;
            SDL_Surface* screen;

            // Screen width/height
            const Uint32 width;
            const Uint32 height;

            // Camera vector
            SMVector camera { 0, 0, 0 };
            // Camera orientation angles
            SMVector orientation { 0, 0, 0};

            // Some useful pi constants;
            static constexpr double pi = 3.14159265359;
            static constexpr double pi2 = 6.28318530718;

            // Bool to indicate whether the window has mouse grab
            bool mousemode = false;

            // Meshes to render
            std::vector<SMMesh> meshes;

            // Thread functions
            void threadinit();
            void render();
            void initMeshes();

            // Render functions
            inline void drawBlank();
            inline void drawPixel(int x, int y, Uint32 color);
            inline void drawLine(const SMVector& pt0, const SMVector& pt1, Uint32 color);

            // Input handler
            inline void getInput(SDL_Event& event);

            // Vector functions
            double dotProduct(const SMVector& vecta, const SMVector& vectb);
            bool intersection(const SMVector& vecta, const SMVector& vectb);
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
