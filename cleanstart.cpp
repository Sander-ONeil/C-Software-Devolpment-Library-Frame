#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <chrono>
#include <iostream>
using namespace std;
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

const int CW = SCREEN_WIDTH/2;
const int CH = SCREEN_HEIGHT/2;

float inputs[2] = {0,0};

int main(int argc, char* argv[]) {

    SDL_Init(SDL_INIT_VIDEO);
    
    // create window
    SDL_Window* window = SDL_CreateWindow("Texture Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
    
    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    
    // create texture
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SDL_SetTextureBlendMode(texture,
                            SDL_BLENDMODE_NONE);
    
    bool quit = false;

		SDL_Event e;
		float angle = 0;
		float start = SDL_GetTicks();
		int i = 0;
		while( !quit )
		{
		  i++;
			while ( SDL_PollEvent( &e ) != 0 )
			{
  			switch (e.type)
  			{
  				case SDL_QUIT:
  					quit = true;
  					break;
  				case SDL_KEYDOWN:
  				  switch (e.key.keysym.sym)
  				  {
  				    case SDLK_ESCAPE:
  				      quit = true;
  				      break;
  				  }
  				case SDL_MOUSEMOTION:
  				  inputs[0] += e.motion.xrel/100.;
  				  inputs[1] += e.motion.yrel/100.;
  			}
			}
    void *pixels;
    int pitch;
    
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't lock texture: %s\n", SDL_GetError());
        
    }
    
    Uint8 * d1 = ((Uint8*)pixels);
    Uint32 * dst = (Uint32*)(d1);
    
    for (int row = 0; row < SCREEN_HEIGHT; ++row)
    {
      for (int col = 0; col < SCREEN_WIDTH; ++col)
      {
          
        *(dst++) = (Uint32)row+(col<<8) +row*row+inputs[0]+((int)inputs[1]<<8) + (SCREEN_HEIGHT-row << 16);
      }
      dst = (Uint32*)(d1 += pitch);
    }
    
    SDL_UnlockTexture(texture);
    // copy texture to renderer and display on screen
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
		}
		
    // clean up and quit
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    
    
    float stop = SDL_GetTicks();
  	
    float duration = (stop - start);
  
    cout << "Time taken by function: " << duration << " miliseconds" << endl;
    cout << ((float)i)/(duration/1000.0)<< " framerate" << endl;
    cout << i << " frames" << endl;
    SDL_Quit();
    return 0;
}
