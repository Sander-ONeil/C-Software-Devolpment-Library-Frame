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
  return min(max(max(a,b),c),d);
}
float minf(int a, int b, int c, int d){
  return max(min(min(a,b),c),d);
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
float * na4(float q[4])
{
  float mag = sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
  
  q[0] /= mag;
  q[1] /= mag;
  q[2] /= mag;
  q[3] /= mag;
  
  
  return q;
}

float dot(float a[3],float b[3])
{
  return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

float * anaxis(float a, float ax[3], float out[4])
{

    float c = cos(a);
    float s = sin(a);
    
    
    //static float q[4] = {0,0,0,0};
    ax = na3(ax);
    out[0] = c;
    out[1] = s*ax[0];
    out[2] = s*ax[1];
    out[3] = s*ax[2];
    
    
    return out;
}

float * quatmult(float a[4], float b[4])
{

    float A0  = a[0]*b[0] - a[1]*b[1] - a[2]*b[2] - a[3]*b[3];
    float A1  = a[1]*b[0] + a[3]*b[2] + a[0]*b[1] - a[2]*b[3];
    float A2  = a[2]*b[0] - a[3]*b[1] + a[0]*b[2] + a[1]*b[3];
    float A3  = a[3]*b[0] + a[2]*b[1] - a[1]*b[2] + a[0]*b[3];
    
    a[0] = A0;
    a[1] = A1;
    a[2] = A2;
    a[3] = A3;
    
    return a;
}

void rot(float * v,float * vp, int n,float Q[4])
{

  float * q = na4(Q);

  float qr = q[0];
  float qi = q[1];
  float qj = q[2];
  float qk = q[3];
  
  float s = 1.0;
  float R[3][3] = {
      { 1 - 2*s*(qj*qj+qk*qk), 2*s*(qi*qj-qk*qr), 2*s*(qi*qk+qj*qr)},
      
      { 2*s*(qi*qj+qk*qr), 1 - 2*s*(qi*qi+qk*qk), 2*s*(qj*qk-qi*qr)},
      
      { 2*s*(qi*qk - qj*qr), 2*s*(qj*qk+qi*qr), 1 - 2*s*(qi*qi+qj*qj)}
  };
  
  // float vp[n*3];
  // float* vppoint = vp;

  for (int x = 0; x<n; x++){
    for (int y = 0; y < 3; y++){
      vp[x*3+y] = 0;
      for (int z = 0; z< 3; z++){
        
        vp[x*3+y] += R[z][y]*v[x*3+z];
        
      }
    }
    
  }
  // return vp;
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
      float r = 2;
      W[a] = cos(A)*r;
      H[a] = sin(A*1.0)*r;
      D[a] = cos(A*4);
      
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
    
    float coors[N*3];
    for (int a = 0; a<N; a++)
    {
      coors[a*3] = W[a];
      coors[a*3+1] = H[a];
      coors[a*3+2] = D[a];
    }
    
    float NORMS[N];
    for (int a = 0; a<N/3; a++)
    {
      NORMS[a*3] = Normsx[a];
      NORMS[a*3+1] = Normsy[a];
      NORMS[a*3+2] = Normsz[a];
    }
    
    
    // float q[4]={0,0,0,0};
    // float axis[3] = {0,0,1};
    // float * Q = anaxis(.01,axis,q);
    
    
    // float crot[N*3];
    // float * coorsrot = crot;
    // rot(coors,coorsrot,N,Q);
    
    
    
    // for (int a = 0; a<N; a++)
    // {
    //   D[a] = coorsrot[a*3+2];
    //   W[a] = coorsrot[a*3+0]/D[a]*500+CW;
    //   H[a] = coorsrot[a*3+1]/D[a]*500+CH;
    //   cout<<W[a]<<" "<<H[a]<<" D "<<D[a]<<endl;
    // }
    
    // float nrot[N];
    // float * normsrot = nrot;
    // rot(NORMS,normsrot,N/3,Q);
    
    // for (int a = 0; a<N/3; a++){
    //   Normsx[a] = normsrot[a*3+0];
    //   Normsy[a] = normsrot[a*3+1];
    //   Normsz[a] = normsrot[a*3+2];
    // }
    
    // for (int a = 0; a<N; a+=3){
      
    //   float normp[3] = {Normsx[a/3],Normsy[a/3],Normsz[a/3]};
      
    //   float p[3] = {W[a],H[a],D[a]};
      
    //   NormsD[a/3] = -dot(p,normp);
      
      
    //   cout<<Normsx[a/3]<<" "<<Normsy[a/3]<<" "<<Normsz[a/3]<<" D :"<<NormsD[a/3]<<endl;
    // }
    
    
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
    //////////////////////////////////
    
    
    float q[4]={0,0,0,0};
    float axis[3] = {0,0,1};
    float * Q = anaxis(inputs[0],axis,q);
    float a2[3] = {1,0,0};
    float q2[4]={0,0,0,0};
    float * Q2 = anaxis(inputs[1],a2,q2);
    Q = quatmult(Q2,Q);
    
    float crot[N*3];
    float * coorsrot = crot;
    rot(coors,coorsrot,N,Q);
    
    for (int a = 0; a<N; a++)
    {
      coorsrot[a*3+2]+=4;
      
    }
    
    
    for (int a = 0; a<N; a++)
    {
      D[a] = coorsrot[a*3+2];
      W[a] = coorsrot[a*3+0]/D[a]*500+CW;
      H[a] = coorsrot[a*3+1]/D[a]*500+CH;
      // cout<<W[a]<<" "<<H[a]<<" D "<<D[a]<<endl;
    }
    
    // float nrot[N];
    // float * normsrot = nrot;
    // rot(NORMS,normsrot,N/3,Q);
    
    // for (int a = 0; a<N/3; a++){
    //   Normsx[a] = normsrot[a*3+0];
    //   Normsy[a] = normsrot[a*3+1];
    //   Normsz[a] = normsrot[a*3+2];
    // }
    
    // for (int a = 0; a<N; a+=3){
      
    //   float normp[3] = {Normsx[a/3],Normsy[a/3],Normsz[a/3]};
      
    //   float p[3] = {W[a],H[a],D[a]};
      
    //   NormsD[a/3] = -dot(p,normp);
      

    // }
    
    
    
    
    for (int a = 0; a<N; a+=3){
      
      
      // float v1[3] = {W[a]-W[a+1],H[a]-H[a+1],D[a]-D[a+1]};
      // float v2[3] = {W[a]-W[a+2],H[a]-H[a+2],D[a]-D[a+2]};
      // float p[3] = {W[a],H[a],D[a]};
      
      
      float v1[3] = {coorsrot[a*3+0]-coorsrot[a*3+3+0],coorsrot[a*3+1]-coorsrot[a*3+3+1],coorsrot[a*3+2]-coorsrot[a*3+3+2]};
      float v2[3] = {coorsrot[a*3+0]-coorsrot[a*3+6+0],coorsrot[a*3+1]-coorsrot[a*3+6+1],coorsrot[a*3+2]-coorsrot[a*3+6+2]};
      float p[3] = {*(coorsrot+a*3+6),coorsrot[a*3+1+6],coorsrot[a*3+2+6]};
      
      
      
      float * v1p = na3(v1);
      float * v2p = na3(v2);
      
      
      float norm[3] = {
        crossx ( v1p[0],v1p[1],v1p[2],v2p[0],v2p[1],v2p[2]),
        crossy ( v1p[0],v1p[1],v1p[2],v2p[0],v2p[1],v2p[2]),
        crossz ( v1p[0],v1p[1],v1p[2],v2p[0],v2p[1],v2p[2])};
      
      float * normp = norm;
      
      normp  = na3(normp);
      
      Normsx[a/3]  = *normp;
      Normsy[a/3]  = *(normp+1);
      Normsz[a/3]  = *(normp+2);
      
      
      
      
      NormsD[a/3] = -dot(p,normp);
      
      
      // cout<<Normsx[a/3]<<" "<<Normsy[a/3]<<" "<<Normsz[a/3]<<" D :"<<NormsD[a/3]<<endl;
    }
    
    ///////////////////////
    
    
    //std::fill(zbuffer,zbuffer)
    
    for (int a = 0; a<N; a+=3)
    {
      int bym = maxf(H[a],H[a+1],H[a+2],SCREEN_HEIGHT);
      int byl = minf(H[a],H[a+1],H[a+2],0);
      
      if (bym>byl){
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
        
        int bxm = min(max(tops[0],tops[1]),SCREEN_WIDTH);
        int bxl = max(min(tops[0],tops[1]),0);
        
        
        if (bxl<bxm)
        {
          dst = (Uint32 *)((Uint8 *)pixels + y*pitch) + bxl;
          float A = Normsx[a/3];
          float B = Normsy[a/3];
          float C = Normsz[a/3];
          float Dd = NormsD[a/3];
          float n[3] = {A,B,C};
          // cout<<A<<" A "<<B<<" B "<<endl;
          
          
        for (int x = bxl; x<bxm; x++)
        {
          float v[3] = {((float)x-CW)/500,((float)y-CH)/500,1};
          float * V = v;
          float pp[3] = {coorsrot[a*3],coorsrot[a*3+1],coorsrot[a*3+2]};
          
          
          float d = dot(pp,n)/dot(n,V);  //(-(A*x+B*y+D)/C);
          
          // cout<<d<<" A : "<<a<<endl;
          // float d = (-(A*x+B*y+Dd)/C);
          //float d = (Dd)/(x+y-C);
          
          
          
          if (d < zbuffer[y*SCREEN_WIDTH+x])
          {
            dst = (Uint32 *)((Uint8 *)pixels + y*pitch) + x;
          *(dst) = (int)(d*1000) +100+ (a*4<<16) ;//(int)(d)<<7 + (a*4<<16);
          zbuffer[y*SCREEN_WIDTH+x] = d;

          }
          
        }
          
        }
      }}
      
      
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
