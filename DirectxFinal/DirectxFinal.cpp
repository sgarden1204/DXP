// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>

#include"CommandCenter.h"
#include"Sound.h"
#include"GameManager.h"
#include"Friend.h"

// define the screen resolution and keyboard macros
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define STR_LEN 25
#define FRIEND_MAX 10

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface
LPDIRECTSOUNDBUFFER g_lpDSBG[2] = { NULL, };

LPDIRECTSOUND g_lpDS = NULL;

LPD3DXFONT font;

						//스프라이트 선언

						//텍스쳐를 선언
LPDIRECT3DTEXTURE9 sprite_start_menu;
LPDIRECT3DTEXTURE9 sprite_end_menu;

LPDIRECT3DTEXTURE9 sprite_story;
LPDIRECT3DTEXTURE9 sprite_background;
LPDIRECT3DTEXTURE9 sprite_ui;
LPDIRECT3DTEXTURE9 sprite_ui_status;

LPDIRECT3DTEXTURE9 sprite_base;
LPDIRECT3DTEXTURE9 sprite_base_attack;
LPDIRECT3DTEXTURE9 sprite_base_shoot;

LPDIRECT3DTEXTURE9 sprite_cat_basic_atk;
LPDIRECT3DTEXTURE9 sprite_cat_basic_move;

void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory

void init_game(void);
void do_game_logic(void);

void Render_Draw(int x1, int y1, int x2, int y2, int pos_x, int pos_y, LPDIRECT3DTEXTURE9 name);
void Create_Texture(LPCWSTR filename, LPDIRECT3DTEXTURE9  *sprite_name);

bool sphere_collision_check(float x0, float y0, float size0, float x1, float y1, float size1);

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


using namespace std;
//////////////////////////////////////////////////

enum Test
{
	ap, ape
};

enum GameState : int 
{
	start_menu, end_menu, ingame
};

enum class GameStage : int
{
	stage1, stage2, stage3, stage4, stage5, stage6, stage7
};

enum class FriendType : int
{
	basic, tank, axe, angle, cow, hero
};

enum class EnemyType : int
{
	dog, snake, Udog, Usnake, bear,blackdog,blackbear
};

enum class UnitState : int
{
	move, atk, die
};

GameState game_state = GameState::start_menu;
GameStage game_stage = GameStage::stage1;
FriendType friend_type;
EnemyType enemy_type;
UnitState unit_state = UnitState::move;


CommandCenter CC;
Sound sound;
GameManager * GameManager::instance = nullptr;
GameManager * gm = GameManager::get_Instance();

Friend Cat[FRIEND_MAX];

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

bool Rect_collision_check(int left1, int top1, int right1, int bottom1, int left2, int top2, int right2, int bottom2)
{
	if (right2 >= left1 && right1 >= left2 && bottom2 >= top1 && bottom1 >= top2)
		return true;
	else
		return false;
}

void Render_Draw(int x1,int y1,int x2, int y2,int pos_x,int pos_y,LPDIRECT3DTEXTURE9 name)
{
	RECT rect;
	SetRect(&rect, x1, y1, x2, y2);
	D3DXVECTOR3 rect_center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 rect_position(pos_x, pos_y, 0.0f);    // position at 50, 50 with no depth
	d3dspt->Draw(name, &rect, &rect_center, &rect_position, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void Create_Texture(LPCWSTR filename,LPDIRECT3DTEXTURE9 * sprite_name)
{
	D3DXCreateTextureFromFileEx(d3ddev,    // the device pointer
		filename,    // the file name
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
		sprite_name);    // load to sprite
}

void Create_Font()
{
	D3DXCreateFont(d3ddev,
		30, 0,
		FW_NORMAL,
		1,
		FALSE,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH || FF_DONTCARE,
		L"Arial",
		&font);
}

void Play_Sound()
{
	sound.PlayWave();
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


	//폰트 크리에이트
	Create_Font();

	sound.DSoundInit(hWnd);
	sound.LoadWave(L"BGM1.wav");

	// create the Direct3D Sprite object
										  //스프라이트 생성
										  //이 이후로 텍스쳐를 생성한다
	D3DXCreateSprite(d3ddev, &d3dspt);

	//게임 시작 메뉴
	Create_Texture(L"GameStart.png", &sprite_start_menu);

	//게임 끝내기 메뉴
	Create_Texture(L"GameEnd.png", &sprite_end_menu);

	//게임 스토리
	Create_Texture(L"Story.png", &sprite_story);

	// 백그라운드
	Create_Texture(L"BackGround.png", &sprite_background);

		// UI
	Create_Texture(L"UI.png", &sprite_ui);

	// 맨위 UI 상태창 
	Create_Texture(L"UI2.png", &sprite_ui_status);

	//베이스 기지
	Create_Texture(L"Base.png", &sprite_base);

	//베이스 기지 공격
	Create_Texture(L"Base_Attack.png", &sprite_base_attack);

	//베이스기지 총알
	Create_Texture(L"Base_Shoot.png", &sprite_base_shoot);

	//Basic Cat
	Create_Texture(L"Basic_Cat_Atk.png", &sprite_cat_basic_atk);
	Create_Texture(L"Basic_Cat_Move.png", &sprite_cat_basic_move);

	return;
}
 
void init_game(void)
{
	gm->Init();
}

void do_game_logic(void)
{

	//////////////////////////
		
	switch (game_state)
	{
	case GameState::start_menu:

		if (KEY_DOWN(VK_RETURN)) // 스타트에서 엔터키 입력시 인게임으로 이동
		{
			game_state = GameState::ingame;
			break;
		}
		else if (KEY_DOWN(VK_DOWN) || KEY_DOWN(VK_LEFT))
		{
			game_state = GameState::end_menu;
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
			game_state = GameState::start_menu;
		}
		break;

	case GameState::ingame:
		
		gm->cool_down--;

		//타입 버튼 1번 기본 캣
		if (KEY_DOWN(0x31)) // basic_cat_setting & active
		{
			if (gm->cool_down < 0)
			{
				gm->cool_down = 30;

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					if (Cat[i].active == false)
					{
						Cat[i].active = true;
						Cat[i].Unit_Init(CC.Position_x, CC.Position_y, 10, 5, 1,static_cast<int>(FriendType::basic));
						break;
						//Hp 10, ATK 5, SPD 1
					}
				}
			}
		}

		//타입 버튼 2번 탱크 캣
		if (KEY_DOWN(0x32)) // basic_cat_setting & active
		{
			if (gm->cool_down < 0)
			{
				gm->cool_down = 30;

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					if (Cat[i].active == false)
					{
						Cat[i].active = true;
						Cat[i].Unit_Init(CC.Position_x, CC.Position_y, 30, 3, 1, static_cast<int>(FriendType::tank));
						break;
						//Hp 30, ATK 3, SPD 1
					}
				}
			}
		}
		//타입 버튼 3번 도끼 캣
		if (KEY_DOWN(0x33)) // basic_cat_setting & active
		{
			if (gm->cool_down < 0)
			{
				gm->cool_down = 30;

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					if (Cat[i].active == false)
					{
						Cat[i].active = true;
						Cat[i].Unit_Init(CC.Position_x, CC.Position_y, 10, 15, 1, static_cast<int>(FriendType::axe));
						break;
						//Hp 10, ATK 15, SPD 1
					}
				}
			}
		}

		//타입 버튼4번 천사 캣
		if (KEY_DOWN(0x34)) // basic_cat_setting & active
		{
			if (gm->cool_down < 0)
			{
				gm->cool_down = 30;

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					if (Cat[i].active == false)
					{
						Cat[i].active = true;
						Cat[i].Unit_Init(CC.Position_x, CC.Position_y, 10, 1, 2, static_cast<int>(FriendType::angle));
						break;
						//Hp 10, ATK 1, SPD 2
					}
				}
			}
		}

		//버튼 타입 5 소 캣
		if (KEY_DOWN(0x35)) // basic_cat_setting & active
		{
			if (gm->cool_down < 0)
			{
				gm->cool_down = 30;

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					if (Cat[i].active == false)
					{
						Cat[i].active = true;
						Cat[i].Unit_Init(CC.Position_x, CC.Position_y, 10, 5, 3, static_cast<int>(FriendType::cow));
						break;
						//Hp 10, ATK 5, SPD 3
					}
				}
			}
		}

		//버튼 타입 6 히어로 캣
		if (KEY_DOWN(0x36)) // basic_cat_setting & active
		{
			if (gm->cool_down < 0)
			{
				gm->cool_down = 30;

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					if (Cat[i].active == false)
					{
						Cat[i].active = true;
						Cat[i].Unit_Init(CC.Position_x, CC.Position_y, 20, 10, 2, static_cast<int>(FriendType::hero));
						break;
						//Hp 20, ATK 10, SPD 2
					}
				}
			}
		}



		for (int i = 0; i < FRIEND_MAX; i++)
		{
			if (Cat[i].active == true)
			{
				if(Cat[i].pos_x < 800 && Cat[i].pos_x > 100 )
					Cat[i].Unit_Move();
				//여기서 내일 적들과 처리 해야됨
			}
		}

		if (KEY_DOWN(VK_SPACE))
		{
			if (gm->energy_percent == 100)
			{
				CC.Fire = false;
				gm->energy_percent = 0;
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


	switch (game_state)
	{
	case GameState::start_menu://시작메뉴
	{
		if (gm->wait_music < 1)
		{
			sound.PlayWave();
			gm->wait_music++;
			break;
		}
		
		Render_Draw(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,0,0,sprite_start_menu);
		break;
	}
	case GameState::end_menu: //끝내기
	{
		Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_end_menu);
		break;
	}
	case GameState::ingame: //인게임 
	{
		//스토리 설명
		if (gm->wait_count < 80)
		{
			gm->wait_count++;
			Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_story);
			break;
			
		}
		//백 그라운드
		Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_background);

		//UI버튼
		Render_Draw(0, gm->current_game_stage * 120, 800, 120, 0, 480, sprite_ui);

		//맨위 UI
		Render_Draw(0, 0, 800, 60, 0, 0, sprite_ui_status);

		sprintf(gm->unit_str, "아군병력MAX %d / 30        ",gm->unit_count);
		font->DrawTextA(d3dspt,gm->unit_str,STR_LEN,&gm->unit_rect,DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));

		sprintf(gm->unit_enemy_str, "남은 적군수 : %d          ", gm->unit_enemy_count);
		font->DrawTextA(d3dspt, gm->unit_enemy_str, STR_LEN, &gm->unit_enemy_rect, DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));

		sprintf(gm->energy_str, "충전게이지 : %d           ", gm->energy_percent);
		font->DrawTextA(d3dspt, gm->energy_str, STR_LEN, &gm->energy_rect, DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));

		//베이스기지
		gm->wait_energy++;
		if (gm->wait_energy == 5)
		{
			gm->wait_energy = 0;
			gm->energy_percent++;
			if (gm->energy_percent > 100)
				gm->energy_percent = 100;
		}

		if (CC.Fire == false)
		{
			//베이스 기지 공격
			Render_Draw(0, 0, 150, 300, CC.Position_x, CC.Position_y, sprite_base_attack);

			//베이스 기지 레이저 발사 공격
			Render_Draw(70 * (CC.Fire_Frame - 1 / 10), 0, 70 * (CC.Fire_Frame / 10), 120, CC.Position_x - (85 * CC.Fire_Frame) / 10, CC.Position_y + 175, sprite_base_shoot);

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
			Render_Draw(0, 0, 150, 300, CC.Position_x, CC.Position_y, sprite_base);
		}

		//타입별 드로우
		for (int i = 0; i < FRIEND_MAX; i++)
		{
			if (Cat[i].active == true)
			{
				switch(friend_type)
				{ 
				case FriendType::basic:
					Render_Draw(50 * (Cat[i].frame / 5), 0, 50 * ((Cat[i].frame / 5) + 1), 60, Cat[i].pos_x - Cat[i].move_speed, Cat[i].pos_y, sprite_cat_basic_move);

					Cat[i].frame++;

					if (Cat[i].frame >= 15)
						Cat[i].frame = 0;

					break;
				case FriendType::tank:
					break;
				case FriendType::axe:
					break;
				case FriendType::angle:
					break;
				case FriendType::cow:
					break;
				case FriendType::hero:
					break;
				}
			}

			else
				continue;
		}

		//////////////////////////// 여기까지 ingame
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

	sprite_start_menu->Release();
	sprite_end_menu->Release();

	sprite_story->Release();
	sprite_background->Release();
	sprite_ui->Release();
	sprite_ui_status->Release();

	sprite_base->Release();
	sprite_base_attack->Release();
	sprite_base_shoot->Release();

	sprite_cat_basic_atk->Release();
	sprite_cat_basic_move->Release();

	sound.ReleaseDSound();
	font->Release();

	return;
}