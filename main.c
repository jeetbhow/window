#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

static int INIT_WIDTH = 800;
static int INIT_HEIGHT = 600;

// Front declaration of our main window procedure.
LRESULT CALLBACK mainWindowProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lPARAM);

// The only parameter that's really used here is the instance. The other ones can be ignored.
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int showCode)
{
	// Class name identifies the window class.
	const wchar_t CLASS_NAME[] = L"Example Class Name";

	/*
		Setup the window class. A window class is a template that defines the behaviour of
		multiple windows. It has three key properties:
		* instance - the process instance
		* class name - identifies the class
		* window procedure - behaviour of the window (message handling)

		The other properties can be set to 0 as they aren't important.
	*/

	WNDCLASS windowClass = {0};
	windowClass.hInstance = instance;
	windowClass.lpfnWndProc = mainWindowProc;
	windowClass.lpszClassName = CLASS_NAME;

	// Register the window class so that it can be used.
	RegisterClass(&windowClass);

	// Create the main window and set its window class to the one we just made.
	HWND window = CreateWindowEx(
		0,						// optional style info
		CLASS_NAME,				// name of the window class
		L"Example Window Name", // window title

		// Style and position data
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, INIT_WIDTH, INIT_HEIGHT,

		NULL,	  // parent
		NULL,	  // menu
		instance, // app instance
		NULL);

	/*
		This code is the event loop. The DispatchMessage function inside the loop activates the main window procedure.
	*/

	ShowWindow(window, showCode);

	MSG message = {0};
	while (GetMessage(&message, window, 0, 0) > 0)
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return 0;
}

HBITMAP *createBitMap()
{
}

LRESULT CALLBACK mainWindowProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HDC backbuf = {0};
	static BITMAPINFO bitmapInfo = {0};
	static HBITMAP bitmapHandle = {0};
	static HANDLE handleOld = {0};
	static RGBQUAD *pixels;
	switch (msg)
	{
	case WM_CREATE:
		// We use the handle to create the backbuffer
		HDC frontbuf = GetDC(handle);

		// This is what we're drawing on and copying over to the frontbuffer.
		backbuf = CreateCompatibleDC(frontbuf);

		// Set up the BITMAPINFO struct
		bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitmapInfo.bmiHeader.biWidth = INIT_WIDTH;
		bitmapInfo.bmiHeader.biHeight = -INIT_HEIGHT;
		bitmapInfo.bmiHeader.biPlanes = 1;
		bitmapInfo.bmiHeader.biBitCount = 32;
		bitmapInfo.bmiHeader.biCompression = BI_RGB;

		// Create the device independent bitmap using the frontbuffer
		bitmapHandle = CreateDIBSection(frontbuf, &bitmapInfo, DIB_RGB_COLORS, (void **)&pixels, NULL, 0);

		// This function designates the bitmap that we created as being able to manipulate the backbuffer memory.
		handleOld = SelectObject(backbuf, bitmapHandle);
		break;
	case WM_SIZE:
		RECT rect;
		GetClientRect(handle, &rect);
		bitmapInfo.bmiHeader.biWidth = rect.left - rect.right;
		bitmapInfo.bmiHeader.biHeight = rect.bottom - rect.top;
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;

		HDC hdc = BeginPaint(handle, &ps);
		LONG width = bitmapInfo.bmiHeader.biWidth;
		LONG height = bitmapInfo.bmiHeader.biHeight;
		for (int i = 0; i < width * height; i++)
		{
			pixels[i].rgbRed = 255;
		}

		BitBlt(hdc, 0, 0, width, height, backbuf, 0, 0, SRCCOPY);
		EndPaint(handle, &ps);

		break;
	case WM_DESTROY:
		SelectObject(backbuf, handleOld);
		DeleteObject(bitmapHandle);
		DeleteDC(backbuf);
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(handle);
		break;
	default:
		return DefWindowProc(handle, msg, wParam, lParam);
	}
	return 0;
}