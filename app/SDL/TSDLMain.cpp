
#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static SDL_FRect mouseposrect;

SDL_AppResult SDL_AppIterate(void *appstate)
{
	(void)appstate;
	Uint8 r;

	/* fade between shades of red every 3 seconds, from 0 to 255. */
	r = (Uint8) ((((float) (SDL_GetTicks() % 3000)) / 3000.0f) * 255.0f);
	SDL_SetRenderDrawColor(renderer, r, 0, 0, 255);

	/* you have to draw the whole window every frame. Clearing it makes sure the whole thing is sane. */
	SDL_RenderClear(renderer);  /* clear whole window to that fade color. */

	/* set the color to white */
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	/* draw a square where the mouse cursor currently is. */
	SDL_RenderFillRect(renderer, &mouseposrect);

	/* put everything we drew to the screen. */
	SDL_RenderPresent(renderer);

	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
	(void)appstate;
	switch (event->type) {
		case SDL_EVENT_QUIT:  /* triggers on last window close and other things. End the program. */
			return SDL_APP_SUCCESS;

		case SDL_EVENT_KEY_DOWN:  /* quit if user hits ESC key */
			if (event->key.scancode == SDL_SCANCODE_ESCAPE) {
				return SDL_APP_SUCCESS;
			}
			break;

		case SDL_EVENT_MOUSE_MOTION:  /* keep track of the latest mouse position */
			/* center the square where the mouse is */
			mouseposrect.x = event->motion.x - (mouseposrect.w / 2);
			mouseposrect.y = event->motion.y - (mouseposrect.h / 2);
			break;
	}
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
	(void)appstate;
	(void)argv;
	(void)argc;

	SDL_SetAppMetadata("SDL Hello World Example", "1.0", "com.example.sdl-hello-world");

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("SDL_Init(SDL_INIT_VIDEO) failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	if (!SDL_CreateWindowAndRenderer("Hello SDL", 640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
		SDL_Log("SDL_CreateWindowAndRenderer() failed: %s", SDL_GetError());
		return SDL_APP_FAILURE;
	}

	mouseposrect.x = mouseposrect.y = -1000;  /* -1000 so it's offscreen at start */
	mouseposrect.w = mouseposrect.h = 50;

	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
	(void)appstate;
	(void)result;

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
