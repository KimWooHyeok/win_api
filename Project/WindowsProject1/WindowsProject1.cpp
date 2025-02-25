// WindowsProject1.cpp: 응용 프로그램의 진입점을 정의합니다.
//
#include "stdafx.h"
#include <list>
#include<Windows.h>
#include<ObjIdl.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#include "WindowsProject1.h"
#include "resource.h"
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

WCHAR lpszText[10] = L"TEXT";

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

BOOL isRnder = FALSE;

//WORD DIR = 0; // 0 : NONE , 1: RIGHT, 2 : LEFT, 3: DOWN, 4 : UP
HDC DC;
HDC DC2;


void DrawFlip(Image* image, int x, int y, int cx, int cy, int px, int py, HDC DC, bool isFlip, int x2)
{
	
	Graphics graphics(DC);

	if (isFlip)
	{
		x = x - x2;
		y;
		cx = 400 - cx - px;
		cy;
		px;
		py;
	}
	graphics.DrawImage(image, x, y, cx, cy, px, py, Gdiplus::UnitPixel);
}


class Missile
{
public:
	int posX, posY;
	int DIR;
	int speed;
	int delay;
	int width;
	bool isActive = false;

	Missile(int x, int y, int dir, int spd, int delay)
	{
		posX = x;
		posY = y;
		DIR = dir;
		speed = spd;
		this->delay = delay;
		width = 5;
	}

	void Update()
	{
		if (!isActive) return;
		posX += speed * (DIR == 2 ? -1 : 1);

		if (delay > 0)
		{
			delay--;
		}
		else
		{
			isActive = false;
			delay = 15;
		}
	}

	void Active(bool isActive)
	{
		this->isActive = isActive;
	}

	void Render(HDC hdc)
	{		
		if (!isActive) return;

		//HPEN pen = CreatePen(PS_SOLID, 5, RGB(255, 0, 0));
		HBRUSH hbr = CreateSolidBrush(RGB(0, 0, 255));
		HBRUSH hbr2 = CreateSolidBrush(RGB(0, 0, 255));

		//SelectObject(hdc, pen);
		SelectObject(hdc, hbr);
		
		RoundRect(hdc, posX, posY, posX + 10, posY + 10, 5, 5);
		
		//Rectangle(hdc, x, y, x + 50, y + 50);

		
		//if (isRnder) {
		////	Rectangle(hdc, 0, 0, 10, 10);
		//}
		SelectObject(hdc, hbr2);

		DeleteObject(hbr);
	}
};

std::list<Missile*> missileLst;

class Background
{
public:
	int groundPosX = 0;
	int skyPosX = 0;
	Image* ground;
	Image* sky;

	Background()
	{
		ground = new Image(L"background.png");
		sky = new Image(L"sky.png");
	}

	void Render(HDC hdc)
	{
		Graphics graphics(hdc);
		Image   ground(L"background.png");
		Image   sky(L"sky.png");

		if (groundPosX > 576 || groundPosX < -576)
		{
			groundPosX = 0;
		}

		if (skyPosX > 824 || skyPosX < -824)
		{
			skyPosX = 0;
		}
				
		graphics.DrawImage(&sky, -824 + skyPosX, 0, 0, 0, 824, 206, Gdiplus::UnitPixel);
		graphics.DrawImage(&sky, 0 + skyPosX, 0, 0, 0, 824, 206, Gdiplus::UnitPixel);
		graphics.DrawImage(&sky, 824 + skyPosX, 0, 0, 0, 824, 206, Gdiplus::UnitPixel);
		graphics.DrawImage(&sky, 824 * 2 + skyPosX, 0, 0, 0, 824, 206, Gdiplus::UnitPixel);

		graphics.DrawImage(&ground, -576 + groundPosX, 150, 10, 450, 576, 500, Gdiplus::UnitPixel);
		graphics.DrawImage(&ground, 0 + groundPosX, 150, 10, 450, 576, 500, Gdiplus::UnitPixel);
		graphics.DrawImage(&ground, 576 + groundPosX, 150, 10, 450, 576, 500, Gdiplus::UnitPixel);
		graphics.DrawImage(&ground, 1152 + groundPosX, 150, 10, 450, 576, 500, Gdiplus::UnitPixel);


	}

};

Background* background;

class Enemy {
public:
	int x;
	int y;
	int frame;
	int DIR;
	int width;
	bool isMove;
	bool isActive = true;

	Image*	image;
	Enemy(int _x, int _y) {

		width = 30;

		image = new Image(L"pngresource.png");
		int size = image->GetPaletteSize();
		ColorPalette* tmp = (ColorPalette*)malloc(size);
		image->GetPalette(tmp, size);


		BYTE a = 0x50;
		BYTE r = 0x80;
		BYTE g = 0x70;
		BYTE b = 0x60;

		// Create an ARGB value from the four component values.
		ARGB argb = Color::MakeARGB(a, r, g, b);

		Color argbColor(argb);


		tmp->Count = 4;
		tmp->Entries[0] = argb;

		image->SetPalette(tmp);

		x = _x;
		y = _y;
		frame = 0;
		DIR = 1;
		isMove = false;




	}
	void Render() {

		if (!isActive) return;
		if (DIR == 2) {
			image->RotateFlip(RotateNoneFlipX);
		}
		if (!isMove)
		{

			frame %= 3;
			if (frame == 0) {
				DrawFlip(image, x, y, 0, 0, 34, 40, DC, DIR == 2, 0);
				//graphics.DrawImage(&image, x, y, 0, 0, 34, 40, Gdiplus::UnitPixel);
			}
			else if (frame == 1) {
				DrawFlip(image, x + 3, y, 34, 0, 30, 40, DC, DIR == 2, 1);
				//graphics.DrawImage(&image, x+3, y, 34, 0, 30, 40, Gdiplus::UnitPixel);
			}
			else if (frame == 2) {
				DrawFlip(image, x + 4, y, 64, 0, 32, 40, DC, DIR == 2, 4);
				//graphics.DrawImage(&image, x+4, y, 64, 0, 32, 40, Gdiplus::UnitPixel);
			}
		}
		else {
			frame %= 8;
			switch (frame)
			{
			case 0:DrawFlip(image, x, y, 137, 0, 31, 43, DC, DIR == 2, 0); break;
			case 1:DrawFlip(image, x, y, 168, 0, 32, 43, DC, DIR == 2, 0); break;
			case 2:DrawFlip(image, x, y, 200, 0, 32, 43, DC, DIR == 2, 0); break;
			case 3:DrawFlip(image, x, y, 234, 0, 34, 43, DC, DIR == 2, 0); break;
			case 4:DrawFlip(image, x, y, 266, 0, 32, 43, DC, DIR == 2, 0); break;
			case 5:DrawFlip(image, x, y, 298, 0, 32, 43, DC, DIR == 2, 0); break;
			case 6:DrawFlip(image, x, y, 331, 0, 33, 43, DC, DIR == 2, 0); break;
			case 7:DrawFlip(image, x, y, 367, 0, 36, 43, DC, DIR == 2, 0); break;
			case 8:DrawFlip(image, x, y, 403, 0, 36, 43, DC, DIR == 2, 0); break;
			}
		}
	}
	void Move(int v) {
		x += v;
	}
	void Update() {
		frame += 1;
	}
};

std::list<Enemy*> enumLst;


class Avatar
{
public:
	int x;
	int y;
	int frame;
	int DIR;
	int width;
	bool isMove;
	bool isFlip = false;
	Image* image;

	Avatar(int _x, int _y)
	{
		x = _x;
		y = _y;
		frame = 0;
		DIR = 1;
		isMove = false;
		image = new Image(L"pngresource.png");
		width = 30;
	}

	void Render()
	{
		//Image	image(L"imgrsc.png");

		if (isFlip)
		{
			isFlip = false;
			image->RotateFlip(RotateNoneFlipX);
		}

		if (!isMove)
		{


			frame %= 3;
			if (frame == 0) {
				DrawFlip(image, x, y, 0, 0, 34, 40, DC, DIR == 2, 0);
			}
			else if (frame == 1) {

				DrawFlip(image, x + 3, y, 34, 0, 29, 40, DC, DIR == 2, 1);

			}
			else if (frame == 2) {

				DrawFlip(image, x + 3, y, 63, 0, 32, 40, DC, DIR == 2, 4);
			}
		}
		else {

			frame %= 8;

			switch (frame)
			{
			case 0:DrawFlip(image, x, y, 105, 0, 30, 40, DC, DIR == 2, 0); break;
			case 1:DrawFlip(image, x, y, 137, 0, 30, 40, DC, DIR == 2, 0); break;
			case 2:DrawFlip(image, x, y, 167, 0, 30, 40, DC, DIR == 2, 0); break;
			case 3:DrawFlip(image, x, y, 200, 0, 30, 40, DC, DIR == 2, 0); break;
			case 4:DrawFlip(image, x, y, 233, 0, 30, 40, DC, DIR == 2, 0); break;
			case 5:DrawFlip(image, x, y, 264, 0, 30, 40, DC, DIR == 2, 0); break;
			case 6:DrawFlip(image, x, y, 296, 0, 30, 40, DC, DIR == 2, 0); break;
			case 7:DrawFlip(image, x, y, 330, 0, 30, 40, DC, DIR == 2, 0); break;
			case 8:DrawFlip(image, x, y, 366, 0, 30, 40, DC, DIR == 2, 0); break;

			}
		}
	}

	void Move(int v)
	{
		if ((v > 0 && x < 800) || (v < 0 && x > 300))
		{
			x += v;
		}
		else if(v > 0 && x >= 800)
		{
			background->groundPosX -= 10;
			background->skyPosX -= 4;
			auto iter = enumLst.begin();
			for (iter; iter != enumLst.end(); iter++)
			{
				if (DIR == 2)
				{
					(*iter)->Move(v);
				}
				else
				{
					(*iter)->Move(-v);
				}
			}
		}
		else if (v < 0 && x <= 300)
		{
			background->groundPosX += 10;
			background->skyPosX += 4;
			auto iter = enumLst.begin();
			for (iter; iter != enumLst.end(); iter++)
			{
				if (DIR == 2)
				{
					(*iter)->Move(-v);
				}
				else
				{
					(*iter)->Move(v);
				}
			}
		}
	}

	void Update()
	{
		frame+=1;

		if (isMove)
		{
			switch (DIR)
			{
			case 1:
				Move(10);
				break;
			case 2:
				Move(-10);
				break;

			}
		}


	}


};


Avatar* avatar;






void CollisionCheck()
{
	auto iter = enumLst.begin();
	auto iter2 = missileLst.begin();
	
	for (iter2; iter2 != missileLst.end(); iter2++)
	{

		for (iter; iter != enumLst.end(); iter++)
		{			
			if ((*iter)->x + (*iter)->width >= avatar->x && (*iter)->x <= avatar->x + avatar->width)
			{
				(*iter)->isActive = false;
			//enumLst.erase(iter);
				//continue;
			}
			if ((*iter)->isActive)
			{
				if ((*iter2)->posX + (*iter2)->width >= (*iter)->x && (*iter2)->posX <= (*iter)->x + (*iter)->width)
				{
					(*iter2)->Active(false);
					(*iter)->isActive = false;
					//continue;
				}
			}

		}
	}

	//auto iter = enumLst.begin();

	//for (iter; iter != enumLst.end(); iter++) {
	//	avatar->x;
	//	avatar->width;
	//	(*iter)->x;
	//	(*iter)->width;

	//	if ((*iter)->x >= avatar->x && (*iter)->x <= avatar->x + avatar->width)
	//	{
	//		//충돌
	//		enumLst.erase(iter);
	//		break;
	//	}
	//	if ((*iter)->x + (*iter)->width >= avatar->x
	//		&& (*iter)->x + (*iter)->width <= avatar->x + avatar->width)
	//	{
	//		//충돌
	//		enumLst.erase(iter);
	//		break;
	//	}
	//}

	//auto iter2 = missileLst.begin();

	//for (iter2; iter2 != missileLst.end(); iter2++) {
	//	if (!(*iter2)->isActive) continue;
	//	iter = enumLst.begin();
	//	for (iter; iter != enumLst.end(); iter++) {
	//		if (!(*iter)->isActive) continue;
	//		if ((*iter)->x >= (*iter2)->posX && (*iter)->x <= (*iter2)->posX + (*iter2)->width)
	//		{
	//			//충돌
	//			//enumLst.erase(iter);
	//			(*iter)->isActive = false;
	//			(*iter2)->isActive = false;
	//			continue;
	//		}
	//		if ((*iter)->x + (*iter)->width >= (*iter2)->posX
	//			&& (*iter)->x + (*iter)->width <= (*iter2)->posX + (*iter2)->width)
	//		{
	//			//충돌
	//			//enumLst.erase(iter);
	//			(*iter)->isActive = false;
	//			(*iter2)->isActive = false;
	//			continue;
	//		}
	//	}
	//}
	

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


	ULONG_PTR gdiToken;
	GdiplusStartupInput gidplusStartupInput;
	GdiplusStartup(&gdiToken, &gidplusStartupInput, NULL);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // 기본 메시지 루프입니다.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	GdiplusShutdown(gdiToken);
    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	//wcex.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, L"TEST", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//



void OnPaint(HDC hdc)
{
	
	if (DC == NULL) DC = CreateCompatibleDC(hdc);
	if (DC2 == NULL) DC2 = CreateCompatibleDC(hdc);





	HBITMAP newBitmap, preBitmap;

	
	newBitmap = CreateCompatibleBitmap(hdc, 1152, 352);// 비트맵을 실제로 읽어온다 - 이미지를 만든다
	//newBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(DC, 1000, 1000));// 비트맵을 실제로 읽어온다
	preBitmap = (HBITMAP)SelectObject(DC, (HGDIOBJ)newBitmap);  // 새로만든 dc에 읽어온 비트맵을 장착
	(HBITMAP)SelectObject(DC2, (HGDIOBJ)preBitmap);
	Graphics graphics(DC);
	//Graphics graphics(hdc);
	PatBlt(DC, 0, 0, 1152, 352, BLACKNESS);


	background->Render(DC);
	avatar->Render();

	auto iter = enumLst.begin();

	for (iter; iter != enumLst.end(); iter++) {
		(*iter)->Render();
	}

	auto iter2 = missileLst.begin();

	for (iter2; iter2 != missileLst.end(); iter2++) {
		(*iter2)->Render(DC);
	}
	

	
	//MAKEINTRESOURCE(DC, 1000, 1000);// 비트맵을 실제로 읽어온다
	BitBlt(hdc, 0, 0, 1152, 345, DC2, 0, 0, SRCCOPY); // 여기 앞에 좌표를 카메라 좌표로 사용
	//SelectObject(DC, (HGDIOBJ)preBitmap);  // 새로만든 dc에 읽어온 비트맵을 장착


	//ReleaseDC(hWnd, hdc); -> 언제 할지 고민해볼것. 오래 켜놓으면렉걸림.

}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	

	case WM_KEYDOWN:
	{
		// avatar 관련 처리
		// 눌렸다는거 인지
		//if (wParam == VK_LEFT || wParam == VK_RIGHT)
		
		switch (wParam)
		{
		case VK_LEFT:
		{
			if (avatar->DIR == 1)
			{
				avatar->DIR = 2;
				avatar->isFlip = true;
			}
			avatar->isMove = true;
		}
		break;
		case VK_RIGHT:
		{
			if (avatar->DIR == 2)
			{
				avatar->DIR = 1;
				avatar->isFlip = true;
			}
			avatar->isMove = true;
		}
		break;
		case VK_SPACE:
		{
			auto iter = missileLst.begin();
			for (iter; iter != missileLst.end(); iter++)
			{
				if (!((*iter)->isActive))
				{
					(*iter)->Active(true);
					if (avatar->DIR == 2)
					{
						(*iter)->posX = avatar->x - 5;
					}
					else
					{
						(*iter)->posX = avatar->x + 20;
					}
					(*iter)->posY = avatar->y+15;
					(*iter)->DIR = avatar->DIR;
					break;
				}
			}

		}
		break;



			
		}

	}
	break;
	case WM_KEYUP:
	{
		avatar->isMove = false;
		//풀렸다는 것 인지
		//DIR = 0;
		//isMove = FALSE;
	}
	break;
	case WM_CREATE:
	{
		avatar = new Avatar(500, 240);
		background = new Background();
		for (int i = 0; i < 10; i++)
		{
			enumLst.push_back(new Enemy(160 * i, 240));
		}
		for (int i = 0; i < 10; i++)
		{
			missileLst.push_back(new Missile(0, 0, 1, 15, 15));
			
	
		}
		static HANDLE hTimer = (HANDLE)SetTimer(hWnd, 1, 100, NULL);
		
	}
		break;
	case WM_TIMER:
	{
		// avatar의 update 호출
		avatar->Update();
		auto iter = enumLst.begin();
		for (iter; iter != enumLst.end(); iter++) {
			(*iter)->Update();
		}

		auto iter2 = missileLst.begin();
		for (iter2; iter2 != missileLst.end(); iter2++) {
			(*iter2)->Update();
		}

			//OnPaint((HDC)hWnd);
			//InvalidateRect((HWND)DC, NULL, true);
			//InvalidateRect(hWnd, NULL, true);

		CollisionCheck();
		

			HDC hdc = GetDC(hWnd);
			OnPaint(hdc);
		
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
		// 펜
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.


			

			OnPaint(hdc);





            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
