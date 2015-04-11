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
    // Spawn and start renderer
    SMRenderer renderer(640, 480);;
    renderer.run();

    // IO Block until we want to quit
    std::string whatever;
    std::cin >> whatever;
    return 0;
}


