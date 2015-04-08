//
//  SMRenderer.h
//  smtech1
//
//  Created by Matt on 4/2/15.
//  Copyright (c) 2015 Matt. All rights reserved.
//

#ifndef __smtech1__SMRenderer__
#define __smtech1__SMRenderer__

#include <SDL2/SDL.h>
#include <vector>

namespace smtech1 {
    class SMRenderer {
        private:
            struct SMVector;
            SDL_Window* window;
            SDL_Surface* screen;
            Uint32 width;
            Uint32 height;

            double dotProduct(const SMVector& vecta, const SMVector& vectb);
            SMVector crossProduct(const SMVector& vecta, const SMVector& vectb);
            void drawPixel(int x, int y, Uint32 color);
            void drawLine(SMVector& a, SMVector& b, Uint32 color);

        public:
            SMRenderer(SDL_Window*&, SDL_Surface*&, Uint32 width, Uint32 height);
            void render();
    };
}

#endif /* defined(__smtech1__SMRenderer__) */
