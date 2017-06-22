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
#include"Enemy.h"

// define the screen resolution and keyboard macros
#pragma warning(disable:4996)
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEY_UP(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)

#define STR_LEN 25
#define FRIEND_MAX 10
#define ENEMY_MAX 10

#define TEST_SPEED 1

// include the Direct3D Library file
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

// global declarations
LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;    // the pointer to the device class
LPD3DXSPRITE d3dspt;    // the pointer to our Direct3D Sprite interface
LPD3DXFONT font; // 폰트 선언

						//텍스쳐를 선언
LPDIRECT3DTEXTURE9 sprite_start_menu;
LPDIRECT3DTEXTURE9 sprite_end_menu;

LPDIRECT3DTEXTURE9 sprite_story;

LPDIRECT3DTEXTURE9 sprite_background_stage1;
LPDIRECT3DTEXTURE9 sprite_background_stage2;
LPDIRECT3DTEXTURE9 sprite_background_stage3;
LPDIRECT3DTEXTURE9 sprite_background_stage4;
LPDIRECT3DTEXTURE9 sprite_background_stage5;
LPDIRECT3DTEXTURE9 sprite_background_stage6;
LPDIRECT3DTEXTURE9 sprite_background_stage7;

LPDIRECT3DTEXTURE9 sprite_ui;
LPDIRECT3DTEXTURE9 sprite_ui_status;
LPDIRECT3DTEXTURE9 sprite_victory;
LPDIRECT3DTEXTURE9 sprite_ending;
LPDIRECT3DTEXTURE9 sprite_game_over;

LPDIRECT3DTEXTURE9 sprite_base;
LPDIRECT3DTEXTURE9 sprite_base_attack;
LPDIRECT3DTEXTURE9 sprite_base_shoot;

LPDIRECT3DTEXTURE9 sprite_cat_basic_atk;
LPDIRECT3DTEXTURE9 sprite_cat_basic_move;

LPDIRECT3DTEXTURE9 sprite_cat_tank_atk;
LPDIRECT3DTEXTURE9 sprite_cat_tank_move;

LPDIRECT3DTEXTURE9 sprite_cat_axe_atk;
LPDIRECT3DTEXTURE9 sprite_cat_axe_move;

LPDIRECT3DTEXTURE9 sprite_cat_angle_move;

LPDIRECT3DTEXTURE9 sprite_cat_cow_atk;
LPDIRECT3DTEXTURE9 sprite_cat_cow_move;

LPDIRECT3DTEXTURE9 sprite_cat_hero_atk;
LPDIRECT3DTEXTURE9 sprite_cat_hero_move;

LPDIRECT3DTEXTURE9 sprite_snake_atk;
LPDIRECT3DTEXTURE9 sprite_snake_move;

LPDIRECT3DTEXTURE9 sprite_usnake_atk;
LPDIRECT3DTEXTURE9 sprite_usnake_move;

LPDIRECT3DTEXTURE9 sprite_dog_atk;
LPDIRECT3DTEXTURE9 sprite_dog_move;

LPDIRECT3DTEXTURE9 sprite_udog_atk;
LPDIRECT3DTEXTURE9 sprite_udog_move;

LPDIRECT3DTEXTURE9 sprite_dog_dark_atk;
LPDIRECT3DTEXTURE9 sprite_dog_dark_move;

LPDIRECT3DTEXTURE9 sprite_bear_atk;
LPDIRECT3DTEXTURE9 sprite_bear_move;

LPDIRECT3DTEXTURE9 sprite_bear_dark_atk;
LPDIRECT3DTEXTURE9 sprite_bear_dark_move;

LPDIRECT3DTEXTURE9 sprite_bear_boss_move;

void initD3D(HWND hWnd);    // sets up and initializes Direct3D
void render_frame(void);    // renders a single frame
void cleanD3D(void);		// closes Direct3D and releases memory

void init_game(void);
void do_game_logic(void);

void Render_Draw(int x1, int y1, int x2, int y2, int pos_x, int pos_y, LPDIRECT3DTEXTURE9 name);
void Create_Texture(LPCWSTR filename, LPDIRECT3DTEXTURE9  *sprite_name);

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;

enum UnitState : int
{
	move, atk, die
};

enum GameState : int 
{
	start_menu, end_menu, ingame, game_over, game_ending
};

enum  GameStage : int
{
	stage1 = 1, stage2, stage3, stage4, stage5, stage6, stage7
};

enum FriendType : int
{
	basic = 1, tank, axe, angle, cow, hero
};

enum EnemyType : int
{
	dog = 1, snake, Udog, Usnake, bear,blackdog,blackbear
};


UnitState unit_state = UnitState::move;
GameState game_state = GameState::start_menu;
GameStage game_stage = GameStage::stage1;
FriendType friend_type;
EnemyType enemy_type;


CommandCenter cc;
Sound sound;
GameManager * GameManager::instance = nullptr;
GameManager * gm = GameManager::get_Instance();

Friend cat[FRIEND_MAX];
Enemy enemy[ENEMY_MAX];

void Render_Draw(int x1,int y1,int x2, int y2,int pos_x,int pos_y,LPDIRECT3DTEXTURE9 name)
{
	RECT rect;
	SetRect(&rect, x1, y1, x2, y2);
	D3DXVECTOR3 rect_center(0.0f, 0.0f, 0.0f);    // center at the upper-left corner
	D3DXVECTOR3 rect_position((float)pos_x, (float)pos_y, 0.0f);    // position at 50, 50 with no depth
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

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{

	lpCmdLine = lpCmdLine;
	hPrevInstance = hPrevInstance;
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

	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp,
		&d3ddev);

	//폰트 크리에이트
	Create_Font();

	//사운드 초기화
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
	Create_Texture(L"BackGround1.png", &sprite_background_stage1);
	Create_Texture(L"BackGround2.png", &sprite_background_stage2);
	Create_Texture(L"BackGround3.png", &sprite_background_stage3);
	Create_Texture(L"BackGround4.png", &sprite_background_stage4);
	Create_Texture(L"BackGround5.png", &sprite_background_stage5);
	Create_Texture(L"BackGround6.png", &sprite_background_stage6);
	Create_Texture(L"BackGround7.png", &sprite_background_stage7);

	// 라운드 승리 // 엔딩 // 패배
	Create_Texture(L"Victory.png", &sprite_victory);
	Create_Texture(L"Ending.png", &sprite_ending);
	Create_Texture(L"GameOver.png", &sprite_game_over);

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

	//Tank Cat
	Create_Texture(L"Tank_Cat_Atk.png", &sprite_cat_tank_atk);
	Create_Texture(L"Tank_Cat_Move.png", &sprite_cat_tank_move);

	//Axe Cat
	Create_Texture(L"Axe_Cat_Atk.png", &sprite_cat_axe_atk);
	Create_Texture(L"Axe_Cat_Move.png", &sprite_cat_axe_move);

	//Angle Cat
	Create_Texture(L"Angle_Cat_Move.png", &sprite_cat_angle_move);

	//Cow Cat
	Create_Texture(L"Cow_Cat_Atk.png", &sprite_cat_cow_atk);
	Create_Texture(L"Cow_Cat_Move.png", &sprite_cat_cow_move);

	//Hero Cat
	Create_Texture(L"Hero_Cat_Atk.png", &sprite_cat_hero_atk);
	Create_Texture(L"Hero_Cat_Move.png", &sprite_cat_hero_move);

	//Snake
	Create_Texture(L"Snake_Atk.png", &sprite_snake_atk);
	Create_Texture(L"Snake_Move.png", &sprite_snake_move);

	//Upgrade Snake
	Create_Texture(L"USnake_Atk.png", &sprite_usnake_atk);
	Create_Texture(L"USnake_Move.png", &sprite_usnake_move);

	//Dog
	Create_Texture(L"Dog_Atk.png", &sprite_dog_atk);
	Create_Texture(L"Dog_Move.png", &sprite_dog_move);

	//Upgrede Dog
	Create_Texture(L"UDog_Atk.png", &sprite_udog_atk);
	Create_Texture(L"UDog_Move.png", &sprite_udog_move);

	//Dark Dog
	Create_Texture(L"Dark_Dog_Atk.png", &sprite_dog_dark_atk);
	Create_Texture(L"Dark_Dog_Move.png", &sprite_dog_dark_move);

	//Bear
	Create_Texture(L"Bear_Atk.png", &sprite_bear_atk);
	Create_Texture(L"Bear_Move.png", &sprite_bear_move);

	//Dark Bear
	Create_Texture(L"Dark_Bear_Atk.png", &sprite_bear_dark_atk);
	Create_Texture(L"Dark_Bear_Move.png", &sprite_bear_dark_move);

	//Boss Bear
	Create_Texture(L"Boss_Bear_Move.png", &sprite_bear_boss_move);

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
		if (KEY_DOWN(0x31))
		{
			if (gm->cool_down < 0)
			{
				gm->cool_down = 30;

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					if (cat[i].active == false)
					{
						cat[i].active = true;
						cat[i].Unit_Init(cc.Position_x, cc.Position_y+240, 500, 5, 1, FriendType::basic);
						gm->unit_count++;
						break;
						//Hp 500, ATK 5, SPD 1
					}
				}
			}
		}

		//타입 버튼 2번 탱크 캣
		if (KEY_DOWN(0x32))
		{
			if (gm->cool_down < 0)
			{
				gm->cool_down = 30;

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					if (cat[i].active == false)
					{
						cat[i].active = true;
						cat[i].Unit_Init(cc.Position_x, cc.Position_y+180, 1500, 3, 1, FriendType::tank);
						gm->unit_count++;
						break;
						//Hp 1500, ATK 3, SPD 1
					}
				}
			}
		}
		//타입 버튼 3번 도끼 캣
		if (KEY_DOWN(0x33))
		{
			if (gm->cool_down < 0)
			{
				gm->cool_down = 30;

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					if (cat[i].active == false)
					{
						cat[i].active = true;
						cat[i].Unit_Init(cc.Position_x-30, cc.Position_y+220, 1000, 15, 1, FriendType::axe);
						gm->unit_count++;
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
					if (cat[i].active == false)
					{
						cat[i].active = true;
						cat[i].Unit_Init(cc.Position_x-30, cc.Position_y+220, 10, 1000, 2, FriendType::angle);
						gm->unit_count++;
						break;
						//Hp 10, ATK 500, SPD 2
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
					if (cat[i].active == false)
					{
						cat[i].active = true;
						cat[i].Unit_Init(cc.Position_x-30, cc.Position_y+200, 1000, 10, 3, FriendType::cow);
						gm->unit_count++;
						break;
						//Hp 1000, ATK 10, SPD 3
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
					if (cat[i].active == false)
					{
						cat[i].active = true;
						cat[i].Unit_Init(cc.Position_x-30, cc.Position_y+200, 2000, 50, 3, FriendType::hero);
						gm->unit_count++;
						break;
						//Hp 2000, ATK 200, SPD 3
					}
				}
			}
		}

		switch (game_stage)
		{
		case GameStage::stage1:

			if (gm->stage_ready)
			{
				for (int i = 0; i < ENEMY_MAX; i++)
				{
					if (enemy[i].active == false)
					{
						enemy[i].active = true;
						enemy[i].state = UnitState::move;
						enemy[i].Unit_Init((rand() % 1000) - 1000, cc.Position_y + 240, 500, 5, 1 + TEST_SPEED, EnemyType::snake);
						//뱀 Hp 1000, Atk 5, Speed 1
					}

					else
						continue;

					gm->unit_enemy_count = ENEMY_MAX;
				}
				gm->stage_ready = false;
			}

			if (gm->unit_enemy_count <= 0)
			{
				game_stage = GameStage::stage2;
				gm->current_game_stage++;
				gm->stage_ready = true;
				gm->victory_count = 0;

				for (int i = 0; i < ENEMY_MAX; i++)
				{
					gm->unit_enemy_count = 0;
					enemy[i].pos_x = (rand() % 1000) - 1000;
					enemy[i].active = false;
				}

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					gm->unit_count = 0;
					cat[i].pos_x = 900;
					cat[i].active = false;
				}
			}

			break;
		case GameStage::stage2:
			if (gm->stage_ready)
			{
				for (int i = 0; i < ENEMY_MAX; i++)
				{
					if (enemy[i].active == false)
					{
						enemy[i].active = true;
						enemy[i].state = UnitState::move;
						enemy[i].Unit_Init((rand() % 1000) - 1000, cc.Position_y + 240, 2000, 3, 1 + TEST_SPEED, EnemyType::dog);
						//개 Hp 2000, Atk 3, Speed 1
					}

					else
						continue;

					gm->unit_enemy_count = ENEMY_MAX;
				}
				gm->stage_ready = false;
			}

			if (gm->unit_enemy_count <= 0)
			{
				game_stage = GameStage::stage3;
				gm->current_game_stage++;
				gm->stage_ready = true;
				gm->victory_count = 0;

				for (int i = 0; i < ENEMY_MAX; i++)
				{
					gm->unit_enemy_count = 0;
					enemy[i].pos_x = (rand() % 1000) - 1000;
					enemy[i].active = false;
				}

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					gm->unit_count = 0;
					cat[i].pos_x = 900;
					cat[i].active = false;
				}
			}
			break;

		case GameStage::stage3:
			if (gm->stage_ready)
			{
				for (int i = 0; i < ENEMY_MAX; i++)
				{
					if (enemy[i].active == false)
					{
						enemy[i].active = true;
						enemy[i].state = UnitState::move;
						enemy[i].Unit_Init((rand() % 1000) - 1000, cc.Position_y + 215, 1500, 15, 1 + TEST_SPEED, EnemyType::Usnake);
						//강화 뱀 Hp 1500, Atk 15, Speed 1
					}

					else
						continue;

					gm->unit_enemy_count = ENEMY_MAX;
				}
				gm->stage_ready = false;
			}

			if (gm->unit_enemy_count <= 0)
			{
				game_stage = GameStage::stage4;
				gm->current_game_stage++;
				gm->stage_ready = true;
				gm->victory_count = 0;

				for (int i = 0; i < ENEMY_MAX; i++)
				{
					gm->unit_enemy_count = 0;
					enemy[i].pos_x = (rand() % 1000) - 1000;
					enemy[i].active = false;
				}

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					gm->unit_count = 0;
					cat[i].pos_x = 900;
					cat[i].active = false;
				}
			}
			break;

		case GameStage::stage4:
			if (gm->stage_ready)
			{
				for (int i = 0; i < ENEMY_MAX; i++)
				{
					if (enemy[i].active == false)
					{
						enemy[i].active = true;
						enemy[i].state = UnitState::move;
						enemy[i].Unit_Init((rand() % 1000) - 1000, cc.Position_y + 215, 2000, 20, 2 + TEST_SPEED, EnemyType::Udog);
						//강화 개 Hp 2000, Atk 20, Speed 2
					}

					else
						continue;

					gm->unit_enemy_count = ENEMY_MAX;
				}
				gm->stage_ready = false;
			}

			if (gm->unit_enemy_count <= 0)
			{
				game_stage = GameStage::stage5;
				gm->current_game_stage++;
				gm->stage_ready = true;
				gm->victory_count = 0;

				for (int i = 0; i < ENEMY_MAX; i++)
				{
					gm->unit_enemy_count = 0;
					enemy[i].pos_x = (rand() % 1000) - 1000;
					enemy[i].active = false;
				}

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					gm->unit_count = 0;
					cat[i].pos_x = 900;
					cat[i].active = false;
				}
			}
			break;

		case GameStage::stage5:
			if (gm->stage_ready)
			{
				for (int i = 0; i < ENEMY_MAX; i++)
				{
					if (enemy[i].active == false)
					{
						enemy[i].active = true;
						enemy[i].state = UnitState::move;
						enemy[i].Unit_Init((rand() % 1000) - 1000, cc.Position_y +140, 3000, 30, 1 + TEST_SPEED, EnemyType::bear);
						//곰 Hp 3000, Atk 30, Speed 1
					}

					else
						continue;

					gm->unit_enemy_count = ENEMY_MAX;
				}
				gm->stage_ready = false;
			}

			if (gm->unit_enemy_count <= 0)
			{
				game_stage = GameStage::stage6;
				gm->current_game_stage++;
				gm->stage_ready = true;
				gm->victory_count = 0;

				for (int i = 0; i < ENEMY_MAX; i++)
				{
					gm->unit_enemy_count = 0;
					enemy[i].pos_x = (rand() % 1000) - 1000;
					enemy[i].active = false;
				}

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					gm->unit_count = 0;
					cat[i].pos_x = 900;
					cat[i].active = false;
				}
			}
			break;

		case GameStage::stage6:
			if (gm->stage_ready)
			{
				for (int i = 0; i < ENEMY_MAX; i++)
				{
					if (enemy[i].active == false)
					{
						enemy[i].active = true;
						enemy[i].state = UnitState::move;
						enemy[i].Unit_Init((rand() % 1000) - 1000, cc.Position_y + 200, 2500, 30, 3 + TEST_SPEED, EnemyType::blackdog);
						//뱀 Hp 1000, Atk 5, Speed 1
					}

					else
						continue;

					gm->unit_enemy_count = ENEMY_MAX;
				}
				gm->stage_ready = false;
			}

			if (gm->unit_enemy_count <= 0)
			{
				game_stage = GameStage::stage7;
				gm->current_game_stage = 6;
				gm->stage_ready = true;
				gm->victory_count = 0;

				for (int i = 0; i < ENEMY_MAX; i++)
				{
					gm->unit_enemy_count = 0;
					enemy[i].pos_x = (rand() % 1000) - 1000;
					enemy[i].active = false;
				}

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					gm->unit_count = 0;
					cat[i].pos_x = 900;
					cat[i].active = false;
				}
			}
			break;

		case GameStage::stage7:
			if (gm->stage_ready)
			{
				for (int i = 0; i < ENEMY_MAX; i++)
				{
					if (enemy[i].active == false)
					{
						if (i == ENEMY_MAX - 1)
						{
							// 보스곰
							enemy[i].active = true;
							enemy[i].state = UnitState::move;
							enemy[i].Unit_Init(-2000, cc.Position_y-70, 10000, 100, 1 + TEST_SPEED, EnemyType::blackbear);
						}

						else
						{
							enemy[i].active = true;
							enemy[i].state = UnitState::move;
							enemy[i].Unit_Init((rand() % 1000) - 1000, cc.Position_y + 140, 3000, 100, 2 + TEST_SPEED, EnemyType::blackbear);
							//검은 곰 Hp 4000, Atk 100, Speed 3
						}
					}

					else
						continue;

					gm->unit_enemy_count = ENEMY_MAX;
				}
				gm->stage_ready = false;
			}

			if (gm->unit_enemy_count <= 0)
			{
				game_state = GameState::game_ending;

				gm->stage_ready = true;

				for (int i = 0; i < ENEMY_MAX; i++)
				{
					gm->unit_enemy_count = 0;
					enemy[i].pos_x = (rand() % 1000) - 1000;
					enemy[i].active = false;
				}

				for (int i = 0; i < FRIEND_MAX; i++)
				{
					gm->unit_count = 0;
					cat[i].active = false;
					cat[i].pos_x = 900;
				}
			}
			break;
		}

		//////////////////////////////////////////////스테이지 밖
		for (int i = 0; i < FRIEND_MAX; i++)
		{

			for (int j = 0; j < ENEMY_MAX; j++)
			{

					if (cat[i].pos_x - enemy[j].pos_x <= 60)
					{
						cat[i].hp -= enemy[j].atk_damage;
						enemy[j].hp -= cat[i].atk_damage;
						cat[i].state = UnitState::atk;
						enemy[j].state = UnitState::atk;

						if (cat[i].hp <= 0)
						{
							cat[i].pos_x = 900;
							cat[i].active = false;
							cat[i].state = UnitState::die;
							enemy[j].state = UnitState::move;
						}

						if (enemy[j].hp <= 0)
						{
							enemy[j].state = UnitState::die;
							enemy[j].active = false;
							enemy[j].pos_x = -100;
							cat[i].state = UnitState::move;
						}

						break;
					}

					else
					{
						cat[i].state = UnitState::move;
					}

			}

			for (int j = 0; j < FRIEND_MAX; j++)
			{

				if (-enemy[i].pos_x + cat[j].pos_x <= 60)
				{
					enemy[i].hp -= cat[j].atk_damage;
					cat[j].hp -= enemy[i].atk_damage;
					enemy[i].state = UnitState::atk;
					cat[j].state = UnitState::atk;

					if (enemy[i].hp <= 0)
					{
						enemy[i].pos_x = 900;
						enemy[i].active = false;
						enemy[i].state = UnitState::die;
						cat[j].state = UnitState::move;
					}

					if (cat[j].hp <= 0)
					{
						cat[j].state = UnitState::die;
						cat[j].active = false;
						cat[j].pos_x = -100;
						enemy[i].state = UnitState::move;
					}

					break;
				}

				else
				{
					enemy[i].state = UnitState::move;
				}

			}

		}

		gm->active_unit_count = 0;
		gm->active_enemy_unit_count = 0;
		for (int i = 0; i < FRIEND_MAX; i++)
		{

			if (enemy[i].active == true)
			{
				if (enemy[i].state == UnitState::move && enemy[i].pos_x < 750)
					enemy[i].Unit_Move();

				if (enemy[i].pos_x >= 750)
				{
					game_state = GameState::game_over;
				}
				

				gm->active_enemy_unit_count++;
			}

			if (cat[i].active == true)
			{
				if (cat[i].state == UnitState::move && cat[i].pos_x > 50)
					cat[i].Unit_Move();

				gm->active_unit_count++;
			}

		}

		gm->unit_count = gm->active_unit_count;
		gm->unit_enemy_count = gm->active_enemy_unit_count;

		///////////////////

		if (KEY_DOWN(VK_SPACE))
		{
			if (gm->energy_percent == 100)
			{
				cc.Fire = false;
				gm->energy_percent = 0;

				for (int i = 0; i < ENEMY_MAX; i++)
				{
					if (enemy[i].pos_x >= 10)
					{
						enemy[i].hp = -1000;

						if (enemy[i].hp <= 0)
						{
							enemy[i].active = false;
							enemy[i].state = die;
							enemy[i].pos_x = -100;
						}
					}
				}
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

		switch (game_stage)
		{
			case GameStage::stage1:
				Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_background_stage1);
			break;
			case GameStage::stage2:
				Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_background_stage2);
			break;
			case GameStage::stage3:
				Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_background_stage3);
			break;
			case GameStage::stage4:
				Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_background_stage4);
			break;
			case GameStage::stage5:
				Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_background_stage5);
			break;
			case GameStage::stage6:
				Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_background_stage6);
			break;
			case GameStage::stage7:
				Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_background_stage7);
			break;
		}
	

		//UI버튼
		Render_Draw(0,120*(gm->current_game_stage - 1), 800, 120 * gm->current_game_stage, 0, 480, sprite_ui);

		//맨위 UI
		Render_Draw(0, 0, 800, 60, 0, 0, sprite_ui_status);

		sprintf(gm->unit_str, "아군병력MAX %d / 10        ",gm->unit_count);
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

		if (cc.Fire == false)
		{
			//베이스 기지 공격
			Render_Draw(0, 0, 150, 300, cc.Position_x, cc.Position_y, sprite_base_attack);

			//베이스 기지 레이저 발사 공격
			Render_Draw(70 * (cc.Fire_Frame - 1 / 10), 0, 70 * (cc.Fire_Frame / 10), 120, cc.Position_x - (85 * cc.Fire_Frame) / 10, cc.Position_y + 175, sprite_base_shoot);

			cc.Fire_Frame++;

			if (cc.Fire_Frame == 20)
			{
				cc.Fire_Frame = 0;
				cc.Fire = true;
			}

		}

		else // base_attack false;
		{
			//베이스 기지 일반
			Render_Draw(0, 0, 150, 300, cc.Position_x, cc.Position_y, sprite_base);
		}

		//타입별 드로우
		for (int i = 0; i < FRIEND_MAX; i++)
		{
			if (cat[i].active == true)
			{

				switch(cat[i].type)
				{ 
				case FriendType::basic: // 베이직 캣 타입

					switch (cat[i].state)
					{
					case UnitState::move:
						Render_Draw(50 * (cat[i].frame / 5), 0, 50 * ((cat[i].frame / 5) + 1), 60, cat[i].pos_x, cat[i].pos_y, sprite_cat_basic_move);

						cat[i].frame++;

						if (cat[i].frame >= 15)
							cat[i].frame = 0;
						break;
					case UnitState::atk:
						Render_Draw(50 * (cat[i].frame / 5), 0, 50 * ((cat[i].frame / 5) + 1), 60, cat[i].pos_x, cat[i].pos_y, sprite_cat_basic_atk);

						cat[i].frame++;

						if (cat[i].frame >= 20)
							cat[i].frame = 0;
						break;
					case UnitState::die:
						cat[i].active = false;
						break;
					}

					break;
				case FriendType::tank: // 탱크 캣 타입
					switch (cat[i].state)
					{
						case UnitState::move:
							Render_Draw(50 * (cat[i].frame / 5), 0, 50 * ((cat[i].frame / 5) + 1), 120, cat[i].pos_x, cat[i].pos_y, sprite_cat_tank_move);

							cat[i].frame++;

							if (cat[i].frame >= 15)
								cat[i].frame = 0;
							break;
						case UnitState::atk:
							Render_Draw(100 * (cat[i].frame / 5), 0, 100 * ((cat[i].frame / 5) + 1), 120, cat[i].pos_x, cat[i].pos_y, sprite_cat_tank_atk);

							cat[i].frame++;

							if (cat[i].frame >= 20)
								cat[i].frame = 0;
							break;
						case UnitState::die:
							cat[i].active = false;
							break;
					}
					break;
				case FriendType::axe: // 액스 캣 타입
					switch (cat[i].state)
					{
					case UnitState::move:
						Render_Draw(100 * (cat[i].frame / 5), 0, 100 * ((cat[i].frame / 5) + 1), 80, cat[i].pos_x, cat[i].pos_y, sprite_cat_axe_move);

						cat[i].frame++;

						if (cat[i].frame >= 15)
							cat[i].frame = 0;

						break;
					case UnitState::atk:
						Render_Draw(120 * (cat[i].frame / 5), 0, 120 * ((cat[i].frame / 5) + 1), 80, cat[i].pos_x, cat[i].pos_y, sprite_cat_axe_atk);

						cat[i].frame++;

						if (cat[i].frame >= 20)
							cat[i].frame = 0;

						break;
					case UnitState::die:
						cat[i].active = false;
						break;
					}
					break;
				case FriendType::angle: // 엔젤 캣 타입
					switch (cat[i].state)
					{
					case UnitState::move:
						Render_Draw(125 * (cat[i].frame / 5), 0, 125 * ((cat[i].frame / 5) + 1), 75, cat[i].pos_x, cat[i].pos_y, sprite_cat_angle_move);

						cat[i].frame++;

						if (cat[i].frame >= 20)
							cat[i].frame = 0;
						break;
					case UnitState::atk:
						Render_Draw(125 * (cat[i].frame / 5), 0, 125 * ((cat[i].frame / 5) + 1), 75, cat[i].pos_x, cat[i].pos_y, sprite_cat_angle_move);

						cat[i].frame++;

						if (cat[i].frame >= 20)
							cat[i].frame = 0;
						break;
					case UnitState::die:
						cat[i].active = false;
						break;
					}
					break;
				case FriendType::cow: // 카우 캣 타입
					switch (cat[i].state)
					{
					case UnitState::move:
						Render_Draw(100 * (cat[i].frame / 5), 0, 100 * ((cat[i].frame / 5) + 1), 100, cat[i].pos_x, cat[i].pos_y, sprite_cat_cow_move);

						cat[i].frame++;

						if (cat[i].frame >= 20)
							cat[i].frame = 0;
						break;
					case UnitState::atk:
						Render_Draw(150 * (cat[i].frame / 5), 0, 150 * ((cat[i].frame / 5) + 1), 100, cat[i].pos_x, cat[i].pos_y, sprite_cat_cow_atk);

						cat[i].frame++;

						if (cat[i].frame >= 20)
							cat[i].frame = 0;

						break;
					case UnitState::die:
						cat[i].active = false;
						break;
					}
					break;
				case FriendType::hero: // 히어로 캣 타입
					switch (cat[i].state)
					{
					case UnitState::move:
						Render_Draw(100 * (cat[i].frame / 5), 0, 100 * ((cat[i].frame / 5) + 1), 100, cat[i].pos_x, cat[i].pos_y, sprite_cat_hero_move);

						cat[i].frame++;

						if (cat[i].frame >= 15)
							cat[i].frame = 0;
						break;
					case UnitState::atk:
						Render_Draw(150 * (cat[i].frame / 5), 0, 150 * ((cat[i].frame / 5) + 1), 100, cat[i].pos_x, cat[i].pos_y, sprite_cat_hero_atk);

						cat[i].frame++;

						if (cat[i].frame >= 20)
							cat[i].frame = 0;
						break;
					case UnitState::die:
						cat[i].active = false;
						break;
					}
					break;
				}
			}

			else
				continue;
		}

		//스테이지 관련 드로우

		switch (game_stage)
		{
		case GameStage::stage1:
			for(int i = 0; i < ENEMY_MAX; i++)
			{
				switch (enemy[i].state)
				{
				case UnitState::move:
					Render_Draw(80 * (enemy[i].frame / 5), 0, 80 * ((enemy[i].frame / 5) + 1), 60, enemy[i].pos_x, enemy[i].pos_y, sprite_snake_move);

					enemy[i].frame++;

					if (enemy[i].frame >= 20)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::atk:
					Render_Draw(80 * (enemy[i].frame / 5), 0, 80 * ((enemy[i].frame / 5) + 1), 60, enemy[i].pos_x, enemy[i].pos_y, sprite_snake_atk);
					
					enemy[i].frame++;

					if (enemy[i].frame >= 20)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::die:
					enemy[i].active = false;
					break;

				}

			} 
			break;
		case GameStage::stage2:

			gm->victory_count++;

			if (gm->victory_count <= 20)
			{
				Render_Draw(0, 0, 400, 200, 200, 200, sprite_victory);
			}

			for (int i = 0; i < ENEMY_MAX; i++)
			{
				switch (enemy[i].state)
				{
				case UnitState::move:
					Render_Draw(55 * (enemy[i].frame / 5), 0, 55 * ((enemy[i].frame / 5) + 1), 60, enemy[i].pos_x, enemy[i].pos_y, sprite_dog_move);

					enemy[i].frame++;

					if (enemy[i].frame >= 15)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::atk:
					Render_Draw(55 * (enemy[i].frame / 5), 0, 55 * ((enemy[i].frame / 5) + 1), 60, enemy[i].pos_x, enemy[i].pos_y, sprite_dog_atk);

					enemy[i].frame++;

					if (enemy[i].frame >= 20)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::die:
					enemy[i].active = false;
					break;
				}

			}
			break;

		case GameStage::stage3:

			gm->victory_count++;

			if (gm->victory_count <= 20)
			{
				Render_Draw(0, 0, 400, 200, 200, 200, sprite_victory);
			}

			for (int i = 0; i < ENEMY_MAX; i++)
			{
				switch (enemy[i].state)
				{
				case UnitState::move:
					Render_Draw(80 * (enemy[i].frame / 5), 0, 80 * ((enemy[i].frame / 5) + 1), 80, enemy[i].pos_x, enemy[i].pos_y, sprite_usnake_move);

					enemy[i].frame++;

					if (enemy[i].frame >= 20)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::atk:
					Render_Draw(80 * (enemy[i].frame / 5), 0, 80 * ((enemy[i].frame / 5) + 1), 80, enemy[i].pos_x, enemy[i].pos_y, sprite_usnake_atk);

					enemy[i].frame++;

					if (enemy[i].frame >= 20)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::die:
					enemy[i].active = false;
					break;

				}

			}
			break;

		case GameStage::stage4:

			gm->victory_count++;

			if (gm->victory_count <= 20)
			{
				Render_Draw(0, 0, 400, 200, 200, 200, sprite_victory);
			}

			for (int i = 0; i < ENEMY_MAX; i++)
			{
				switch (enemy[i].state)
				{
				case UnitState::move:
					Render_Draw(65 * (enemy[i].frame / 5), 0, 65 * ((enemy[i].frame / 5) + 1), 80, enemy[i].pos_x, enemy[i].pos_y, sprite_udog_move);

					enemy[i].frame++;

					if (enemy[i].frame >= 15)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::atk:
					Render_Draw(65 * (enemy[i].frame / 5), 0, 65 * ((enemy[i].frame / 5) + 1), 80, enemy[i].pos_x, enemy[i].pos_y, sprite_udog_atk);

					enemy[i].frame++;

					if (enemy[i].frame >= 20)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::die:
					enemy[i].active = false;
					break;

				}

			}
			break;

		case GameStage::stage5:

			gm->victory_count++;

			if (gm->victory_count <= 20)
			{
				Render_Draw(0, 0, 400, 200, 200, 200, sprite_victory);
			}

			for (int i = 0; i < ENEMY_MAX; i++)
			{
				switch (enemy[i].state)
				{
				case UnitState::move:
					Render_Draw(100 * (enemy[i].frame / 5), 0, 100 * ((enemy[i].frame / 5) + 1), 150, enemy[i].pos_x, enemy[i].pos_y, sprite_bear_move);

					enemy[i].frame++;

					if (enemy[i].frame >= 30)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::atk:
					Render_Draw(150 * (enemy[i].frame / 5), 0, 150 * ((enemy[i].frame / 5) + 1), 200, enemy[i].pos_x, enemy[i].pos_y, sprite_bear_atk);

					enemy[i].frame++;

					if (enemy[i].frame >= 20)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::die:
					enemy[i].active = false;
					break;

				}

			}
			break;

		case GameStage::stage6:

			gm->victory_count++;

			if (gm->victory_count <= 20)
			{
				Render_Draw(0, 0, 400, 200, 200, 200, sprite_victory);
			}

			for (int i = 0; i < ENEMY_MAX; i++)
			{
				switch (enemy[i].state)
				{
				case UnitState::move:
					Render_Draw(100 * (enemy[i].frame / 5), 0, 100 * ((enemy[i].frame / 5) + 1), 100, enemy[i].pos_x, enemy[i].pos_y, sprite_dog_dark_move);

					enemy[i].frame++;

					if (enemy[i].frame >= 15)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::atk:
					Render_Draw(100 * (enemy[i].frame / 5), 0, 100 * ((enemy[i].frame / 5) + 1), 100, enemy[i].pos_x, enemy[i].pos_y, sprite_dog_dark_atk);

					enemy[i].frame++;

					if (enemy[i].frame >= 20)
					{
						enemy[i].frame = 0;
					}
					break;
				case UnitState::die:
					enemy[i].active = false;
					break;

				}

			}

			break;

		case GameStage::stage7:

			gm->victory_count++;

			if (gm->victory_count <= 20)
			{
				Render_Draw(0, 0, 400, 200, 200, 200, sprite_victory);
			}

			for (int i = 0; i < ENEMY_MAX; i++)
			{
				switch (enemy[i].state)
				{
				case UnitState::move:

					if (i == ENEMY_MAX - 1)
					{
						Render_Draw(200 * (enemy[i].frame / 5), 0, 200 * ((enemy[i].frame / 5) + 1), 400, enemy[i].pos_x, enemy[i].pos_y, sprite_bear_boss_move);
					
						enemy[i].frame++;

						if (enemy[i].frame >= 30)
						{
							enemy[i].frame = 0;
						}
					}

					else
					{
						Render_Draw(100 * (enemy[i].frame / 5), 0, 100 * ((enemy[i].frame / 5) + 1), 150, enemy[i].pos_x, enemy[i].pos_y, sprite_bear_dark_move);

						enemy[i].frame++;

						if (enemy[i].frame >= 30)
						{
							enemy[i].frame = 0;
						}
					}
					break;
				case UnitState::atk:
					if (i == ENEMY_MAX - 1)
					{
						Render_Draw(200 * (enemy[i].frame / 5), 0, 200 * ((enemy[i].frame / 5) + 1), 400, enemy[i].pos_x, enemy[i].pos_y, sprite_bear_boss_move);

						enemy[i].frame++;

						if (enemy[i].frame >= 30)
						{
							enemy[i].frame = 0;
						}
					}

					else
					{
						Render_Draw(150 * (enemy[i].frame / 5), 0, 150 * ((enemy[i].frame / 5) + 1), 200, enemy[i].pos_x, enemy[i].pos_y, sprite_bear_dark_atk);

						enemy[i].frame++;

						if (enemy[i].frame >= 20)
						{
							enemy[i].frame = 0;
						}
					}
					break;
				case UnitState::die:
					enemy[i].active = false;
					break;

				}

			}
			break;
		}

		//////////////////////////// 여기까지 ingame
		break;

		case GameState::game_over: // 게임 오버
			gm->ending_count++;
			if (gm->ending_count <= 150)
			{
				Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, sprite_game_over);
			}

			else
			{
				exit(0);
			}
			break;
		case GameState::game_ending: // 게임 엔딩
			gm->ending_count++;
			if (gm->ending_count <= 150)
			{
				Render_Draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0,0,sprite_ending);
			}

			else
			{
				exit(0);
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

	//메뉴
	sprite_start_menu->Release();
	sprite_end_menu->Release();

	sprite_story->Release();

	//스테이지
	sprite_background_stage1->Release();
	sprite_background_stage2->Release();
	sprite_background_stage3->Release();
	sprite_background_stage4->Release();
	sprite_background_stage5->Release();
	sprite_background_stage6->Release();
	sprite_background_stage7->Release();

	//UI
	sprite_ui->Release();
	sprite_ui_status->Release();

	//엔딩 & 오버
	sprite_victory->Release();
	sprite_ending->Release();
	sprite_game_over->Release();

	//기지
	sprite_base->Release();
	sprite_base_attack->Release();
	sprite_base_shoot->Release();


	//아군유닛
	sprite_cat_basic_atk->Release();
	sprite_cat_basic_move->Release();

	sprite_cat_tank_atk->Release();
	sprite_cat_tank_move->Release();

	sprite_cat_axe_atk->Release();
	sprite_cat_axe_move->Release();

	sprite_cat_angle_move->Release();

	sprite_cat_cow_atk->Release();
	sprite_cat_cow_move->Release();

	sprite_cat_hero_atk->Release();
	sprite_cat_hero_move->Release();

	////////////////////적들 유닛

	sprite_snake_atk->Release();
	sprite_snake_move->Release();

	sprite_usnake_atk->Release();
	sprite_usnake_move->Release();

	sprite_dog_atk->Release();
	sprite_dog_move->Release();

	sprite_udog_atk->Release();
	sprite_udog_move->Release();

	sprite_dog_dark_atk->Release();
	sprite_dog_dark_atk->Release();

	sprite_bear_atk->Release();
	sprite_bear_move->Release();

	sprite_bear_dark_atk->Release();
	sprite_bear_dark_move->Release();

	sprite_bear_boss_move->Release();

	//사운드 및 폰트
	sound.ReleaseDSound();
	font->Release();

	return;
}