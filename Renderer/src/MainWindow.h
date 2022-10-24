#pragma once
#include <string>
#include "Win.h"

struct Resolution
{
	int width;
	int height;
};

class MainWindow
{
public:
	static void Init(std::wstring name, int clientWidth, int clientHeight);
	static void Shutdown();

	static HWND GetHWND();
	static Resolution GetDisplayResolution();
	static bool isMinimized();
	static void HideCursor();
	static void ShowCursor();
	static void ClipCursor(bool clip);
	static void SetCursorToMiddle();
private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void CreateMainWindow(int clientWidth,int clientHeight);
	static void CreateMainWindowClass();
	static Resolution GetAdjustedResolution(int clientWidth, int clientHeight,DWORD dwStyle);

private:
	//MainWindow Class
	inline static const std::wstring MainWindowClassName = L"MainWindowClass";
	inline static bool isCreated = false;

	//MainWindow
	inline static std::wstring MainWindowName;
	inline static HWND hWnd = nullptr;
	inline static Resolution displayResolution;
	inline static bool isFullScreen = false;
	inline static bool isCursorHidden = false;
};
