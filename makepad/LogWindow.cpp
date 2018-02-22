#pragma once
#ifndef as_header
#define def_LogWindow
#endif

#define as_header
#include "Window.cpp"
#include "ToolBar.cpp"
#include "LogControl.cpp"

dec_open(Window, LogWindow)
dec_strong(ToolBar, toolBar)
dec_strong(LogControl, logControl)
dec_table(Window, LogWindow)
dec_method(LogWindow, void, createWindow, HINSTANCE hInstance, int nCmdShow)
dec_method(LogWindow, LRESULT, windowProc, UINT message, WPARAM wParam, LPARAM lParam)
dec_close(Window, LogWindow)

#ifdef def_LogWindow

#define CLASS_LOGWINDOW L"LOGWINDOWCLASS"

def_override(Base, LogWindow, void, constructor) {
	self_override(LogWindow)
}

static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	LogWindow *win = (LogWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	sLogWindow self = win;
	if (!self) return DefWindowProc(hWnd, message, wParam, lParam);
	return (*self)->windowProc(self, message, wParam, lParam);
}
 
void onTailChangeToolBar(sToolBar tb, bool checked) {
	sLogWindow self = tb->parentWindow.ptr();
	(*self->logControl)->setTailCheck(self->logControl, checked);

}

void onTailChangeLogControl(sLogControl lc,  bool checked) {
	sLogWindow self = lc->parentWindow.ptr();
	(*self->toolBar)->setTailCheck(self->toolBar, checked);
}

void onClearToolBar(sToolBar tb) {
	sLogWindow self = tb->parentWindow.ptr();
	(*self->logControl)->clearLog(self->logControl);
}


def_method(LogWindow, void, createWindow, HINSTANCE hInstance, int nCmdShow){
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = StaticWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAKEPAD));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = CLASS_LOGWINDOW;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	RegisterClassExW(&wcex);

	self->hWnd = CreateWindowW(
		CLASS_LOGWINDOW, L"Makepad Log", WS_OVERLAPPEDWINDOW, 0, 0,
		800, 600, GetDesktopWindow(), 0, hInstance, 0
	);

	SetWindowLongPtr(self->hWnd, GWLP_USERDATA, (LONG_PTR)self.ptr());

	SetTimer(self->hWnd, 0, 100, 0);

	self->toolBar.create(self);
	self->toolBar->onTailChange = onTailChangeToolBar;
	self->toolBar->onClear = onClearToolBar;
	(*self->toolBar)->createWindow(self->toolBar, self.ptr(), hInstance, nCmdShow);
	
	self->logControl.create(self);
	self->logControl->onTailChange = onTailChangeLogControl;
	(*self->logControl)->createWindow(self->logControl, self.ptr(), hInstance, nCmdShow);

	ShowWindow(self->hWnd, nCmdShow);
	UpdateWindow(self->hWnd);
}

def_method(LogWindow, LRESULT, windowProc, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message) {
		case WM_SIZE: {
			// set positions of child windows.
			RECT rcClient;
			GetClientRect(self->hWnd, &rcClient);
			RECT rcToolbar;
			GetClientRect(self->toolBar->hWnd, &rcToolbar);
			int toolbarHeight = rcToolbar.bottom - rcToolbar.top;
			SetWindowPos(self->toolBar->hWnd, NULL, 0, 0, rcClient.right - rcClient.left, toolbarHeight, 0);
			SetWindowPos(self->logControl->hWnd, NULL, 0, toolbarHeight, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top - toolbarHeight, 0);
		}
		break;
		default:
			return DefWindowProc(self->hWnd, message, wParam, lParam);
	}
	return 0;
};

def_open(Window, LogWindow)
def_strong(ToolBar, toolBar)
def_strong(LogControl, logControl)
def_bind(LogWindow, constructor)
def_bind(LogWindow, createWindow)
def_bind(LogWindow, windowProc)
def_close(Window, LogWindow)

#endif