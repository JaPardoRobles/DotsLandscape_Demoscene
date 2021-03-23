#include <SDL\include\SDL.h>

#include <iostream>
#include <time.h>

#include "heightmap.c"

using namespace std;

// This is done just for fun.
// I have implemented in C++ a small demo based on the amiga demoscene "Dots Landscape", to practice a little bit in my free time with memory addresses and learn a little bit of SDL 2.0.
// The idea has been taken from here https://youtu.be/ObiLBAhY6dU?t=1m15s

#define WIDTH 1024
#define HEIGHT 768
#define ITERATIONS 128
#define PI 3.14159265359f

struct WindowSettings 
{
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	SDL_Texture* BitmapTex;

    Uint32* Pixels;
    size_t SizePixel;
	Uint32 uiTimeStep;
	bool bQuitGame;

    WindowSettings()
    {
        Window = NULL;
        BitmapTex = NULL;
        Renderer = NULL;
        uiTimeStep = 40;
        bQuitGame = false;
        Pixels = nullptr;
        SizePixel = 0;
    }

} Game;

struct LandscapeData
{
    int X;
    int ProjectedZ;

} Landscape[WIDTH * HEIGHT];

struct HeightmapLandscape 
{
    Uint32 Y;
    int Color;
} Heightmap[ITERATIONS * ITERATIONS];

void InitializeHeightmap(Uint32* tex, float proy, float trans_z, int halfWidth)
{
    Uint32 image_resolution = ITERATIONS;
    Uint32 image_offset = 3;
    Uint32 image_count = 0;
    Uint32 i = 0;

    // Fill Heightmap from heightmap.c 
    for (i = 0; i < image_resolution; ++i) 
    {
        for (Uint32 o = 0; o < image_resolution; ++o)
		{
			Heightmap[i * image_resolution + o].Y = Uint32(heightmap.pixel_data[image_count] >> 1);
            Heightmap[i * image_resolution + o].Color = Uint32(((255 - (Heightmap[i * image_resolution + o].Y)) << 8) |	((0x0000ff & (Heightmap[i * image_resolution + o].Y)) << 2));
			image_count += image_offset;
		}
	}

    i = 0;
    float scale = 7.0f;
    Uint32 HalfIteratins = ITERATIONS >> 1;
	for (Uint32 z = 0; z < HalfIteratins; ++z)
	{
		for (Uint32 x = 0; x < HalfIteratins; ++x)
		{
            Landscape[i].X = int(32.0f * scale - scale * (float)x);
			float temp_z = 32.0f * scale - scale * (float)z;
			temp_z += trans_z;

			Landscape[i].X = int((Landscape[i].X * proy) / temp_z);
			Landscape[i].X += halfWidth;
			temp_z = 1.0f / temp_z;
			temp_z = temp_z * proy;
            Landscape[i].ProjectedZ = (int)(temp_z * HalfIteratins * HalfIteratins);
			++i;
		}
	}
}

void DrawHeightmap(float inTime)
{
    // Normally I wouldn't declare static variables but I'm doing this for fun
    static float count = 0.0f;
    static Uint32 view_x = 0;
    static Uint32 view_z = 0;

    /*
     * it is difficult to determine if register really does any good with today's processors and compilers, 
     * at least I have not been able to get any measurements to show me that it does any good, in windows, maybe in arduino...
     */
    register int HalfHeight = HEIGHT >> 1;
    register Uint32 i, j, k = 0;
    register Uint32 HalfIteration = ITERATIONS >> 1;

    for (i = 0; i < HalfIteration; ++i)
    {
		register HeightmapLandscape* heights = Heightmap + view_x + (i + view_z) * ITERATIONS;

        for (j = 0; j < HalfIteration; ++j)
        {
            int x = Landscape[k].X;
            int rec_z = Landscape[k].ProjectedZ;
            int y = heights[j].Y + 200;
            int yp = y * rec_z;
            yp += HalfHeight - 100;
            yp = yp >> 12;
            Game.Pixels[x + (yp * WIDTH)] = heights[j].Color;
            ++k;
        }
    }

    // Simulate circle rotation
    view_x = Uint32(sin(count) * 32.0f + 32.0f);
    view_z = Uint32(cos(count) * 32.0f + 32.0f);

	count += inTime;
	if (count > (PI) * 2)
	{
		count = 0.0f;
	}
}

bool Init() 
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) 
    {
        cout << "Unable to init SDL: " << SDL_GetError() << "\n";
        return false;
    }

    if (SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &Game.Window, &Game.Renderer) < 0)
    {
        cout << "Unable to CreateWindowAndRenderer \n";
        return false;
    }

	atexit(SDL_Quit);
	SDL_ShowCursor(SDL_ENABLE);

    SDL_SetRenderDrawColor(Game.Renderer, 0, 0, 0, 255);
    SDL_RenderClear(Game.Renderer);
    SDL_RenderPresent(Game.Renderer);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear"); 
    SDL_RenderSetLogicalSize(Game.Renderer, WIDTH, HEIGHT);

    Game.BitmapTex = SDL_CreateTexture(Game.Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (Game.BitmapTex == nullptr)
    {
        cout << "Unable to SDL_CreateTexture \n";
        return false;
    }

    Game.SizePixel = WIDTH * HEIGHT * sizeof(Uint32);
    Game.Pixels = new Uint32[Game.SizePixel];
    memset(Game.Pixels, 0, Game.SizePixel);

    // Horizontal field of view
    float hfov = 80.0f * ((PI * 2.0f) / 360.0f);  // Degrees to radians
    int halfWidth = WIDTH >> 1;
    float projection = (1.0f / tan(hfov * 0.5f)) * (float)halfWidth;
    float offs_z = 500.0f;

    //Fill the heightmap with data
    InitializeHeightmap(&Game.Pixels[0], projection, offs_z, halfWidth);

	return true;
}

void ShutDown() 
{
    SDL_DestroyTexture(Game.BitmapTex);
    SDL_DestroyRenderer(Game.Renderer);
    SDL_DestroyWindow(Game.Window);
    delete[] Game.Pixels;
    SDL_ShowCursor(SDL_ENABLE);
    SDL_Quit();
}

void Draw() 
{
    SDL_RenderClear(Game.Renderer);
    SDL_RenderCopy(Game.Renderer, Game.BitmapTex, NULL, NULL);
    SDL_RenderPresent(Game.Renderer);
}

void InputService() {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT)
        {
            Game.bQuitGame = true;
        }

        if (event.type == SDL_KEYDOWN) {

            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                Game.bQuitGame = true;
            }
        }
    }

    SDL_GetKeyboardState(NULL);
}

void Update(Uint32 inTime)
{
    memset(Game.Pixels, 0, Game.SizePixel);
	DrawHeightmap((float)inTime / 1000.0f);

    if (SDL_UpdateTexture(Game.BitmapTex, NULL, Game.Pixels, WIDTH * sizeof(Uint32)) < 0)
    {
        cout << "Unable to SDL_UpdateTexture \n";
        Game.bQuitGame = true;
    }
}

void Start() 
{
    Uint32 uiCurrentTime = SDL_GetTicks();
    Uint32 dt = 0;
    while (!Game.bQuitGame) {

        InputService();

        Uint32 uiAccumTime = SDL_GetTicks() - uiCurrentTime;
        while (uiAccumTime >= Game.uiTimeStep) {

            Update(Game.uiTimeStep);
			uiCurrentTime += Game.uiTimeStep;
			uiAccumTime = SDL_GetTicks() - uiCurrentTime;
        }
        Draw();
    }
}

int main(int argc, char* argv[]) 
{
    if (Init())
    {
		Start();
    }

    ShutDown();

    return 0;
}
