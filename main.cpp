#include <windows.h>
#include <iostream>
#include <SDL.h>
#include <string>

/*current plan :
store colors in a 16bit int that contains hex codes for the colors later to be decoded int rgb values by
taking the first two digits and converting them to decimal the result is the rgb value of it's respective chanel.
after that we make a matrix with all the colors stored in it then in the fall function we work according to that matrix*/

const uint32_t lightYellow2 = 0b11111111111111111001100111111111;
const uint32_t blue = 0b00000000000000001111111111111111;
const uint32_t saddlebrown = 0b10001011010001010001001111111111;
const uint32_t black = 0b01110000100000001001000011111111;//0b11110000111111111111111111111111;
const uint32_t red = 0b11111111010001010000000011111111;
const uint32_t grey = 0b11010011110100111101001111111111;
const uint32_t ice = 0b01111111111111111001100111111111;

//game setup
bool Game = true;
int x = 200;
int y = 150;
const int w = 480;
const int h = 480;
const int coeff = 4;
bool bigBrush = true;
SDL_Renderer* renderer = nullptr;
SDL_Window* win = nullptr;

//player input
int mouseX;
int mouseY;

//fps

int frameDelay = 1000, FPS, frameCount;

double TimerLimit = 3000.00;
struct cell {
	uint32_t color;
	int lastX;
	bool checked;
	bool flamable;
	double timer = TimerLimit;
	double diff;
};


uint32_t color = 0;
cell grid[w / coeff][h / coeff];

void Clear();
void sand(int i, int j);
void water(int i, int j);
void fire(int i, int j);
void smoke(int i, int j);

void EventCheck(SDL_Event* event)
{
	if (event->type == SDL_WINDOWEVENT) {
		switch (event->window.event)
		{
		case SDL_WINDOWEVENT_CLOSE:
			OutputDebugStringA("Close Window Event. \n");
			Game = false;
			break;
		default:
			OutputDebugStringA("unhandled Event. \n");
			break;
		}
	}
}
bool StartUpError(SDL_Window* win, SDL_Renderer* renderer)
{
	OutputDebugStringA("\n");
	bool b = true;
	if (!win) {
		OutputDebugStringA("window not instanisiated properly."); b = false;
	}
	else if (!renderer) {
		OutputDebugStringA("renderer not instansiated properly."); b = false;
	}

	return b;
}

void SetAPixel(int x, int y, uint32_t Color) {
	uint8_t r = Color >> 24;
	uint8_t g = (Color >> 16) - ((Color >> 24) << 8);
	uint8_t b = (Color >> 8) - (Color >> 16 - Color >> 24 << 8) + (Color >> 24 << 16);

	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_RenderDrawPoint(renderer, x, y);
	grid[y][x].color = Color;
}
void goTo(int fx, int fy, int x, int y)
{
	int32_t aux = grid[fy][fx].color;
	SetAPixel(fx, fy, grid[y][x].color);
	SetAPixel(x, y, aux);
}

void simulate()
{
	//byte update_mask = (frameCount & (long)1) << 7; // take the first byte of frameCount
	for (int i = (h / coeff - 1); i > 0; i--) {
		for (int j = (w / coeff - 1); j > 0; j--) {
			if (!grid[i][j].checked)
			{
				switch (grid[i][j].color)
				{
				case black:
					break;
				case lightYellow2: // in case of sand
					sand(i, j);
					break;
				case blue: //in case of water
					water(i, j);
					break;
				case red: //in case of fire
					fire(i, j);
					break;
				case grey: //in case of smoke
					smoke(i, j);
					break;
				case ice: //in case od ice
					if (grid[i + 1][j].color != ice)
						SetAPixel(j, i + 1, blue);
					break;
				default:
					break;
				}
			}

		}
	}

	for (int i = 0; i < w / coeff; i++)
	{
		for (int j = 0; j < h / 4; j++) {
			grid[i][j].checked = false;
		}
	}
}

void input(SDL_Event* event, SDL_Window* win)
{
	if (event->type == SDL_KEYDOWN) {
		switch (event->key.keysym.sym)
		{
		case SDLK_ESCAPE:
			OutputDebugStringA("Close Window Event. \n");
			Game = false;
			break;
		case SDLK_a:
			//SDL_SetRenderDrawColor(renderer, 255, 255, 153, 255); // sand 
			color = lightYellow2;
			break;
		case SDLK_z:
			//SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // water
			color = blue;
			break;
		case SDLK_e:
			//SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // dirt
			color = saddlebrown;
			break;
		case SDLK_d:
			//SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // delete button || air
			color = black;
			break;
		case SDLK_r:
			color = red;
		case SDLK_f:
			//TODO : fullscreen
			break;
		case SDLK_s:
			color = ice;
			break;
		case SDLK_m:
			bigBrush = false;
			break;
		case SDLK_b:
			bigBrush = true;
			break;
		case SDLK_c:
			Clear();
			break;
		default:
			OutputDebugStringA("unhandled input.\n");
			break;
		}
	}

	//mouse events	
	mouseX = event->button.x / coeff;
	mouseY = event->button.y / coeff;
	if (event->button.button == SDL_BUTTON_LEFT && color != 0)
	{
		//newPixel(Pixel, 0, 255, 255, 153, mouseX, mouseY);
		uint8_t r = color >> 24;
		uint8_t g = (color >> 16) - ((color >> 24) << 8);
		uint8_t b = (color >> 8) - (color >> 16 - color >> 24 << 8) + (color >> 24 << 16);
		SDL_SetRenderDrawColor(renderer, r, g, b, 255);
		SDL_Rect DrawArea;
		DrawArea.x = mouseX;
		DrawArea.y = mouseY;
		DrawArea.h = 3;
		DrawArea.w = 3;
		if (bigBrush)
		{
			for (int i = mouseX; i < mouseX + 3; i++) {
				for (int j = mouseY; j < mouseY + 3; j++) {
					grid[j][i].color = color;
				}
			}
			SDL_RenderDrawRect(renderer, &DrawArea);
			SDL_RenderDrawPoint(renderer, mouseX + 1, mouseY + 1);
		}
		else {
			grid[mouseY][mouseX].color = color;
			SDL_RenderDrawPoint(renderer, mouseX, mouseY);
			SDL_RenderPresent(renderer);
		}

	}
}

int main(int argc, char** args)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	if (SDL_Init(SDL_INIT_EVERYTHING)) // initialize SDL
	{
		OutputDebugStringA("SDL not instansiated properly.");
		OutputDebugStringA(SDL_GetError());
	}

	win = SDL_CreateWindow("pixel simulation", x, y, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE); //create the window
	// ↑ must be passed as a pointer cuz the program can't read the code or some shit like that
	renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);//get the surface of the windo to draw stuff on it

	SDL_RenderSetScale(renderer, coeff, coeff);//multiplies the renderers scale by 4 (to have the pixels be visisble )

	Clear();

	OutputDebugStringA("Creating Window...\n");
	if (!StartUpError(win, renderer)) {
		OutputDebugStringA(SDL_GetError());
		return 1;
	}
	OutputDebugStringA("Window Created Succesfuly!\n");

	//end of program
	while (Game)	//game loop
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			EventCheck(&event); //check if the [X] button is pressed or not (add other functionality in the future)
			input(&event, win); // check for gameplay related input
		}

		//frame Rate  related stuff
		frameCount++;
		if (SDL_GetTicks() > frameDelay)
		{
			FPS = frameCount;
			frameCount = 0;

			frameDelay = SDL_GetTicks() + 1000;

		}
		SDL_Delay(1000 / 30); // control the FPS !!NEVER PUT TO 1!!
		//update game state & draw current frame
		simulate();

		SDL_RenderPresent(renderer);
	}

	//↓close down the window
	SDL_DestroyWindow(win);
	win = nullptr;
	//↓shut down SDL itself
	SDL_Quit();

	delete win, renderer;

	return 0;
}

void sand(int i, int j)
{
	if (grid[i + 1][j].color == black || grid[i + 1][j].color == blue) {
		goTo(j, i, j, i + 1);
	}
	else if ((grid[i + 1][j + 1].color == black || grid[i + 1][j + 1].color == blue) && (grid[i][j + 1].color == black || grid[i][j + 1].color == blue)) {
		goTo(j, i, j + 1, i + 1);
	}
	else if ((grid[i + 1][j - 1].color == black || grid[i + 1][j - 1].color == blue) && (grid[i][j - 1].color == black || grid[i][j - 1].color == blue)) {
		goTo(j, i, j - 1, i + 1);
	}
}
void water(int i, int j)
{
	if (grid[i + 1][j].color == black) {
		goTo(j, i, j, i + 1);
	}
	else if (grid[i + 1][j - 1].color == black) {
		goTo(j, i, j - 1, i + 1);
	}
	else if (grid[i + 1][j + 1].color == black) {
		goTo(j, i, j + 1, i + 1);
	}
	else {
		if (grid[i][j + 1].color == black && grid[i][j].lastX != j + 1) {
			goTo(j, i, j + 1, i);
			grid[i][j + 1].checked = true;
		}
		else if (grid[i][j - 1].color == black && grid[i][j].lastX != j - 1) { //ahmed & amina
			goTo(j, i, j - 1, i);
			grid[i][j - 1].checked = true;
		}
		else if (grid[i][j + 1].color == black && grid[i][j].lastX == j + 1 && grid[i][j - 1].color != black) {
			goTo(j, i, j + 1, i);
			grid[i][j - 1].checked = true;
		}
		else if (grid[i][j - 1].color == black && grid[i][j].lastX == j - 1 && grid[i][j + 1].color != black) {
			goTo(j, i, j - 1, i);
			grid[i][j + 1].checked = true;
		}
	}
}
void fire(int i, int j)
{
	if (grid[i - 1][j].color == grey)
		smoke(i - 1, j);
	if (grid[i - 1][j].color == black)
		SetAPixel(j, i - 1, grey);
	if (grid[i][j].timer == TimerLimit) {
		grid[i][j].timer = SDL_GetTicks() + TimerLimit;
	}
	else if (grid[i][j].timer >= SDL_GetTicks()) {
		grid[i][j].diff = grid[i][j].timer - SDL_GetTicks();
		grid[i][j].diff /= grid[i][j].timer;
		grid[i][j].diff *= 255;
		SDL_SetRenderDrawColor(renderer, 255, 69 - grid[i][j].diff, 0, 255);
		SDL_RenderDrawPoint(renderer, j, i);

	}
	else if (grid[i][j].timer <= SDL_GetTicks()) {
		SetAPixel(j, i, black);
		grid[i][j].timer = TimerLimit;
	}
}

void smoke(int i, int j)
{

	if (grid[i][j].timer == TimerLimit) {
		grid[i][j].timer = SDL_GetTicks() + TimerLimit;
	}
	else if (grid[i][j].timer >= SDL_GetTicks()) {
		grid[i][j].diff = grid[i][j].timer - SDL_GetTicks();
		grid[i][j].diff /= grid[i][j].timer;
		grid[i][j].diff *= 34;
		SDL_SetRenderDrawColor(renderer, 211 + grid[i][j].diff, 211 + grid[i][j].diff, 211 + grid[i][j].diff, 255);
		SDL_RenderDrawPoint(renderer, j, i);

	}
	else if (grid[i][j].timer <= SDL_GetTicks()) {
		SetAPixel(j, i, black);
		grid[i][j].timer = TimerLimit;
	}


	if (grid[i - 1][j].color == black && grid[i][j].checked != true) {
		goTo(j, i, j, i - 1);
		grid[i - 1][j].checked = true;
	}
	else if (grid[i - 1][j + 1].color == black && grid[i][j].checked != true) {
		goTo(j, i, j + 1, i - 1);
		grid[i - 1][j + 1].checked = true;
	}
	else if (grid[i - 1][j - 1].color == black && grid[i][j].checked != true) {
		goTo(j, i, j - 1, i - 1);
		grid[i - 1][j + 1].checked = true;
	}
	else {
		if (grid[i][j + 1].color == black && grid[i][j].lastX != j + 1) {
			goTo(j, i, j + 1, i);
			grid[i][j + 1].checked = true;
		}
		else if (grid[i][j - 1].color == black && grid[i][j].lastX != j - 1) { //ahmed & amina
			goTo(j, i, j - 1, i);
			grid[i][j - 1].checked = true;
		}
		else if (grid[i][j + 1].color == black && grid[i][j].lastX == j + 1 && grid[i][j - 1].color != black) {
			goTo(j, i, j + 1, i);
			grid[i][j + 1].checked = true;
		}
		else if (grid[i][j - 1].color == black && grid[i][j].lastX == j - 1 && grid[i][j + 1].color != black) {
			goTo(j, i, j - 1, i);
			grid[i][j - 1].checked = true;
		}
	}
}

void Clear()
{
	uint8_t r = black >> 24;
	uint8_t g = (black >> 16) - ((black >> 24) << 8);
	uint8_t b = (black >> 8) - (black >> 16 - black >> 24 << 8) + (black >> 24 << 16);

	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_RenderClear(renderer);

	for (int i = 0; i < h / coeff; i++)
	{
		for (int j = 0; j < w / coeff; j++)
		{
			grid[i][j].color = black;
		}
	}

	r = saddlebrown >> 24;
	g = (saddlebrown >> 16) - ((saddlebrown >> 24) << 8);
	b = (saddlebrown >> 8) - (saddlebrown >> 16 - saddlebrown >> 24 << 8) + (saddlebrown >> 24 << 16);

	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	for (int i = 0; i < h / coeff; i++)
	{
		grid[i][h / coeff - 1].color = saddlebrown;
		SDL_RenderDrawPoint(renderer, h / 4 - 1, i);
		SDL_RenderDrawPoint(renderer, 0, i);
		grid[i][0].color = saddlebrown;
	}
	for (int j = 0; j < w / coeff; j++)
	{
		grid[w / coeff - 1][j].color = saddlebrown;
		SDL_RenderDrawPoint(renderer, j, w / 4 - 1);
		grid[0][j].color = saddlebrown;
	}
	SDL_RenderPresent(renderer);

	r = black >> 24;
	g = (black >> 16) - ((black >> 24) << 8);
	b = (black >> 8) - (black >> 16 - black >> 24 << 8) + (black >> 24 << 16);

	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
}

//LRESULT CALLBACK window_callback(HWND hund, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//	LRESULT result = 0;
//	switch (uMsg)
//	{
//		case WM_CLOSE:
//		case WM_DESTROY:
//			Game = false;
//			break;
//		default:
//			return result = DefWindowProc(hund, uMsg, wParam, lParam);
//	}
//
//	return result;
//}
//
//int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
//	// Create window Class
//	WNDCLASS window_class = {};
//	window_class.style = CS_HREDRAW | CS_VREDRAW;
//	window_class.lpszClassName = "Game Window Class";
//	window_class.lpfnWndProc = window_callback;
//	// Register Class
//	RegisterClass(&window_class);
//	// Create window
//	HWND window = CreateWindow(window_class.lpszClassName, "pixel simulation", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, 0, 0, hInstance, 0);
//
//	while(Game)
//	{
//		//Input
//		MSG message;
//		while(PeekMessage(&message, window, 0, 0, PM_REMOVE))
//		{
//			TranslateMessage(&message);
//			DispatchMessage(&message);
//		}
//	}
//
//}