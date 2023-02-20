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

float maxf(int a, int b, int c, int d){
  return max(max(max(a,b),c),d);
}
float minf(int a, int b, int c, int d){
  return min(min(min(a,b),c),d);
}

float crossx(float a1, float b1, float c1, float a2, float b2, float c2)
{
  return b1 * c2 - c1 * b2;
}
float crossy(float a1, float b1, float c1, float a2, float b2, float c2)
{
  return c1 * a2 - a1 * c2;
}
float crossz(float a1, float b1, float c1, float a2, float b2, float c2)
{
  return a1 * b2 - b1 * a2;
}

float * na3(float q[3])
{
  float mag = sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2]);
  
  q[0] /= mag;
  q[1] /= mag;
  q[2] /= mag;
  
  
  return q;
}

float dot(float a[3],float b[3])
{
  return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

int main(int argc, char* argv[])
{

    SDL_Init(SDL_INIT_VIDEO);
    
    // create window
    SDL_Window* window = SDL_CreateWindow("Texture Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
    
    // create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    
    // create texture
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    SDL_SetTextureBlendMode(texture,
                            SDL_BLENDMODE_NONE);
    
    int N = 60;
    // float W[N] = {0,1,0,0,0,1};
    // float H[N] = {1,1,0,1,0,1};
    // float D[N] = {0,1,1,1,0,0};
    
    
    // for (int a = 0; a < N; a++)
    // {
      
    //   W[a] *=300;
    //   H[a] *= 300;
    //   D[a] *=300;
      
      
      
    // }
    
    float W[N];
    float H[N];
    float D[N];
    
    float Normsx[N/3];
    float Normsy[N/3];
    float Normsz[N/3];
    float NormsD[N/3];
    
    for (int a = 0; a < N; a++)
    {
      float A = ((float)a)*3.14157/3*.9;
      float r = 1;
      W[a] = 400*cos(A)*r+500;
      H[a] = 400*sin(A*1.0)*r+500;
      D[a] = 400*cos(A*10+2)+450;
      
      
      
    }
    
    
    
    
    
    
    for (int a = 0; a<N; a+=3){
      
      float norm[3] = {
        crossx ( W[a]-W[a+1],H[a]-H[a+1],D[a]-D[a+1],W[a]-W[a+2],H[a]-H[a+2],D[a]-D[a+2]),
        crossy ( W[a]-W[a+1],H[a]-H[a+1],D[a]-D[a+1],W[a]-W[a+2],H[a]-H[a+2],D[a]-D[a+2]),
        crossz ( W[a]-W[a+1],H[a]-H[a+1],D[a]-D[a+1],W[a]-W[a+2],H[a]-H[a+2],D[a]-D[a+2])};
      
      float * normp  = na3(norm);
      
      Normsx[a/3]  = *normp;
      Normsy[a/3]  = *(normp+1);
      Normsz[a/3]  = *(normp+2);
      
      
      float p[3] = {W[a],H[a],D[a]};
      
      NormsD[a/3] = -dot(p,normp);
      
      
      cout<<Normsx[a/3]<<" "<<Normsy[a/3]<<" "<<Normsz[a/3]<<" D :"<<NormsD[a/3]<<endl;
    }
    
    
    float zbuffer[SCREEN_WIDTH*SCREEN_HEIGHT] = {0};
    
    
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
        *(dst++) = 0;
        zbuffer[row*SCREEN_WIDTH+col] = 1<<15;
      }
      dst = (Uint32*)(d1 += pitch);
    }
    
    
    
    
    //std::fill(zbuffer,zbuffer)
    
    for (int a = 0; a<N; a+=3)
    {
      int bym = maxf(H[a],H[a+1],H[a+2],0);
      int byl = minf(H[a],H[a+1],H[a+2],SCREEN_HEIGHT);
      
      for (int y = byl; y<bym; y++)
      {
        
        
        int tops[3] = {0,0,0};
        int topsc = 0;
        for (int c = 0; c < 3; c++)
        {
          float dt = (y-H[a+c])/(H[a+(c+1)%3]-H[a+c]);
          
          if ((dt < 1) and (dt>0))
          {
            
            tops[topsc] = W[a+c] + dt * (W[a + (c+1)%3]-W[a+c]);
            
            //dst = (Uint32 *)((Uint8 *)pixels + y*pitch) + tops[topsc];
            
            //*(dst) = 250;
            
            topsc++;
            
          }
        }
        
        int bxm = max(max(tops[0],tops[1]),0);
        int bxl = min(min(tops[0],tops[1]),SCREEN_HEIGHT);
        
        dst = (Uint32 *)((Uint8 *)pixels + y*pitch) + bxl;
        if (bxl<bxm)
        {
          float A = Normsx[a/3];
          float B = Normsy[a/3];
          float C = Normsz[a/3];
          float D = NormsD[a/3];
          
        for (int x = bxl; x<bxm; x++)
        {
          
          float d = (-(A*x+B*y+D)/C);
          
          if (d <= zbuffer[y*SCREEN_WIDTH+x])
          {
          *(dst) = (a*60)+((-a+3)<<15)+(int)(d)<<7;//(int)(d)<<7 + (a*4<<16);
          zbuffer[y*SCREEN_WIDTH+x] = d;
          //cout<<d;}
          }
          dst++;
        }
          
        }
      }
      
      
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
