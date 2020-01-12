#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_image.h>

void Slock(SDL_Surface *screen)
{
  if ( SDL_MUSTLOCK(screen) )
    if ( SDL_LockSurface(screen) < 0 )
      return;
}

void Sulock(SDL_Surface *screen)
{
  if ( SDL_MUSTLOCK(screen) )
    SDL_UnlockSurface(screen);
}


void fade_code(SDL_Surface *im1, SDL_Surface *im2, Uint8 alpha, SDL_Surface* imOut)
{
    int pixelsCount = imOut->w * im1->h;
    int byteCount = imOut->format->BytesPerPixel * pixelsCount;

    Uint8 *A = (Uint8*) im1->pixels;
    Uint8 *B = (Uint8*) im2->pixels;
    Uint8 *out = (Uint8*) imOut->pixels;
    Uint8 *end = out + byteCount;

    for(; out != end; out++,A++,B++)
        *out = (Uint8)(( (*A) * alpha + (*B) * (128 - alpha))/128) ;
}

uint32_t MMXBlend(uint32_t source, uint32_t overlap, uint8_t alpha){
   asm volatile(
      "movd %%edx,%%mm3;"::"d"(alpha));
   asm volatile(
       "movd %%edx,%%mm4;"::"d"(128-alpha));
   asm volatile(
           "movd %%edx,%%mm1;"::"d"(source));
  asm volatile(
            "movd %%edx,%%mm2;"::"d"(overlap));
   asm volatile(
       "pxor %mm0, %mm0;"
       "punpcklbw %mm0,%mm1;"
       "punpcklbw %mm0,%mm2;"
      "pmullw %mm3,%mm1;"
      "pmullw %mm4,%mm2;"
      "paddw %mm2, %mm1;"
      "psrlw $7,%mm1;"
      "packuswb %mm0,%mm1;"
   );
   asm volatile(
       "movd %%mm1,%%edx":"=d"(source));
   return source;
}
uint32_t MMXBlend2(uint32_t source, uint32_t overlap, uint8_t alpha){
asm volatile("pxor %mm3, %mm3");
/*asm volatile( "mov %%eax, %[dst];"::[dst]"d"(source));
asm volatile( "movd %%mm0,%[dst];"::[dst]"d"(source));
asm volatile( "movd %%mm1,%[src];"::[src]"d"(overlap));
asm volatile( "movd %%mm2,%[alpha];"::[alpha]"d"(alpha));*/
asm volatile("movd %%edx,%%mm0;"::"d"(source));
asm volatile("movd %%edx,%%mm1;"::"d"(overlap));
asm volatile("movd %%edx,%%mm2;"::"d"(alpha));

asm volatile("punpcklbw %mm3, %mm0");
asm volatile("punpcklbw %mm3, %mm1");
asm volatile("punpcklbw %mm2, %mm2");
asm volatile("punpcklbw %mm2, %mm2");
asm volatile("punpcklbw %mm3, %mm2");

asm volatile("psubusb %mm0, %mm1");
asm volatile("pmullw %mm2, %mm1");
asm volatile("psrlw $8, %mm1");
asm volatile("paddusw %mm0, %mm1");

asm volatile("packuswb %mm3, %mm1");

asm volatile("movd %%mm1, %%edx":"=d"(source));

return source;

}
void fade_mmx(SDL_Surface* im1,SDL_Surface* im2,Uint8 alpha, SDL_Surface* imOut)
{
    int pixelsCount = imOut->w * im1->h;
    Uint8 alpha1Table[] = {alpha, alpha, alpha, alpha};
    //Uint8 alpha1Table[] = {0, alpha, 0, alpha, 0, alpha, 0, alpha};
    // Uint8 alpha2 = 128 - alpha;
    // Uint8 alpha2Table[] =  {alpha2, alpha2, alpha2, alpha2, alpha2, alpha2, alpha2};

    Uint32 A = (Uint32) im1->pixels;
    Uint32 B = (Uint32) im2->pixels;
    Uint32 out = (Uint32) imOut->pixels;
    Uint32 *end = out + pixelsCount;
    //printf("TU JESZCZE DZIALA\n");

    /* TUTAJ WPISZ KOD WYKORZYSTUJACY MMX !!! */
	for(; out <= end; out++,A++,B++){
		  out = MMXBlend(A,B,alpha);
	}
	__asm__("emms" : : ); // koniec
}

int main(int argc, char *argv[])
{
    char *file1 = "image1.jpg",*file2 = "image2.jpg";

    if(argc == 3){
        file1 = argv[1];
        file2 = argv[2];
    }

    if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ){
        printf("Unable to init SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_Surface *screen;
    screen = SDL_SetVideoMode( 800, 600,32, SDL_HWSURFACE | SDL_DOUBLEBUF);

    if ( screen == NULL ){
        printf("Unable to set %dx%d video: %s\n",800, 600, SDL_GetError());
        exit(1);
    }

    SDL_Surface *image1,*image2,*output;
    SDL_Surface *temp;

    temp = IMG_Load(file1);
    if (temp == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        return 1;
    }
    image1 = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    temp = IMG_Load(file2);
    if (temp == NULL)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
        return 1;
    }
    image2 = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    if( image1->w != image2->w || image1->h != image2->h )
    {
        printf("Bad bitmaps sizes\n");
        return 1;
    }

    temp = SDL_CreateRGBSurface(
        SDL_SWSURFACE, image1->w, image1->h,
        image1->format->BitsPerPixel,
        image1->format->Rmask, image1->format->Gmask,
        image1->format->Bmask, image1->format->Amask
    );

    output = SDL_DisplayFormat(temp);
    SDL_FreeSurface(temp);

    SDL_Rect src, dest;

    src.x = 0;
    src.y = 0;
    src.w = output->w;
    src.h = output->h;

    dest.x = 0;
    dest.y = 0;
    dest.w = output->w;
    dest.h = output->h;

    int maxFPS = 0;
    int FPS = 0;
    int lastTimeFPS = 0;

    int done = false;
    int mode = 1, lastMode = 0;
    int alpha;
    Uint8 f = 128;
    char buffer[32];

    while(!done)
    {
        SDL_Event event;

        while ( SDL_PollEvent(&event) )
        {
            if ( event.type == SDL_QUIT )
                done = true;

            if ( event.type == SDL_KEYDOWN )
            {
                if ( event.key.keysym.sym == SDLK_ESCAPE )
                    done = true;
                if ( event.key.keysym.sym == SDLK_1 )
                    mode = 1;
                if ( event.key.keysym.sym == SDLK_2 )
                    mode = 2;
            }
        }

        alpha = f<0x80 ? (f&0x7f) : 0x80-(f&0x7f);
        f++;

        switch(mode)
        {
            case 1:
                fade_mmx(image1, image2, alpha, output);
                break;
            default:
                fade_mmx(image1, image2, alpha, output);
                break;
        }

        int currentTime = SDL_GetTicks();

        Slock(screen);
        SDL_BlitSurface(output, &src, screen, &dest);
        Sulock(screen);

        SDL_Flip(screen);
        FPS++;

        if ( currentTime - lastTimeFPS >= 1000 )
        {
            if (FPS > maxFPS)
                maxFPS = FPS;

            if (lastMode != mode)
                maxFPS = 0;

            snprintf( buffer, sizeof(buffer), "%d FPS (max:%d) [%d]", FPS, maxFPS, mode );
            SDL_WM_SetCaption( buffer,0 );
            FPS = 0;
            lastTimeFPS = currentTime;
            lastMode = mode;
        }
    }

    SDL_Quit();

    return 0;
}
