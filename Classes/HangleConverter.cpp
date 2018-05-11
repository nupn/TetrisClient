// Win32Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Project1.h"

#include <windows.h>
#include <string>

#include <imm.h>

//https://blog.naver.com/unjong_/220736503338
//http://showmiso.tistory.com/125

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32PROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable


   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	ImmAssociateContext(hWnd, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//



enum
{
	kCusorInsert = 0,
	kCursorTyping,
};

int nCursorState = kCusorInsert;
int nCursorIdx = 0;
int nCharlen = 0;
TCHAR inputChar[512] = {0, };

int GetFirstFromCombinded(TCHAR character)
{
	return ((character - 0xAC00) / 28) / 21;
}

int GetMiddleFromCombinded(TCHAR character)
{
	return ((character - 0xAC00) / 28) % 21;
}

int GetLastFromCombinded(TCHAR character)
{
	return (character - 0xAC00) % 28;
}

bool IsCombindKorean(TCHAR character)
{
	if (character >= 0xAC00 && character <= 0xD7AF)
	{
		return true;
	}

	return false; 
}

bool IsConsonat(unsigned int chararacter)
{
	//Q,W,E,r,t,
	//a,s,d,f,g
	//z,x,c,v,
	switch (chararacter)
	{
	case 81://Q
	case 87://W
	case 69://E
	case 82://R
	case 84://T

	case 65://A
	case 83://S
	case 68://D
	case 70://F
	case 71://G

	case 90://z
	case 88://x
	case 67://c
	case 86://v


	case 113://Q
	case 119://W
	case 101://E
	case 114://R
	case 116://T

	case 97://A
	case 115://S
	case 100://D
	case 102://F
	case 103://G

	case 122://z
	case 120://x
	case 99://c
	case 118://v
		return true;


	default:
		return false;
	}

	return false;
}

bool IsSoloKorean(TCHAR chararacter)
{
	TCHAR nRelativeIdx = chararacter;
	if (nRelativeIdx >= 0x3131 && nRelativeIdx <= 0x3163)
	{
		return true;
	}

	return false;
}

TCHAR getMainChar(unsigned int chararacter, bool isShift)
{
	switch (chararacter)
	{
	case 81://Q
	case 113://Q
		if (isShift)
		{
			return 0x3143;
		}
		return 0x3142; //ㅂ
	case 87://W
	case 119://W
		if (isShift)
		{
			return 0x3149;//ㅉ
		}
		return 0x3148;//ㅈ
	case 69://E
	case 101://E
		if (isShift)
		{
			return 0x3138;
		}
		return 0x3137;//ㄷ
	case 82://R
	case 114://R
		if (isShift)
		{
			return 0x3132;
		}
		return 0x3131;//ㄱ
	case 84://T
	case 116://T
		if (isShift)
		{
			return 0x3146;//ㅅ
		}
		return 0x3145;//ㅅ

	case 89://Y
	case 121://Y
		return 0x315B;//ㅛ
	case 85://U
	case 117://U
		return 0x3155;//ㅕ
	case 73://I
	case 105://I
		return 0x3151;//ㅑ
	case 79:
	case 111://O
		if (isShift)
		{
			return 0x3152;
		}
		return 0x3150;
		//ㅐ,ㅒ
	case 80://p
	case 112://p
			 //ㅔ,ㅖ
		if (isShift)
		{
			return 0x3156;
		}
		return 0x3154;


	case 65://A
	case 97://A
		return 0x3141; //ㅁ
	case 83://S
	case 115://S
		return 0x3134;//ㄴ
	case 68://D
	case 100://D
		return 0x3147;//o
	case 70://F
	case 102://F
		return 0x3139;//ㄹ
	case 71://G
	case 103://G
		return 0x314E;//ㅎ
	case 72: //H
	case 104: //H
		return 0x3157;//ㅗ
	case 74: //J
	case 106: //J
		return 0x3153;//ㅓ
	case 75: //K
	case 107: //K
		return 0x314F;//ㅏ
	case 76: //L
	case 108: //L
		return 0x3163;//ㅣ

	case 122://z
	case 90://z
		return 0x314B;//ㅋ
	case 88://x
	case 120://x
		return 0x314C;//ㅌ

	case 67://c
	case 99://c
		return 0x314A;//ㅊ
	case 86://v
	case 118://v
		return 0x314D;//ㅍ
	case 66://B
	case 98://B
		return 0x3160;//ㅠ
	case 78://N
	case 110://N
		return 0x315C;//ㅜ
	case 77://M
	case 109://M
		return 0x3161;//ㅡ


	default:
		return -1;
	}

	return -1;
}


int getMainChar(TCHAR prevchar, unsigned int chararacter)
{
	switch (chararacter)
	{
	case 79:
	case 111://O
		if (prevchar == 0x3157)
		{
			return 0x3159;//ㅙ
		}
		//ㅐ,ㅒ
		break;
	case 80://p
	case 112://p
		if (prevchar == 0x315C)
		{
			return 0x315E;//ㅞ
		}
		//ㅔ,ㅖ
		break;

	case 74: //J
	case 106: //J
		if (prevchar == 0x315C)
		{
			return 0x315D;//ㅝ
		}
		//ㅓ
		break;

	case 75: //K
	case 107: //K
		if (prevchar == 0x3157)
		{
			return 0x3158;//ㅘ
		}
		break;

	case 76: //L
	case 108: //L
		if (prevchar == 0x3157)
		{
			return 0x315A;//ㅚ
		}

		if (prevchar == 0x315C)
		{
			return 0x315F;//ㅟ
		}

		if (prevchar == 0x3161)
		{
			return 0x3162;//ㅢ
		}
		break;

	default:
		return -1;
	}

	return -1;
}

int getSubCode(TCHAR prevchar, unsigned int chararacter, bool isShift)
{
	switch (chararacter)
	{
	case 79:
	case 111://O
		if (prevchar == 8 && isShift == false)
		{
			return 10;//ㅙ
		}
		//ㅐ,ㅒ
		break;

	case 80://p
	case 112://p
		//ㅔ,ㅖ
		if (prevchar == 13 && isShift == false)
		{
			return 15;//ㅞ
		}
		break;


	case 74: //J
	case 106: //J
		if (prevchar == 13)
		{
			return 14;//ㅝ
		}
		break;

	case 75: //K
	case 107: //K
		if (prevchar == 8)
		{
			return 9;//ㅘ
		}
		break;

	case 76: //L
	case 108: //L
		if (prevchar == 8)
		{
			return 11;//ㅚ
		}

		if (prevchar == 13)
		{
			return 16;//ㅟ
		}


		if (prevchar == 18)
		{
			return 19;//ㅢ
		}
		break;

	default:
		return -1;
	}

	return -1;
}


int getSubCode(unsigned int chararacter, bool isShift)
{
	switch (chararacter)
	{
	case 81://Q
	case 113://Q
		if (isShift)
		{
			return 8;
		}
		return 7; //ㅂ
	case 87://W
	case 119://W
		if (isShift)
		{
			return 13;//ㅉ
		}
		return 12;//ㅈ
	case 69://E
	case 101://E
		if (isShift)
		{
			return 4;
		}
		return 3;//ㄷ
	case 82://R
	case 114://R
		if (isShift)
		{
			return 1;
		}
		return 0;//ㄱ
	case 84://T
	case 116://T
		if (isShift)
		{
			return 10;//ㅅ
		}
		return 9;//ㅅ

	case 89://Y
	case 121://Y
		return 12;//ㅛ
	case 85://U
	case 117://U
		return 6;//ㅕ
	case 73://I
	case 105://I
		return 2;//ㅑ
	case 79:
	case 111://O
		if (isShift)
		{
			return 3;
		}
		return 1;
		//ㅐ,ㅒ
	case 80://p
	case 112://p
			 //ㅔ,ㅖ
		if (isShift)
		{
			return 7;
		}
		return 5;


	case 65://A
	case 97://A
		return 6; //ㅁ
	case 83://S
	case 115://S
		return 2;//ㄴ
	case 68://D
	case 100://D
		return 11;//o
	case 70://F
	case 102://F
		return 5;//ㄹ
	case 71://G
	case 103://G
		return 18;//ㅎ
	case 72: //H
	case 104: //H
		return 8;//ㅗ
	case 74: //J
	case 106: //J
		return 4;//ㅓ
	case 75: //K
	case 107: //K
		return 0;//ㅏ
	case 76: //L
	case 108: //L
		return 20;//ㅣ

	case 122://z
	case 90://z
		return 15;//ㅋ
	case 88://x
	case 120://x
		return 16;//ㅌ

	case 67://c
	case 99://c
		return 14;//ㅊ
	case 86://v
	case 118://v
		return 17;//ㅍ
	case 66://B
	case 98://B
		return 17;//ㅠ
	case 78://N
	case 110://N
		return 13;//ㅜ
	case 77://M
	case 109://M
		return 18;//ㅡ


	default:
		return -1;
	}

	return -1;
}

int getLastCode(unsigned int chararacter, bool isShift)
{
	switch (chararacter)
	{
	case 81://Q
	case 113://Q
		if (isShift)
		{
			return -1;
		}
		return 17; //ㅂ
	case 87://W
	case 119://W
		if (isShift)
		{
			return -1;
		}
		return 22;//ㅈ
	case 69://E
	case 101://E
		if (isShift)
		{
			return -1;
		}
		return 7;//ㄷ
	case 82://R
	case 114://R
		if (isShift)
		{
			return 2;//ㄲ
		}
		return 1;//ㄱ
	case 84://T
	case 116://T
		if (isShift)
		{
			return 20;//ㅅ
		}
		return 19;//ㅅ

	case 65://A
	case 97://A
		if (isShift)
		{
			return -1;
		}
		return 16; //ㅁ
	case 83://S
	case 115://S
		if (isShift)
		{
			return -1;
		}
		return 4;//ㄴ
	case 68://D
	case 100://D
		if (isShift)
		{
			return -1;
		}
		return 21;//o
	case 70://F
	case 102://F
		if (isShift)
		{
			return -1;
		}
		return 8;//ㄹ
	case 71://G
	case 103://G
		if (isShift)
		{
			return -1;
		}
		return 27;//ㅎ

	case 122://z
	case 90://z
		if (isShift)
		{
			return -1;
		}
		return 24;//ㅋ
	case 88://x
	case 120://x
		if (isShift)
		{
			return -1;
		}
		return 25;//ㅌ
	case 67://c
	case 99://c
		if (isShift)
		{
			return -1;
		}
		return 23;//ㅊ
	case 86://v
	case 118://v
		if (isShift)
		{
			return -1;
		}
		return 26;//ㅍ


	default:
		return -1;
	}

	return -1;
}


int getLastCode(TCHAR prevCode, unsigned int chararacter)
{
	switch (chararacter)
	{
	case 81://Q
	case 113://Q
		if (prevCode == 8)
		{
			return 11;//ㄼ
		}
		break;

	case 87://W
	case 119://W
		if (prevCode == 4)
		{
			return 5;//ㄵ
		}
		break;

	case 82://R
	case 114://R
		if (prevCode == 8)
		{
			return 9;//ㄺ
		}
		break;

	case 84://T
	case 116://T
		if (prevCode == 1)
		{
			return 3;//ㄳ
		}
		
		if (prevCode == 8)
		{
			return 12;//ㄽ
		}

		if (prevCode == 17)
		{
			return 18;//ㅄ
		}
		break;

	case 65://A
	case 97://A
		if (prevCode == 8)
		{
			return 10;//ㄻ
		}
		break;

	case 71://G
	case 103://G
		if (prevCode == 4)
		{
			return 6;//ㄶ
		}

		if (prevCode == 8)
		{
			return 15;//ㅀ
		}
		break;

	case 88://x
	case 120://x
		if (prevCode == 8)
		{
			return 13;//ㄾ
		}
		break;

	case 86://v
	case 118://v
		if (prevCode == 8)
		{
			return 14;//ㄿ
		}
		break;


	default:
		return -1;
	}

	return -1;
}

//메인차를 조합형 인덱스로 
int GetFirstIdxFromMainIdx(TCHAR firstIdx)
{
	switch(firstIdx)
	{
	case 0x3131:
		return 0;
	case 0x3132:
		return 1;
	case 0x3134:
		return 2;
	case 0x3137:
		return 3;
	case 0x3138:
		return 4;
	case 0x3139:
		return 5;
	case 0x3141:
		return 6;
	case 0x3142:
		return 7;
	case 0x3143:
		return 8;
	case 0x3145:
		return 9;
	case 0x3146:
		return 10;
	case 0x3147:
		return 11;
	case 0x3148:
		return 12;
	case 0x3149:
		return 13;
	case 0x314A:
		return 14;
	case 0x314B:
		return 15;
	case 0x314C:
		return 16;
	case 0x314D:
		return 17;
	case 0x314E:
		return 18;

	default:
		return -1;
	
	}
	return -1;
}

//조합인덱스를 메인차로
int GetMainFromFirstIdx(int nFirstidx)
{
	switch (nFirstidx)
	{
	case 0:
		return 0x3131;
	case 1:
		return 0x3132;
	case 2:
		return 0x3134;
	case 3:
		return 0x3137;
	case 4:
		return 0x3138;
	case 5:
		return 0x3139;
	case 6:
		return 0x3141;
	case 7:
		return 0x3142;
	case 8:
		return 0x3143;
	case 9:
		return 0x3145;
	case 10:
		return 0x3146;
	case 11:
		return 0x3147;
	case 12:
		return 0x3148;
	case 13:
		return 0x3149;
	case 14:
		return 0x314A;
	case 15:
		return 0x314B;
	case 16:
		return 0x314C;
	case 17:
		return 0x314D;
	case 18:
		return 0x314E;

	default:
		return -1;

	}
	return -1;
}

//조합된 메인차 단일로 되돌리기
int GetMainCharDivide(TCHAR firstIdx)
{
	switch (firstIdx)
	{
	case 0x3158:
	case 0x3159:
	case 0x315A:
		return 0x3157;

	case 0x315D:
	case 0x315E:
	case 0x315F:
		return 0x315C;
	
	case 0x3162:
		return 0x3161;

	default:
		return -1;

	}
	return -1;
}

//조합된 중성일경우 조합전으로 되돌리기
int GetMiddleCharDivide(int middleIdx)
{
	switch (middleIdx)
	{
	case 9:
	case 10:
	case 11:
		return 8;

	case 14:
	case 15:
	case 16:
		return 13;

	case 19:
		return 18;

	default:
		return -1;

	}
	return -1;
}

int ConvertLastToFirst(const int nIdx)
{
	//종성 idx를 초성 idx로 변경
	switch (nIdx)
	{
	case 1:
		return 0;
	case 2:
		return 1;
	case 3:
		return 9;
	case 4:
		return 2;
	case 5:
		return 12;
	case 6:
		return 18;
	case 7:
		return 3;
	case 8:
		return 5;
	case 9:
		return 0;
	case 10:
		return 6;
	case 11:
		return 7;
	case 12:
		return 9;
	case 13:
		return 16;
	case 14:
		return 17;
	case 15:
		return 18;
	case 16:
		return 6;
	case 17:
		return 7;
	case 18:
		return 9;
	case 19:
		return 9;
	case 20:
		return 10;
	case 21:
		return 11;
	case 22:
		return 12;
	case 23:
		return 14;
	case 24:
		return 15;
	case 25:
		return 16;
	case 26:
		return 17;
	case 27:
		return 18;

	default:
		return -1;
	}

	return -1;
}

int DeleteLastCharFromLast(int nIdx)
{
	//종성->종성, 종성에서 쌍자음인경우 하나만 제거하기
	switch (nIdx)
	{
	case 2:
	case 3:
		return 1;

	case 5:
	case 6:
		return 4;

	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
		return 8;

	case 18:
		return 17;

	default:
		return 0;
	}

	return 0;
}

int GetSecondIdxFromMainIdx(TCHAR firstIdx)
{
	if (0x314F <= firstIdx && firstIdx <= 0x3163)
	{
		return firstIdx - 0x314F;
	}

	return -1;
}

TCHAR GetCombinedKorean(int firstIdx, int middleIdx, int lastIdx)
{
	return 0xAC00 + (firstIdx * 588) + (middleIdx * 28) + lastIdx;
}
//Delete 처리
//커서이동처림, 스페이스 처리
//앞 종성뒤 메인이동처리
void AddString(unsigned int character, const bool bShift, bool bPullPrev = false);

void AddStringNext(unsigned int character, const bool bShift)
{
	++nCursorIdx;
	nCursorState = kCusorInsert;
	AddString(character, bShift, true);
}

void Insert(int idx)
{
	int moveCnt = nCharlen - idx;
	if (moveCnt > 0)
	{
		memmove(inputChar + idx + 1, inputChar + idx, sizeof(TCHAR) * moveCnt);
	}
}


void AddString(unsigned int character, const bool bShift, bool bPullPrev)
{
	if ((character >= 65 && character <= 90) ||
		character >= 97 && character <= 122)
	{
		if (nCursorState == kCusorInsert)
		{
			bool bInsert = true;
			if (bPullPrev && nCursorIdx > 0)
			{
				auto prevChar = inputChar[nCursorIdx - 1];
				if (IsCombindKorean(prevChar) && !IsConsonat(character))
				{
					int lastIdx = GetLastFromCombinded(prevChar);
					int firstIdx = ConvertLastToFirst(lastIdx);
					if (firstIdx >= 0)
					{
						//이전글자에서 빼기
						int prevFirstIdx = GetFirstFromCombinded(prevChar);
						int prevSecodeIdx = GetMiddleFromCombinded(prevChar);
						lastIdx = DeleteLastCharFromLast(lastIdx);
						inputChar[nCursorIdx - 1] = GetCombinedKorean(prevFirstIdx, prevSecodeIdx, lastIdx);

						//다음글자에추가
						int middleIdx = getSubCode(character, bShift);
						Insert(nCursorIdx);
						inputChar[nCursorIdx] = GetCombinedKorean(firstIdx, middleIdx, 0);
						bInsert = false;
					}
				}
			}
			
			if (bInsert)
			{
				Insert(nCursorIdx);
				inputChar[nCursorIdx] = getMainChar(character, bShift);
			}
			++nCharlen;
			nCursorState = kCursorTyping;
		}
		else if (nCursorState == kCursorTyping)
		{
			int prevChar = inputChar[nCursorIdx];
			if (IsSoloKorean(prevChar)) // 이미 입력된 문자가 한글인지도 체크된다.
			{
				//자음인지 체크
				if (prevChar >= 0x3131 && prevChar <= 0x314E)
				{
					//자음일때는 모음만가능
					if (!IsConsonat(character))
					{
						int choIdx = GetFirstIdxFromMainIdx(prevChar);
						int jungIdx = getSubCode(character, bShift);
						inputChar[nCursorIdx] = GetCombinedKorean(choIdx, jungIdx, 0);
					}
					else
					{
						//다음칸에 입력
						AddStringNext(character, bShift);
					}
				}
				else
				{
					int newMainChar = getMainChar(prevChar, character);
					if (newMainChar != -1)
					{
						inputChar[nCursorIdx] = newMainChar;
					}
					else
					{
						AddStringNext(character, bShift);
					}
					//이미 입력된게 모음이라면 중첩되는지 체크
				}
			}
			else if (IsCombindKorean(prevChar))//조합형일때
			{
				int firstIdx = GetFirstFromCombinded(prevChar);
				int secodeIdx = GetMiddleFromCombinded(prevChar);
				int lastIdx = GetLastFromCombinded(prevChar);

				if (lastIdx == 0)
				{
					int nSubCodeOverlaped = getSubCode(secodeIdx, character, bShift);
					if (IsConsonat(character))
					{
						//종성이없는데 입력된게 자음일때
						lastIdx = getLastCode(character, bShift);//종성에 불가능한자음을 Shite와 누른경우
						if (lastIdx >= 0)
						{
							inputChar[nCursorIdx] = GetCombinedKorean(firstIdx, secodeIdx, lastIdx);
						}
						else
						{
							AddStringNext(character, bShift);
						}
					}
					else if (nSubCodeOverlaped != -1)
					{
						//종성이없는데 입력된게 모음이이고 모음이 조합될때
						inputChar[nCursorIdx] = GetCombinedKorean(firstIdx, nSubCodeOverlaped, 0);
					}
					else
					{
						//종성이없는데 입력된게 모음이고 모음이 조합안되는경우
						AddStringNext(character, bShift);
					}
				}
				else
				{
					int lastOverlapIdx = getLastCode(lastIdx, character);
					if (IsConsonat(character) && lastOverlapIdx != -1 && bShift == false)
					{
						//종성과 입력된 자음이 조합될때
						inputChar[nCursorIdx] = GetCombinedKorean(firstIdx, secodeIdx, lastOverlapIdx);
					}
					else
					{
						// 종성이있고 입력된게 자음이고 자음이 조합안될때
						// 종성이있고 입력된게 모음
						AddStringNext(character, bShift);
					}
				}
			}
			else
			{
				AddStringNext(character, bShift);
			}
		}
	}
}


void Delete(int idx)
{
	if (idx <= 0)
	{
		return;
	}

	int moveCnt = nCharlen - idx;
	if (moveCnt > 0)
	{
		memmove(inputChar + idx - 1, inputChar + idx, sizeof(TCHAR) * moveCnt);
	}

	//그냥지운다
	--nCursorIdx;
	--nCharlen;
	nCursorState = kCusorInsert;
}

void DeleteChar()
{
	if (nCursorState == kCusorInsert)
	{
		Delete(nCursorIdx);
	}
	else if (nCursorState == kCursorTyping)
	{
		int prevChar = inputChar[nCursorIdx];
		if (IsSoloKorean(prevChar))
		{
			int nDivied = GetMainCharDivide(prevChar);
			if (nDivied != -1)
			{
				//조한된 메인차라면지움
				inputChar[nCursorIdx] = nDivied;
			}
			else
			{
				//아니라면 걍지움
				++nCursorIdx;
				nCursorState = kCusorInsert;
				Delete(nCursorIdx);
			}
		}
		else if (IsCombindKorean(prevChar))//조합형일때
		{
			int firstIdx = GetFirstFromCombinded(prevChar);
			int secodeIdx = GetMiddleFromCombinded(prevChar);
			int lastIdx = GetLastFromCombinded(prevChar);
			if (lastIdx > 0)
			{
				//종성 분리후 제거
				int nDvideLastIdx = DeleteLastCharFromLast(lastIdx);
				inputChar[nCursorIdx] = GetCombinedKorean(firstIdx, secodeIdx, nDvideLastIdx);
			}
			else
			{
				int nDevideSecodeIdx = GetMiddleCharDivide(secodeIdx);
				if (nDevideSecodeIdx != -1)
				{
					inputChar[nCursorIdx] = GetCombinedKorean(firstIdx, nDevideSecodeIdx, 0);
				}
				else
				{
					inputChar[nCursorIdx] = GetMainFromFirstIdx(firstIdx);
				}
			}
		}
	}
}

void OnSpace()
{
	if (nCursorState == kCursorTyping)
	{
		++nCursorIdx;
		nCursorState = kCusorInsert;
	}
	else
	{
		Insert(nCursorIdx);
		inputChar[nCursorIdx] = 32;
		++nCursorIdx;
		++nCharlen;
	}
}

void MoveCoursorFront()
{
	if (nCursorIdx > 0)
	{
		nCursorState = kCusorInsert;
		--nCursorIdx;
	}
}

void MoveCoursorBack()
{
	if (nCursorIdx < nCharlen)
	{
		nCursorState = kCusorInsert;
		++nCursorIdx;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CHAR:
	case WM_SYSCHAR:
	case WM_UNICHAR:
	{
		if (message == WM_UNICHAR && wParam == UNICODE_NOCHAR)
		{
			// WM_UNICHAR is not sent by Windows, but is sent by some
			// third-party input method engine
			// Returning TRUE here announces support for this message
			return TRUE;
		}

		bool bShift = false;
		if (GetKeyState(VK_SHIFT) & (1 << 31))
		{
			bShift = true;
		}
		
		if (wParam == VK_BACK)
		{
			DeleteChar();
		}
		else if (wParam == VK_SPACE)
		{
			OnSpace();
		}
		else
		{
			AddString((unsigned int)wParam, bShift);
		}

		InvalidateRect(hWnd, NULL, true);
		return 0;
	}

	case WM_KEYDOWN:
	{
		if (wParam == VK_LEFT)
		{
			MoveCoursorFront();
		}
		else if (wParam == VK_RIGHT)
		{
			MoveCoursorBack();
		}

		return 0;
	}


    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
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
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...

			//test
			TextOut(hdc, 200, 200, inputChar, nCharlen);


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

// Message handler for about box.
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
