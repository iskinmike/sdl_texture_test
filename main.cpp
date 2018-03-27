
#include "SDL2/SDL.h"

#include <string>
#include <unistd.h>
#include <iostream>

extern "C" { // based on: https://stackoverflow.com/questions/24487203/ffmpeg-undefined-reference-to-avcodec-register-all-does-not-link
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

int width, height;
SDL_Window* screen;
SDL_Renderer* renderer;

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

//    SDL_Surface *BMP_background = SDL_LoadBMP("..\\background.bmp");
//    if (BMP_background == nullptr){
//        std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
//        return ;
//    }

//    SDL_Texture *background = SDL_CreateTextureFromSurface(renderer, BMP_background);
//    SDL_FreeSurface(BMP_background); //Очищение памяти поверхности
//    if (player == nullptr){
//        std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
//        return 1;
//    }

    frame.linesize[0] = 400;
    frame.linesize[1] = 200;
    frame.linesize[2] = 200;

    /* Y */
    int x,y;
    int i = 1;
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
//    SDL_RenderCopy(renderer, background, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(texture);
}

int main(int argc, char const *argv[])
{

    init(200,200, 400,400);

    for (int i = 0; i <5; ++i){
        setFrame();
        sleep(1);
        std::cout << "sleep: " << i << std::endl;
    }

    stopDisplay();

  /* code */
  return 0;
}
