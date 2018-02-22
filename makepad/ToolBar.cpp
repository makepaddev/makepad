#pragma once
#ifndef as_header
#define def_ToolBar
#endif

#define as_header
#include "Window.cpp"

dec_open(Window, ToolBar)
dec_callbk(void, onTailChange, sToolBar self, bool checked)
dec_callbk(void, onClear, sToolBar self)
dec_table(Window, ToolBar)
dec_method(ToolBar, void, setTailCheck, bool checked)
dec_method(ToolBar, void, createWindow, sWindow parent, HINSTANCE hInstance, int nCmdShow)
dec_method(ToolBar, BOOL, windowProc, UINT message, WPARAM wParam, LPARAM lParam)
dec_close(Window, ToolBar)

#ifdef def_ToolBar

INT_PTR CALLBACK StaticWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	sToolBar self = (ToolBar*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (!self) return TRUE;
	return (*self)->windowProc(self, message, wParam, lParam);
}

def_override(Base, ToolBar, void, constructor) {
	self_override(ToolBar)
	
}

def_override(Base, ToolBar, void, destructor) {
	self_override(ToolBar)
	
}

def_method(ToolBar, void, createWindow, sWindow parentWindow, HINSTANCE hInstance, int nCmdShow) {
	self->parentWindow = parentWindow;
	self->hWnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_TOOLBAR), parentWindow->hWnd, (DLGPROC)StaticWndProc, 0);
	SetParent(self->hWnd, parentWindow->hWnd);
	SetWindowLongPtr(self->hWnd, GWLP_USERDATA, (LONG_PTR)self.ptr());
	SendMessage(self->hWnd, WM_INITDIALOG, 0, 0);
	SendDlgItemMessage(self->hWnd, IDC_TAILCHECK, BM_SETCHECK, BST_CHECKED, 0);
	ShowWindow(self->hWnd, nCmdShow);
}

def_method(ToolBar, void, setTailCheck, bool checked) {
	SendDlgItemMessageW(
		self->hWnd, IDC_TAILCHECK, BM_SETCHECK, 
		(WPARAM)(checked?BST_CHECKED:BST_UNCHECKED), 
		(LPARAM)0);
}

def_method(ToolBar, BOOL, windowProc, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {  
		case WM_COMMAND: {
			// which team we are on
			if (wParam == IDC_TAILCHECK) {
				if (self->onTailChange) self->onTailChange(self, SendDlgItemMessage(self->hWnd, IDC_TAILCHECK, BM_GETCHECK, 0, 0)?true:false);
				return TRUE;
			}
			if (wParam == IDC_CLEAR) {
				self->onClear(self);
				return TRUE;
			}
		}
		break;
	}
	return FALSE;
}

def_open(Window, ToolBar)
def_callbk(void, onTailChange, sToolBar self, bool checked)
def_bind(ToolBar, constructor)
def_bind(ToolBar, setTailCheck)
def_bind(ToolBar, createWindow)
def_bind(ToolBar, windowProc)
def_close(Window, ToolBar)

#endif