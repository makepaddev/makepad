#pragma once
#ifndef as_header
#define def_ProcManager
#endif

#define as_header
#include "Base.cpp"
#include "LogWindow.cpp"

dec_open(Base, ProcManager)
dec_data(HWND, hWnd)
//dec_weak(Base, someWk)
//dec_strong(Base, someStr)
//dec_callbk(void, onSomething, sProcManager self)
dec_table(Base, ProcManager)
dec_method(ProcManager, void, createManager, HINSTANCE hInstance, sLogWindow logWindow)
dec_method(ProcManager, LRESULT, windowProc, UINT message, WPARAM wParam, LPARAM lParam)
dec_close(Base, ProcManager)

#ifdef def_ProcManager

#define CLASS_PROCMANAGER L"MAKEPADPROCMGR"

def_override(Base, ProcManager, void, constructor) {
	self_override(ProcManager)
}

static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	sProcManager self = (ProcManager*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (!self) return DefWindowProc(hWnd, message, wParam, lParam);
	return (*self)->windowProc(self, message, wParam, lParam);
}

def_method(ProcManager, void, createManager, HINSTANCE hInstance, sLogWindow logWindow){
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = StaticWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = 0;
	wcex.hCursor = 0;
	wcex.hbrBackground = 0;
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = CLASS_PROCMANAGER;
	wcex.hIconSm = 0;

	RegisterClassExW(&wcex);

	self->hWnd = CreateWindowW(
		CLASS_PROCMANAGER, L"MAKEPADPROCMGR", 0, 0, 0,
		0, 0, GetDesktopWindow(), 0, hInstance, 0
	);

	SetWindowLongPtr(self->hWnd, GWLP_USERDATA, (LONG_PTR)self.ptr());

}

def_method(ProcManager, LRESULT, windowProc, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		default:
			return DefWindowProc(self->hWnd, message, wParam, lParam);
	}
}

def_open(Base, ProcManager)
def_data(HWND, hWnd)
//def_weak(Base, someWk)
//def_strong(Base, someStr)
//def_callbk(void, onSomething, sProcManager self)
def_bind(ProcManager, constructor)
def_bind(ProcManager, createManager)
def_bind(ProcManager, windowProc)
def_close(Base, Window)
  
#endif