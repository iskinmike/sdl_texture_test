
#include "SDL2/SDL.h"

#include <string>
#include <unistd.h>
#include <iostream>
#include <thread>

int width, height;
SDL_Window* screen;
SDL_Renderer* renderer;

int counter = 0;

std::string title("CAM");

std::string init(int pos_x, int pos_y, int _width, int _height)
{
    width = _width;
    height = _height;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
      return std::string("Could not initialize SDL - ") + std::string(SDL_GetError());
    }

    screen = SDL_CreateWindow(title.c_str(), pos_x, pos_y, width, height, 0);
    if(!screen) {
        return std::string("SDL: could not set video mode - exiting");
    }

    renderer = SDL_CreateRenderer(screen, -1, 0);
    if (!renderer) {
        return std::string("SDL: could not create renderer - exiting");
    }
    return std::string{};
}

void stopDisplay()
{
    SDL_DestroyWindow(screen);
    SDL_DestroyRenderer(renderer);
}

struct Frame{
    uint8_t data[3][400*400*3];
    uint64_t linesize[3];
} frame;

void setFrame(){
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12,
                            SDL_TEXTUREACCESS_TARGET, width, height);

    frame.linesize[0] = 400;
    frame.linesize[1] = 200;
    frame.linesize[2] = 200;

    /* Y */
    int x,y;
    int i = counter++;
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
            frame.data[0][y * frame.linesize[0] + x] = x + y + i * 3;

    /* Cb and Cr */
    for (y = 0; y < height / 2; y++) {
        for (x = 0; x < width / 2; x++) {
            frame.data[1][y * frame.linesize[1] + x] = 128 + y + i * 2;
            frame.data[2][y * frame.linesize[2] + x] = 64 + x + i * 5;
        }
    }

    SDL_UpdateYUVTexture(
        texture,
        NULL,
        frame.data[0], //vp->yPlane,
        frame.linesize[0],
        frame.data[1], //vp->yPlane,
        frame.linesize[1],
        frame.data[2], //vp->yPlane,
        frame.linesize[2]
    );

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);
}


void frameThread(){
    while(true) {
        setFrame();
        usleep(20000);
    }
}

int main(int argc, char const *argv[])
{

    init(200,200, 400,400);

    std::thread thread = std::thread([] {return frameThread();});
    thread.detach();

    for (int i = 0; i <5; ++i){
        sleep(1);
        std::cout << "sleep: " << i << std::endl;
    }

    stopDisplay();

  /* code */
  return 0;
}
