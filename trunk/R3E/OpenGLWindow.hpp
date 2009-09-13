#ifndef GL_WINDOW_H
#define GL_WINDOW_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl/gl.h>
#include "String.hpp"

class OpenGLWindow {
public:
	OpenGLWindow() : mRunning(TRUE), mClassName("ROSEGL") {}

	virtual ~OpenGLWindow(){}

	virtual BOOL InitGL() = 0;
	virtual void DrawGL() = 0;
	virtual void ResizeGL(int width, int height) = 0;

	virtual void MouseWheel(WPARAM wParam, LPARAM lParam){}
	virtual void MouseMove(WPARAM wParam, LPARAM lParam){}
	virtual void KeyDown(WPARAM wParam, LPARAM lParam){}
	virtual void KeyUp(WPARAM wParam, LPARAM lParam){}

	int Run(){
		MSG msg;
		while(mRunning){
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
				if(msg.message == WM_QUIT){
					mRunning = FALSE;
				}else{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}else{
				if(!mActive) continue;
				DrawGL();
				SwapBuffers(mHDC);
			}
		}

		Destroy();

		return msg.wParam;
	}

	int Create(){
		GLuint PixelFormat;
		WNDCLASS wc;
		RECT WindowRect;
		WindowRect.left = WindowRect.top = 0;
		WindowRect.right = mWidth;
		WindowRect.bottom = mHeight;

		mInstance = GetModuleHandle(NULL);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = (WNDPROC)WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = mInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = mClassName;

		if(!RegisterClass(&wc)) return FALSE;

		AdjustWindowRectEx(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_APPWINDOW | WS_EX_WINDOWEDGE);

		mHWND = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, mClassName, mTitle, WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, WindowRect.right - WindowRect.left, WindowRect.bottom - WindowRect.top, NULL, NULL, mInstance, (LPVOID)this);

		if(!mHWND){
			Destroy();
			return FALSE;
		}

		static	PIXELFORMATDESCRIPTOR pfd = {
			sizeof(PIXELFORMATDESCRIPTOR),
			1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
			32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, PFD_MAIN_PLANE, 0,0, 0, 0
		};

		mHDC = GetDC(mHWND);
		if(!mHDC){
			Destroy();
			return FALSE;
		}

		PixelFormat = ChoosePixelFormat(mHDC, &pfd);
		if(!PixelFormat){
			Destroy();
			return FALSE;
		}

		if(!SetPixelFormat(mHDC, PixelFormat, &pfd)){
			Destroy();
			return FALSE;
		}

		mHRC = wglCreateContext(mHDC);

		if(!mHRC){
			Destroy();
			return FALSE;
		}

		if(!wglMakeCurrent(mHDC, mHRC)){
			Destroy();
			return FALSE;
		}

		ShowWindow(mHWND, SW_SHOW);
		SetForegroundWindow(mHWND);
		SetFocus(mHWND);
		ResizeGL(mWidth, mHeight);

		if(!InitGL()){
			Destroy();
			return FALSE;
		}

		mActive = TRUE;

		return TRUE;
	}

	void Destroy(){
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(mHRC);
		ReleaseDC(mHWND, mHDC);
		DestroyWindow(mHWND);

		UnregisterClass(mClassName, mInstance);

		mHWND = NULL;
		mHRC = NULL;
		mHDC = NULL;
	}

	void SetActive(BOOL active){
		mActive = active;
	}

	void SetKey(int key, BOOL value){
		mKeys[key] = value;
	}

	OpenGLWindow& SetClassName(const char* classname){
		mClassName = classname;
		return *this;
	}

	OpenGLWindow& SetSize(int width, int height){
		mWidth = width;
		mHeight = height;
		return *this;
	}

	OpenGLWindow& SetTitle(const char* title){
		mTitle = title;
		if(mHWND){
			SetWindowText(mHWND, title);
		}

		return *this;
	}

	BOOL Key(int key){
		return mKeys[key];
	}

	int Height(){
		return mHeight;
	}

	int Width(){
		return mWidth;
	}

private:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
		static OpenGLWindow* wnd = NULL;
		if(uMsg == WM_NCCREATE)
			wnd = (OpenGLWindow*)(((LPCREATESTRUCT)lParam)->lpCreateParams);

		switch (uMsg){
			case WM_ACTIVATE:
				if(wnd) wnd->SetActive(!HIWORD(wParam));
				return 0;

			case WM_CLOSE:
				PostQuitMessage(0);
				return 0;

			case WM_MOUSEMOVE:
				if(wnd) wnd->MouseMove(wParam, lParam);
				return 0;

			case WM_MOUSEWHEEL:
				if(wnd) wnd->MouseWheel(wParam, lParam);
				return 0;

			case WM_KEYDOWN:
				if(wnd){
					wnd->SetKey(wParam, TRUE);
					wnd->KeyDown(wParam, lParam);
				}
				return 0;

			case WM_KEYUP:
				if(wnd){
					wnd->SetKey(wParam, FALSE);
					wnd->KeyUp(wParam, lParam);
				}
				return 0;

			case WM_SIZE:
				if(wnd) wnd->ResizeGL(LOWORD(lParam), HIWORD(lParam));
				return 0;
		}

		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}

protected:
	String mTitle;
	String mClassName;
	int mWidth;
	int mHeight;

	HWND mHWND;
	HDC mHDC;
	HGLRC mHRC;
	HINSTANCE	mInstance;

	unsigned char mKeys[256];
	BOOL mActive;
	BOOL mRunning;
};

#endif