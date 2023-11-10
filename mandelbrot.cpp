#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
using namespace std;
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <list>
#include <typeinfo>
#include <iterator>
#include <time.h>
#include <chrono>
#include <cstdint>

#include <stdio.h>
#include <stdbool.h>
#include <cstring>
#include <math.h>
#include <complex>
#include <string.h>

#define WIDTH 800
#define HEIGHT 800
#define FPS 30
#define PI 3.14159265

uint64_t timems() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

vector<int> mandel(int x, int y, int iter, int hoff, int voff, float scale){
    float x2 = ((float)x-600*(HEIGHT/800)-hoff)/(300.0*scale);
    float y2 = ((float)y-400*(WIDTH/800)+voff)/(300.0*scale);
    complex<float> c = {x2,y2};
    int i = 0;
    complex<float> z = {0.0,0.0};
    while(i<iter && abs(z)<2){
        z = z*z + c;
        i++;
    }
    int col = 255*i/iter;
    return (vector<int>){col,(int)(col*0.75),(int)(col*0.25)};
}

void createIMG(int iter, float scale, int hoff, int voff, string imgname){
    FILE *f;
    unsigned char *img = NULL;
    int filesize = 54 + 3*WIDTH*HEIGHT;

    img = (unsigned char *)malloc(3*WIDTH*HEIGHT);
    memset(img,0,3*WIDTH*HEIGHT);

    for(int i=0; i<WIDTH; i++){
        for(int j=0; j<HEIGHT; j++){
            int x=i, y=(HEIGHT-1)-j;
            vector<int> rgb = mandel(i,j,iter,hoff, voff, scale);
            img[(x+y*WIDTH)*3+2] = (unsigned char)(rgb[0]);
            img[(x+y*WIDTH)*3+1] = (unsigned char)(rgb[1]);
            img[(x+y*WIDTH)*3+0] = (unsigned char)(rgb[2]);
        }
    }

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmppad[3] = {0,0,0};

    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);

    bmpinfoheader[ 4] = (unsigned char)(WIDTH    );
    bmpinfoheader[ 5] = (unsigned char)(WIDTH>> 8);
    bmpinfoheader[ 6] = (unsigned char)(WIDTH>>16);
    bmpinfoheader[ 7] = (unsigned char)(WIDTH>>24);
    bmpinfoheader[ 8] = (unsigned char)(HEIGHT    );
    bmpinfoheader[ 9] = (unsigned char)(HEIGHT>> 8);
    bmpinfoheader[10] = (unsigned char)(HEIGHT>>16);
    bmpinfoheader[11] = (unsigned char)(HEIGHT>>24);

    imgname = "./images/"+ imgname;

    f = fopen(imgname.c_str(),"wb");
    fwrite(bmpfileheader,1,14,f);
    fwrite(bmpinfoheader,1,40,f);
    for(int i=0; i<HEIGHT; i++)
    {
        fwrite(img+(WIDTH*(HEIGHT-i-1)*3),3,WIDTH,f);
        fwrite(bmppad,1,(4-(WIDTH*3)%4)%4,f);
    }

    free(img);
    fclose(f);
}

int main(int argc, char* argv[])
{
    /* Initializes the timer, audio, video, joystick,
    haptic, gamecontroller and events subsystems */
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
      printf("Error initializing SDL: %s\n", SDL_GetError());
      return 0;
    }
    /* Create a window */
    SDL_Window* wind = SDL_CreateWindow("Engine",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WIDTH, HEIGHT, 0);
    if (!wind)
    {
      printf("Error creating window: %s\n", SDL_GetError());
      SDL_Quit();
      return 0;
    }
    /* Create a renderer */
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(wind, -1, render_flags);

    if (!rend) {
      printf("Error creating renderer: %s\n", SDL_GetError());
      SDL_DestroyWindow(wind);
      SDL_Quit();
      return 0;
    }

    /* Main loop */
    bool running = true, up = false, right = false, left = false, down = false;
    int iter = 50;
    float scale = 1.0;
    int h_offset = 0, v_offset = 0;
    SDL_Event event;

    /* ofstream img ("mandel.png");
    img << "P3" << endl;
    img << "800 800" << endl;
    img << "255" << endl; */

    uint64_t t = timems();

    /* for(int x = 0; x < WIDTH; x++){
          for(int y = 0; y< HEIGHT; y++){
            vector<int> col = mandel(x,y,50,h_offset,v_offset);
            img << col[0] << " " << col[1] << " " << col[2] << endl;
            SDL_SetRenderDrawColor(rend, col[0], col[1], col[2], 255);
            SDL_RenderDrawPoint(rend, x, y);
        }
    } */

    createIMG(iter, scale, h_offset, v_offset, "img0.bmp");

    cout << "loaded in " << timems() - t << "ms" << endl;

    vector<SDL_Surface*> SDLimages;
    vector<vector<float>> images; // 0 - SDLimages vector pointer 1 - x coord 2 - y coord 3 - x size at scale 1 4 - y size at scale 1
    	
    SDL_Surface * image = SDL_LoadBMP("img0.bmp");
    
    SDL_Texture * texture = SDL_CreateTextureFromSurface(rend, image);
    SDLimages.push_back(image);
    //vector<float> temp = {0.0, (-600*(HEIGHT/800))/((float)300.0*scale),(-400*(HEIGHT/800))/((float)300.0*scale),800,800};
    images.push_back((vector<float>){0.0, (-600*(HEIGHT/800))/((float)300.0*scale),(-400*(HEIGHT/800))/((float)300.0*scale),800,800});
    SDL_Rect dstrect = { 0, 0, 800, 800};
    SDL_RenderCopy(rend, texture, NULL, &dstrect);
    SDL_RenderPresent(rend);
    string imgname;
    vector<float> temp;

    while (running)
    {
        /* Process events */
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                            v_offset += 100;
                            up = true;
                            break;
                        case SDL_SCANCODE_DOWN:
                            v_offset -= 100;
                            down = true;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            h_offset -= 100;
                            right = true;
                            break;
                        case SDL_SCANCODE_LEFT:
                            h_offset += 100;
                            left = true;
                            break;
                        case SDL_SCANCODE_W:
                            scale *= 1.1;
                            if (h_offset < 0){
                                h_offset -= 100;
                            } else {
                                if (h_offset > 0){
                                    h_offset += 100;
                                }
                            }
                            break;
                        case SDL_SCANCODE_S:
                            scale /= 1.1;
                            if (h_offset < 0){
                                h_offset -= 100;
                            } else {
                                if (h_offset > 0){
                                    h_offset += 100;
                                }
                            }
                            break;
                        case SDL_SCANCODE_R:
                            imgname = "img"+to_string(size(images))+".bmp";
                            createIMG(iter, scale, h_offset, v_offset, imgname);
                            image = SDL_LoadBMP(imgname.c_str());
                            SDLimages.push_back(image);
                            cout << size(SDLimages) << endl;
                            temp = {size(images), (-600*(HEIGHT/800)-h_offset)/((float)300.0*scale),(-400*(HEIGHT/800)-v_offset)/((float)300.0*scale),800/scale,800/scale};
                            images.push_back(temp);
                            break;
                        default:
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_UP:
                            up = false;
                        case SDL_SCANCODE_DOWN:
                            down = false;
                        case SDL_SCANCODE_RIGHT:
                            right = false;
                        case SDL_SCANCODE_LEFT:
                            left = false;
                        default:
                            break;
                      }
                    break;
                default:
                  break;
            }
        }
      /* Clear screen */
      SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
      SDL_RenderClear(rend);
      
      /* Draw to window and loop */

        for (long long unsigned int i = 0; i<size(images); i++){
            SDL_Rect dstrect = {
                (int)(images[i][1] * (300.0 * scale) + h_offset + 600 * (WIDTH / 800)),
                (int)(images[i][2] * (300.0 * scale) + v_offset + 400 * (HEIGHT / 800)),
                (int)(images[i][3] * scale),
                (int)(images[i][4] * scale)
            };
            
            imgname = "./images/img"+to_string(i)+".bmp";
            if (image != nullptr)
            {
                SDL_FreeSurface(image);
            }
            image = SDL_LoadBMP(imgname.c_str());
            if (texture != nullptr)
            {
                SDL_DestroyTexture(texture);
            }
            texture = SDL_CreateTextureFromSurface(rend, image);
            SDL_RenderCopy(rend, texture, NULL, &dstrect);
            //cout << "rendered image " << i << endl;
        }

        SDL_RenderPresent(rend);
        SDL_Delay(1000/FPS);
        //cout << "render" << endl;
    }
    /* Release resources */
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(wind);
    SDL_Quit();
    return 0;
}