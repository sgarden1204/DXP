// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>

#include"CommandCenter.h"
#include"Sound.h"

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface
LPDIRECTSOUNDBUFFER g_lpDSBG;
						//스프라이트 선언

						//텍스쳐를 선언
LPDIRECT3DTEXTURE9 sprite_start_menu;
LPDIRECT3DTEXTURE9 sprite_end_menu;

LPDIRECT3DTEXTURE9 sprite_background;
//LPDIRECT3DTEXTURE9 sprite_ui;

LPDIRECT3DTEXTURE9 sprite_base;
LPDIRECT3DTEXTURE9 sprite_base_attack;
LPDIRECT3DTEXTURE9 sprite_base_shoot;

void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory

void init_game(void);
void do_game_logic(void);
bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1);

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


using namespace std;
//////////////////////////////////////////////////

enum class GameState : int 
{
	start_menu, end_menu, ingame
};

enum class GameStage : int
{
	stage1, stage2, stage3, stage4, stage5, stage6
};

GameState gamestate = GameState::start_menu;
GameStage gamestage = GameStage::stage1;

CommandCenter CC;
Sound sound;


//기본 클래스 
class entity {

};

bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1)
{

	if ((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1) < (size0 + size1) * (size0 + size1))
		return true;
	else
		return false;

}

//주인공 클래스 

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"WindowClass";

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(NULL, L"WindowClass", L"DX9 Final Project",
		WS_OVERLAPPEDWINDOW, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	// set up and initialize Direct3D
	initD3D(hWnd);

	init_game();
	// enter the main loop:

	MSG msg;

	while (TRUE)
	{
		DWORD starting_point = GetTickCount();

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		do_game_logic();

		render_frame();

		// check the 'escape' key
		if (KEY_DOWN(VK_ESCAPE))
			PostMessage(hWnd, WM_DESTROY, 0, 0);

		while ((GetTickCount() - starting_point) < 25);
	}

	// clean up DirectX and COM
	cleanD3D();

	return msg.wParam;
}


// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));
	//d3dpp.Windowed = FALSE;
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;
	//d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth = SCREEN_WIDTH;
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;
	//전체화면-> 창모드 설정

	//사운드 초기화


	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	sound.CreateDirectSound(hWnd);
	//사운드 크리에이트
	D3DXCreateSprite(d3ddev, &d3dspt);    // create the Direct3D Sprite object
										  //스프라이트 생성
										  //이 이후로 텍스쳐를 생성한다

	//게임 시작 메뉴
	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"GameStart.png",    // the file name
		D3DX_DEFAULT_NONPOW2,    // default width
		D3DX_DEFAULT_NONPOW2,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_start_menu);    // load to sprite

	//게임 끝내기 메뉴
	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"GameEnd.png",    // the file name
		D3DX_DEFAULT_NONPOW2,    // default width
		D3DX_DEFAULT_NONPOW2,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_end_menu);    // load to sprite

	// 백그라운드
	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"BackGround.png",    // the file name
		D3DX_DEFAULT_NONPOW2,    // default width
		D3DX_DEFAULT_NONPOW2,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_background);    // load to sprite

		// UI 버튼
	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"BackGround.png",    // the file name
		D3DX_DEFAULT_NONPOW2,    // default width
		D3DX_DEFAULT_NONPOW2,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_background);    // load to sprite

	//베이스 기지
	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Base.png",    // the file name
		D3DX_DEFAULT_NONPOW2,    // default width
		D3DX_DEFAULT_NONPOW2,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_base);    // load to sprite

	//베이스 기지 공격
	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Base_Attack.png",    // the file name
		D3DX_DEFAULT_NONPOW2,    // default width
		D3DX_DEFAULT_NONPOW2,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_base_attack);    // load to sprite

	//베이스기지 총알
	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		L"Base_Shoot.png",    // the file name
		D3DX_DEFAULT_NONPOW2,    // default width
		D3DX_DEFAULT_NONPOW2,    // default height
		D3DX_DEFAULT,    // no mip mapping
		NULL,    // regular usage
		D3DFMT_A8R8G8B8,    // 32-bit pixels with alpha
		D3DPOOL_MANAGED,    // typical memory handling
		D3DX_DEFAULT,    // no filtering
		D3DX_DEFAULT,    // no mip filtering
		D3DCOLOR_XRGB(255, 0, 255),    // the hot-pink color key
		NULL,    // no image info struct
		NULL,    // not using 256 colors
		&sprite_base_shoot);    // load to sprite

	return;
}
 
void init_game(void)
{

}

void sound_run()
{
	//
}

void do_game_logic(void)
{

	//////////////////////////
		
	switch (gamestate)
	{
	case GameState::start_menu:

		if (KEY_DOWN(VK_RETURN) || KEY_DOWN(VK_SPACE)) // 스타트에서 엔터키 입력시 인게임으로 이동
		{
			gamestate = GameState::ingame;
			break;
		}
		else if (KEY_DOWN(VK_DOWN) || KEY_DOWN(VK_LEFT))
		{
			gamestate = GameState::end_menu;
		}

		break;
	case GameState::end_menu:

		 if (KEY_DOWN(VK_RETURN) || KEY_DOWN(VK_SPACE)) // 엔드에서 엔터키 입력시 종료
		{
			exit(0);
			break;
		}
		 else if (KEY_DOWN(VK_UP) || KEY_DOWN(VK_RIGHT))
		{
			gamestate = GameState::start_menu;
		}
		break;

	case GameState::ingame:

			if (KEY_DOWN(VK_UP))
				//hero.move(MOVE_UP);

			if (KEY_DOWN(VK_DOWN))
				//hero.move(MOVE_DOWN);

			if (KEY_DOWN(VK_LEFT))
			{
				//hero.move(MOVE_LEFT);
			}
			if (KEY_DOWN(VK_RIGHT))
			{
				//hero.move(MOVE_RIGHT);
			}
			if (KEY_DOWN(VK_SPACE))
			{
				if (CC.Fire == true)
				{
					CC.Fire = false;
				}
			}
		break;
	}
}
// this is the function used to render a single frame
void render_frame(void)
{
	// clear the window to a deep blue
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();    // begins the 3D scene

	d3dspt->Begin(D3DXSPRITE_ALPHABLEND);    // begin sprite drawing with transparency
	///////////////////////////////////
	//백그라운드

	switch (gamestate)
	{
	case GameState::start_menu://시작메뉴
	{
		RECT start_menu;
		SetRect(&start_menu, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		D3DXVECTOR3 start_menu_centor(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 start_menu_position(0, 0, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_start_menu, &start_menu, &start_menu_centor, &start_menu_position, D3DCOLOR_ARGB(255, 255, 255, 255));

		break;
	}
	case GameState::end_menu: //끝내기
	{
		RECT end_menu;
		SetRect(&end_menu, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		D3DXVECTOR3 end_menu_centor(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 end_menu_position(0, 0, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_end_menu, &end_menu, &end_menu_centor, &end_menu_position, D3DCOLOR_ARGB(255, 255, 255, 255));
		break;
	}
	case GameState::ingame: //인게임 
	{
		RECT background;
		SetRect(&background, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		D3DXVECTOR3 background_centor(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
		D3DXVECTOR3 background_position(0, 0, 0.0f);    // position at 50, 50 with no depth
		d3dspt->Draw(sprite_background, &background, &background_centor, &background_position, D3DCOLOR_ARGB(255, 255, 255, 255));

		//베이스기지

		if (CC.Fire == false)
		{

			//베이스 기지 공격
			RECT base;
			SetRect(&base, 0, 0, 150, 300);
			D3DXVECTOR3 base_centor(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 base_position(CC.Position_x, CC.Position_y, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_base_attack, &base, &base_centor, &base_position, D3DCOLOR_ARGB(255, 255, 255, 255));

			//베이스 기지 레이저 발사 공격

			RECT raser;
			SetRect(&raser, 70 * (CC.Fire_Frame - 1 / 10), 0, 70 * (CC.Fire_Frame / 10), 120);
			D3DXVECTOR3 raser_centor(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 raser_position(CC.Position_x - (85 * CC.Fire_Frame) / 10, CC.Position_y + 175, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_base_shoot, &raser, &raser_centor, &raser_position, D3DCOLOR_ARGB(255, 255, 255, 255));

			CC.Fire_Frame++;

			if (CC.Fire_Frame == 20)
			{
				CC.Fire_Frame = 0;
				CC.Fire = true;
			}

		}

		else // base_attack false;
		{
			//베이스 기지 일반
			RECT base;
			SetRect(&base, 0, 0, 150, 300);
			D3DXVECTOR3 base_centor(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
			D3DXVECTOR3 base_position(CC.Position_x, CC.Position_y, 0.0f);    // position at 50, 50 with no depth
			d3dspt->Draw(sprite_base, &background, &base_centor, &base_position, D3DCOLOR_ARGB(255, 255, 255, 255));
		}

		break;
	}

	}


	
	///////////////////////////////
	d3dspt->End();    // end sprite drawing

	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);

	return;
}


// this is the function that cleans up Direct3D and COM
void cleanD3D(void)
{
	d3ddev->Release();
	d3d->Release();

	//g_lpDSBG->Release();

	sprite_start_menu->Release();
	sprite_end_menu->Release();

	sprite_background->Release();
	//sprite_ui->Release();

	sprite_base->Release();
	sprite_base_attack->Release();
	sprite_base_shoot->Release();

	CC.~CommandCenter();

	//sound.DeleteDirectSound();

	return;
}