/* 
 * 
 * 
 */


#include <iostream>
#include "SDL/SDL.h"
using std::cout;
using std::endl;


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	const int RMASK = 0xFF000000;
	const int GMASK = 0x00FF0000;
	const int BMASK = 0x0000FF00;
	const int AMASK = 0x000000FF;
	const int RED_INDEX   = 3;
	const int GREEN_INDEX = 2;
	const int BLUE_INDEX  = 1;
	const int ALPHA_INDEX = 0;
#else
	const int RMASK = 0x000000FF;
	const int GMASK = 0x0000FF00;
	const int BMASK = 0x00FF0000;
	const int AMASK = 0xFF000000;
	const int RED_INDEX   = 0;
	const int GREEN_INDEX = 1;
	const int BLUE_INDEX  = 2;
	const int ALPHA_INDEX = 3;
#endif


const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;


SDL_Surface *SCREEN;
SDL_Surface *whiteSquare;
SDL_Surface *backGround;
SDL_Rect whiteSquareDstRect;

bool running = false;
bool redrawBackGround = true;
bool redrawWhiteSquare = true;
SDL_Event	event;
int magicNumberA = 7;
int magicNumberB = 2;



bool init()
{
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
		return false;
	if ( SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF) == NULL )
		return false;
	if ( ( SCREEN = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE) ) == NULL )
		return false;
	SDL_WM_SetCaption("Pixel Crunching", NULL);
	return true;
}

void buildBackGround()
{
	backGround = SDL_CreateRGBSurface(
		SDL_HWSURFACE, 
		SCREEN_WIDTH/4, 
		SCREEN_HEIGHT/4, 
		32, 
		RMASK, GMASK, BMASK, AMASK
	);
}
void drawBackGround()
{
	SDL_FillRect(backGround, NULL, SDL_MapRGBA(backGround->format, 0x77, 0x00, 0x33, 0xFF) );//BGR, apparently?
	Uint32 lineColor = SDL_MapRGBA(backGround->format, 0xFF, 0x00, 0x00, 0x77);//BGR, apparently?
	Uint32 *pixel;
	int pixcount = backGround->h * backGround->w;
	SDL_LockSurface(backGround);
		for (int i = 0; i < pixcount; i++)
			if (i%magicNumberA == 0  ||  i%magicNumberB == 0)
			{
				pixel = (Uint32*)backGround->pixels + i;
				*pixel = lineColor;
			}
	SDL_UnlockSurface(backGround);
}

void buildWhiteSquare()
{
	whiteSquare = SDL_CreateRGBSurface(
		SDL_HWSURFACE, 
		SCREEN_WIDTH/2, 
		SCREEN_HEIGHT/2, 
		32, 
		RMASK, GMASK, BMASK, AMASK
	);
	whiteSquareDstRect.x = SCREEN_WIDTH/2 - whiteSquare->w/2;
	whiteSquareDstRect.y = SCREEN_HEIGHT/2 - whiteSquare->h/2;
}
void drawWhiteSquare()
{
	Uint32 *pixel;
	Uint32 triangleColor;
	SDL_Rect square;
	const int squareSize = whiteSquare->w / 8;
	square.w = square.h = squareSize;
	SDL_LockSurface(whiteSquare);
		for (int row = 0; row < whiteSquare->h/squareSize; row++)
		{
			square.y = row*squareSize;
			for (int col = 0; col < whiteSquare->w/squareSize; col++)
			{
				square.x = col*squareSize;
				SDL_FillRect( whiteSquare, &square, SDL_MapRGBA( whiteSquare->format, 0xFF, 0x00, 0x77, rand()/(RAND_MAX/0xFF + 1) ) );//RGB!
				triangleColor = SDL_MapRGBA( whiteSquare->format, 0xFF, 0x00, 0x77, rand()/(RAND_MAX/0xFF + 1) );//RGB!
				for (int innerRow = 0; innerRow < squareSize; innerRow++)
				{
					for (int innerCol = 0; innerCol < innerRow; innerCol++)
					{
						pixel = (Uint32*)whiteSquare->pixels 
							+ row * whiteSquare->w * squareSize 
							+ innerRow * whiteSquare->w 
							+ col * squareSize 
							+ (row%2 ? (col%2 ? squareSize-innerCol : innerCol) : (col%2 ? innerCol : squareSize-innerCol) )
						;
						*pixel = triangleColor;
					}
				}
			}
		}
	SDL_UnlockSurface(whiteSquare);
}

void bigBlit(SDL_Surface *src, SDL_Surface *dst, int embiggenFactor)
{
	Uint32 *srcPixel;
	Uint32 color;
	int pixcount = src->h * src->w;
	SDL_Rect bigPixel;
	bigPixel.w = bigPixel.h = embiggenFactor;
	SDL_LockSurface(src);
		for (int i = 0; i < pixcount; i++)
		{
			srcPixel = (Uint32*)src->pixels + i;
			color = *srcPixel;
			bigPixel.y = embiggenFactor*(i / src->w);
			bigPixel.x = embiggenFactor*(i % src->w);
			SDL_FillRect(dst, &bigPixel, color);
		}
	SDL_UnlockSurface(src);
}

void logVideoInfo()
{
	const SDL_VideoInfo *VI = SDL_GetVideoInfo();
	cout << endl;
	cout << "VideoInfo:" << endl;
	cout << "\thw_available: " << VI->hw_available << endl;
	cout << "\twm_available: " << VI->wm_available << endl;
	cout << "\tblit_hw     : " << VI->blit_hw      << endl;
	cout << "\tblit_hw_CC  : " << VI->blit_hw_CC   << endl;
	cout << "\tblit_hw_A   : " << VI->blit_hw_A    << endl;
	cout << "\tblit_sw     : " << VI->blit_sw      << endl;
	cout << "\tblit_sw_CC  : " << VI->blit_sw_CC   << endl;
	cout << "\tblit_sw_A   : " << VI->blit_sw_A    << endl;
	cout << "\tblit_fill   : " << VI->blit_fill    << endl;
	cout << "\tvideo_mem   : " << VI->video_mem    << endl;
	cout << endl;
}

void checkEvents()
{
	while ( SDL_PollEvent(&event) )
		switch (event.type)
		{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_KEYDOWN:
				SDL_GetKeyName(event.key.keysym.sym);
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
						running = false;
						break;
					case SDLK_UP:
						if (magicNumberA >= SCREEN->w)
							magicNumberA = 2;
						else
							magicNumberA++;
						redrawBackGround = true;
						cout << magicNumberA << "\t" << magicNumberB << endl;
						break;
					case SDLK_DOWN:
						if (magicNumberA <= 3)
							magicNumberA = 2;
						else
							magicNumberA--;
						redrawBackGround = true;
						cout << magicNumberA << "\t" << magicNumberB << endl;
						break;
					case SDLK_RIGHT:
						if (magicNumberB >= SCREEN->w)
							magicNumberB = 2;
						else
							magicNumberB++;
						redrawBackGround = true;
						cout << magicNumberA << "\t" << magicNumberB << endl;
						break;
					case SDLK_LEFT:
						if (magicNumberB <= 3)
							magicNumberB = 2;
						else
							magicNumberB--;
						redrawBackGround = true;
						cout << magicNumberA << "\t" << magicNumberB << endl;
						break;
					case SDLK_SPACE:
						redrawWhiteSquare = true;
						break;
				}
				break;
		}
}



int main(int argc, char *argv[])
{
	if ( init() )
		running = true;
	else
	{
		cout << SDL_GetError() << endl;
		return 1;
	}
	
	logVideoInfo();
	
	buildWhiteSquare();
	buildBackGround();
	
	cout << "Shuffle the transparency of the white triangles with the space bar." << endl;
	cout << "Change the background pattern with the arrow keys." << endl;
	cout << "The numbers listed below are what determine the background pattern:" << endl;
	cout << magicNumberA << "\t" << magicNumberB << endl;
	
	while (running)
	{
		if (redrawBackGround || redrawWhiteSquare)
		{
			if (redrawBackGround)
			{
				drawBackGround();
				redrawBackGround = false;
			}
			else
			{
				drawWhiteSquare();
				redrawWhiteSquare = false;
			}
			
			bigBlit(backGround, SCREEN, SCREEN->w/backGround->w);
			SDL_BlitSurface(whiteSquare, NULL, SCREEN, &whiteSquareDstRect);
			
			SDL_Flip(SCREEN);
		}
 		
		SDL_Delay(30);
		checkEvents();
	}
	
	cout << endl;
	
	SDL_FreeSurface(backGround);
	SDL_FreeSurface(whiteSquare);
	SDL_Quit();
	return 0;
}