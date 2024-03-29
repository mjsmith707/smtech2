//
//  SMRenderer.cpp
//  smtech1
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
}

// Public run call for render thread
void SMRenderer::run() {
    renderRunning = true;
// If OSX or Windows run in calling thread
// If Linux/Unix run in new thread
#if defined( __MACH__)
    threadinit();
#elif defined(__WINDOWS__)
    threadinit();
#else
    renderThread = std::thread(&SMRenderer::threadinit, this);
#endif
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

    // Grab Mouse
    mousemode = SDL_SetRelativeMouseMode(SDL_TRUE) == 0 ? true : false;

    // Create some render data
    initMeshes();

    // Enter render loop
    render();

    // Cleanup
    SDL_Quit();
}

// Loads meshes into SMMesh objects for rendering
// Will be replaced with a file format and more concrete Mesh structure
void SMRenderer::initMeshes() {
    // Origin
    SMVector vecto {0, 0, 0 };
    // X-Axis
    SMVector vectx {20000, 0, 0 };
    // Y-Axis
    SMVector vecty {0, 20000, 0 };
    // Z-Axis
    SMVector vectz {0, 0, 20000 };

    SMLine xaxisl { vecto, vectx, 0xCCFFFF };
    SMLine yaxisl { vecto, vecty, 0xFFFFCC };
    SMLine zaxisl { vecto, vectz, 0xFF99FF };
    std::vector<SMLine> axislines { xaxisl, yaxisl, zaxisl };
    SMMesh axis(axislines);

    // Cube
    SMVector vecta { 0, 200, 0 };
    SMVector vectb { 0, 200, 200 };
    SMVector vectc { 0, 0, 200 };
    SMVector vectd { 0, 0, 0 };
    SMVector vecte { 200, 0, 0 };
    SMVector vectf { 200, 200, 0 };
    SMVector vectg { 200, 200, 200 };
    SMVector vecth { 200, 0, 200 };

    SMLine line1 { vecta, vectb, 0x33CC33};
    SMLine line2 { vectb, vectc, 0xFF0000};
    SMLine line3 { vectc, vectd, 0x0000FF};
    SMLine line4 { vectd, vecta, 0xFFFF00};
    SMLine line5 { vecte, vectf, 0x33CC33};
    SMLine line6 { vectf, vectg, 0xFF0000};
    SMLine line7 { vectg, vecth, 0x0000FF};
    SMLine line8 { vecth, vecte, 0xFFFF00};
    SMLine line9 { vecta, vectf, 0xCC00CC};
    SMLine line10 { vectb, vectg, 0xCC00CC};
    SMLine line11 { vectd, vecte, 0xCC00CC};
    SMLine line12 { vectc, vecth, 0xCC00CC};
    std::vector<SMLine> lines {line1,line2,line3,line4,line5,line6,line7,line8,line9,line10,line11,line12};
    SMMesh cube(lines);

    meshes.push_back(axis);
    meshes.push_back(cube);
}

// Main render thread loop function
void SMRenderer::render() {
    // Frame capping still WIP
    // std::chrono is wack to work with
    const int r_fps = 60;
    // Framerate as 1/60th of a second
    using framerate = std::chrono::duration<uint64_t, std::ratio<1, r_fps>>;
    uint64_t lastfpstime = 0;
    uint64_t fps = 0;

    SDL_Event event;

    auto lasttime = std::chrono::high_resolution_clock::now();
    // Main render loop
    // Checks atomic renderRunning bool to see if we should quit
    while (renderRunning) {
        // Frame capping still TODO
        auto currtime = std::chrono::high_resolution_clock::now();
        auto frametime = currtime - lasttime;
        lasttime = currtime;

        lastfpstime += frametime.count();
        fps++;

        // Take this with a grain of salt
        // Needs to be converted to std::chrono somehow
        if (lastfpstime > 1000000000) {
            std::cout << "fps: " << fps << std::endl;
            lastfpstime = 0;
            fps = 0;
        }

        // Erase framebuffer
        drawBlank();

        // Render all lines from each mesh
        for (auto m : meshes) {
            for (auto l : m.lines) {
                SMVector tvect0 = project(l.pt1);
                SMVector tvect1 = project(l.pt2);
                drawLine(tvect0, tvect1, l.color);
            }
        }

        // Flip buffer
        SDL_UpdateWindowSurface(window);

        // Check for input
        // Will be spun off into a different class/thread someday
        getInput(event);

        // This probably isn't correct.
        while (frametime < framerate{1}) {
            std::this_thread::sleep_for(frametime);
            frametime += frametime;
        }
    }
}

inline void SMRenderer::getInput(SDL_Event& event) {
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_KEYDOWN:
                std::cout << "keydown" << std::endl;
                // Needs movement calculation relative to orientation
                switch (event.key.keysym.sym) {
                    case SDLK_w:
                        camera.z += 2;
                        break;
                    case SDLK_a:
                        camera.x -= 2;
                        break;
                    case SDLK_s:
                        camera.z -= 2;
                        break;
                    case SDLK_d:
                        camera.x += 2;
                        break;
                    case SDLK_ESCAPE:
                        if (mousemode) {
                            mousemode = SDL_SetRelativeMouseMode(SDL_FALSE) == 0 ? false : true;
                        }
                        else {
                            mousemode = SDL_SetRelativeMouseMode(SDL_TRUE) == 0 ? true : false;
                        }
                        break;
                    case SDLK_q:
                        renderRunning = false;
                        break;
                }
            case SDL_MOUSEMOTION:
                if (!mousemode) {
                    break;
                }
                // Clamping camera angle to 0<2pi with wraparound;
                if (event.motion.xrel < 0) {
                    if (orientation.y < 0) {
                        orientation.y = pi2;
                    }
                    else if (orientation.y > pi2) {
                        orientation.y = 0;
                    }
                    orientation.y -= 0.01;
                }
                else if (event.motion.xrel > 0) {
                    if (orientation.y < 0) {
                        orientation.y = pi2;
                    }
                    else if (orientation.y > pi2) {
                        orientation.y = 0;
                    }
                    orientation.y += 0.01;
                }
                if (event.motion.yrel < 0) {
                    if (orientation.x < 0) {
                        orientation.x = pi2;
                    }
                    else if (orientation.x > pi2) {
                        orientation.x = 0;
                    }
                    orientation.x += 0.01;
                }
                else if (event.motion.yrel > 0) {
                    if (orientation.x < 0) {
                        orientation.x = pi2;
                    }
                    else if (orientation.x > pi2) {
                        orientation.x = 0;
                    }
                    orientation.x -= 0.01;
                }
                std::cout << "camera: <" << camera.x << ", " << camera.y << ", " << camera.z << ">" << std::endl
                << "angle: <" << orientation.x << ", " << orientation.y << ", " << orientation.z << ">" << std::endl;
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
    // More debug bounds checking
    // Stop that long ass line draw
    if ((fabs(pt0.x) > 10000000) || (fabs(pt0.y) > 10000000) || (fabs(pt1.x) > 10000000) || (fabs(pt1.y) > 10000000)) {
        return;
    }

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
    if ((x >= width) || (x < 0) || (y >= height) || (y < 0)) {
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

// Vector intersection
bool SMRenderer::intersection(const SMVector& vecta, const SMVector& vectb) {
    return false;
}

// Some useless vector math functions
double SMRenderer::dotProduct(const SMVector& vecta, const SMVector& vectb) {
    double dot = vecta.x * vectb.x;
    dot += vecta.y * vectb.y;
    dot += vecta.z * vectb.z;
    return dot;
}

// Black magic
SMVector SMRenderer::crossProduct(const SMVector& vecta, const SMVector& vectb) {
    double x = vecta.y*vectb.z - vecta.z*vectb.y;
    double y = vecta.z*vectb.x - vecta.x*vectb.z;
    double z = vecta.x*vectb.y - vecta.y*vectb.x;

    SMVector cross { x, y, z };
    return cross;
}

// Convert to normal vector
SMVector SMRenderer::normalize(const SMVector& vecta) {
    double length = sqrt(vecta.x*vecta.x + vecta.y*vecta.y + vecta.z*vecta.z);
    SMVector result;
    result.x = vecta.x/length;
    result.y = vecta.y/length;
    result.z = vecta.z/length;
    return result;
}

// Do some fancy vector math
SMVector SMRenderer::project(const SMVector& vect) {
    SMVector temp;
    SMVector temp2;

    // Camera Translation
    temp.x = vect.x - camera.x;
    temp.y = vect.y - camera.y;
    temp.z = vect.z - camera.z;

    // Camera Transform
    temp2.x = cos(orientation.y)*(sin(orientation.z)*temp.y + cos(orientation.z)*temp.x) - sin(orientation.y)*temp.z;
    temp2.y = sin(orientation.x)*(cos(orientation.y)*temp.z + sin(orientation.y)*(sin(orientation.z)*temp.y + cos(orientation.z)*temp.x)) + cos(orientation.x)*(cos(orientation.z)*temp.y - sin(orientation.z)*temp.x);
    temp2.z = cos(orientation.x)*(cos(orientation.y)*temp.z + sin(orientation.y)*(sin(orientation.z)*temp.y + cos(orientation.z)*temp.x)) - sin(orientation.x)*(cos(orientation.z)*temp.y - sin(orientation.z)*temp.x);

    // Projection
    SMVector result;
    if (temp2.z > 0) {
        // Saving this for now
        result.x = (temp2.x * width / temp2.z);
        result.y = (temp2.y * height / temp2.z);
        result.z = temp2.z;
    } else {
        result.x = 0;
        result.y = 0;
        result.z = 0;
    }

    return result;
}
