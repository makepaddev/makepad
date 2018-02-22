#pragma once
#ifndef as_header
#define def_LogControl
#endif

#define as_header
#include "Window.cpp"

struct LogEntry {
	LogEntry *pNext;
	LogEntry *pPrev;
};

dec_open(Window, LogControl)
dec_data(int, numLogPerPage)
dec_data(int, numLogEntries)
dec_data(int, newLogEntries)
dec_data(LogEntry*, pLogLast)
dec_data(int, indexLogTop)
dec_data(bool, tailLog)
dec_data(LogEntry*, pLogTop)
dec_data(HFONT, hfontLog)
dec_callbk(void, onTailChange, sLogControl self, bool change)
dec_table(Window, LogControl)
dec_method(LogControl, void, setTailCheck, bool checked)
dec_method(LogControl, void, clearLog)
dec_method(LogControl, void, logBuffer, bool error, const char *pBuf)
dec_method(LogControl, void, createWindow, sWindow parentWindow, HINSTANCE hInstance, int nCmdShow)
dec_method(LogControl, LRESULT, windowProc, UINT message, WPARAM wParam, LPARAM lParam)
dec_close(Window, LogControl)

#ifdef def_LogControl

#define CLASS_LogControl L"LogControlCLASS"

def_override(Base, LogControl, void, constructor) {
	self_override(LogControl)
	self->tailLog = true;
}

def_override(Base, LogControl, void, destructor) {
	self_override(LogControl)
	self->tailLog = true;
}


LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	sLogControl self = (LogControl*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (!self) return DefWindowProc(hWnd, message, wParam, lParam);
	return (*self)->windowProc(self, message, wParam, lParam);
}

#define WM_REDRAW_ALL WM_USER+1

def_method(LogControl, void, logBuffer, bool error, const char *pBuf) {
	
	int size = MultiByteToWideChar(CP_UTF8, MB_COMPOSITE, pBuf, -1, NULL, 0);

	LogEntry *pLogNew = (LogEntry *)selfMalloc(sizeof(LogEntry*) * 2 + sizeof(WCHAR)*size);
	WCHAR *strData = (WCHAR*)(pLogNew + 1);
	pLogNew->pPrev = pLogNew->pNext = 0;

	MultiByteToWideChar(CP_UTF8, MB_COMPOSITE, pBuf, -1, strData, size);

	self->newLogEntries++;
	// we have a log list, just add the log entry
	if (!self->pLogTop) self->pLogTop = pLogNew;
	if (!self->pLogLast) self->pLogLast = pLogNew;
	else {
		pLogNew->pPrev = self->pLogLast;
		self->pLogLast->pNext = pLogNew;
		self->pLogLast = pLogNew;
	}
	if (self->hWnd) PostMessage(self->hWnd, WM_REDRAW_ALL, 0, 0);
}

def_method(LogControl, void, createWindow, sWindow parentWindow, HINSTANCE hInstance, int nCmdShow){
	self->parentWindow = parentWindow;

	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = StaticWindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = CLASS_LogControl;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	self->hfontLog = CreateFont(
		22, 0, 0, 0, 300, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Consolas"
	);

	RegisterClassExW(&wcex);

	self->hWnd = CreateWindowW(
		CLASS_LogControl, L"", WS_CHILD | WS_HSCROLL | WS_VSCROLL, 0, 0,
		800, 600, parentWindow->hWnd, 0, hInstance, 0
	);

	SetWindowLongPtr(self->hWnd, GWLP_USERDATA, (LONG_PTR)self.ptr());

	SetTimer(self->hWnd, 0, 100, 0);
	ShowWindow(self->hWnd, nCmdShow);
	UpdateWindow(self->hWnd);
}

def_method(LogControl, void, setTailCheck, bool checked) {
	self->tailLog = checked;
	if (checked) {
		SCROLLINFO info;
		info.cbSize = sizeof(SCROLLINFO);
		info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
		GetScrollInfo(self->hWnd, SB_VERT, &info);
		info.fMask = SIF_POS | SIF_TRACKPOS;
		info.nTrackPos = info.nPos = info.nTrackPos = info.nMax - (int)info.nPage;
		SetScrollInfo(self->hWnd, SB_VERT, &info, true);
		SendMessage(self->hWnd, WM_VSCROLL, 0, 0);
	}
}

def_method(LogControl, void, clearLog) {
	LogEntry *iter = self->pLogLast, *next;
	while (iter) {
		next = iter->pPrev;
		selfFree(iter);
		iter = next;
	}
	self->pLogTop = self->pLogLast = 0;
	self->numLogEntries = 0;
	self->newLogEntries = 0;
	InvalidateRect(self->hWnd, 0, false);
}

int ctr = 0;
def_method(LogControl, LRESULT, windowProc, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message) {
		case WM_COMMAND: {
			// Parse the menu selections:
			switch (LOWORD(wParam)) {
			case IDC_TAILCHECK:
				self->tailLog = lParam ? true : false;
				break;
			case IDM_EXIT:
				DestroyWindow(self->hWnd);

				break;
			default:
				return DefWindowProc(self->hWnd, message, wParam, lParam);
			}
		}break;
		
		case WM_REDRAW_ALL:
			InvalidateRect(self->hWnd, NULL, FALSE);
			break;

		case WM_TIMER: {
//			SetTimer(self->hWnd, 0, 100, 0);
			selfLog ("Item! %d", ctr++);
		}break;

		case WM_VSCROLL: {
			if (wParam != 0) {
				if (self->tailLog) {
					self->tailLog = false;
					if (self->onTailChange) self->onTailChange(self, self->tailLog);
				}
			}
			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_RANGE | SIF_TRACKPOS | SIF_PAGE;
			GetScrollInfo(self->hWnd, SB_VERT, &info);
			int delta = info.nTrackPos - self->indexLogTop;
			LogEntry *pIter = self->pLogTop;
			if (delta < 0) {
				while (delta < 0 && pIter != 0 && pIter->pPrev != 0) {
					pIter = pIter->pPrev;
					delta++;
				}
			}
			else if (delta>0) {
				while (delta > 0 && pIter != 0 && pIter->pNext != 0) {
					pIter = pIter->pNext;
					delta--;
				}
			}

			self->indexLogTop = info.nTrackPos;
			self->pLogTop = pIter;
			InvalidateRect(self->hWnd, NULL, FALSE);
			info.fMask = SIF_POS;
			info.nPos = info.nTrackPos;
			SetScrollInfo(self->hWnd, SB_VERT, &info, false);
			return 0;
		}break;

		case WM_ERASEBKGND:
			return (LRESULT)1; // Say we handled it.

		case WM_MOUSEWHEEL: {
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_RANGE | SIF_POS;
			GetScrollInfo(self->hWnd, SB_VERT, &info);
			int pos = info.nPos + zDelta / -60;
			if (pos > info.nMax) pos = info.nMax;
			if (pos < info.nMin) pos = info.nMin;
			info.nPos = pos;
			info.nTrackPos = pos;
			info.fMask = SIF_POS | SIF_TRACKPOS;
			SetScrollInfo(self->hWnd, SB_VERT, &info, true);
			SendMessage(self->hWnd, WM_VSCROLL, 0, 0);
			self->tailLog = false;
			if (self->onTailChange) self->onTailChange(self, self->tailLog);
		}break;

		case WM_SIZE: {
			SCROLLINFO info;
			info.cbSize = sizeof(SCROLLINFO);
			info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
			GetScrollInfo(self->hWnd, SB_VERT, &info);
			info.fMask = SIF_PAGE | SIF_RANGE;
			int newMax = self->numLogEntries;
			int doScroll = 0;
			if (info.nPos > info.nMax - (int)info.nPage - 2) {
				info.fMask |= SIF_POS | SIF_TRACKPOS;
				info.nTrackPos =
					info.nPos = info.nTrackPos = info.nMax - (int)info.nPage;
				doScroll = 1;
			}
		}break;

		case WM_PAINT: {
			PAINTSTRUCT ps;

			HDC hdcMain = BeginPaint(self->hWnd, &ps);

			RECT rcClient;
			GetClientRect(self->hWnd, &rcClient);

			HDC hdcMem = CreateCompatibleDC(hdcMain);
			HBITMAP hbmMem = CreateCompatibleBitmap(hdcMain, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);

			HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

			HBRUSH hbrBkGnd = CreateSolidBrush(RGB(32, 32, 32));
			FillRect(hdcMem, &rcClient, hbrBkGnd);
			DeleteObject(hbrBkGnd);

			// TODO: Add any drawing code that uses hdc here...
			HFONT hfontOld = (HFONT)SelectObject(hdcMem, self->hfontLog);
			//SetBkColor(hDc, RGB(32, 32, 32));
			SetBkMode(hdcMem, TRANSPARENT);
			SetTextColor(hdcMem, RGB(196, 196, 196));

			LogEntry *pIter = self->pLogTop;

			TEXTMETRIC tm;
			GetTextMetrics(hdcMem, &tm);
			int cx = tm.tmMaxCharWidth;
			int cy = tm.tmHeight + tm.tmExternalLeading;
			int n = 0;

			int newLogPerPage = (rcClient.bottom - rcClient.top) / cy;
			if (newLogPerPage != self->numLogPerPage || self->newLogEntries != self->numLogEntries) {
				// check if scrollbar is at the bottom, ifso move it with it
				SCROLLINFO info;
				info.cbSize = sizeof(SCROLLINFO);
				info.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
				GetScrollInfo(self->hWnd, SB_VERT, &info);
				info.fMask = SIF_PAGE | SIF_RANGE;
				int newMax = self->numLogEntries;
				int doScroll = 0;
				if (self->tailLog) {
					info.fMask |= SIF_POS | SIF_TRACKPOS;
					info.nTrackPos =
						info.nPos = info.nTrackPos = newMax - self->numLogPerPage + 1;
					doScroll = 1;
				}
				self->numLogEntries = self->newLogEntries;
				self->numLogPerPage = newLogPerPage;

				info.nMin = 0;
				info.nMax = newMax;
				info.nPage = (UINT)self->numLogPerPage;
				SetScrollInfo(self->hWnd, SB_VERT, &info, true);
				if (doScroll) SendMessage(self->hWnd, WM_VSCROLL, 0, 0);
			}

			while (pIter) {
				WCHAR *aItem = (WCHAR*)(pIter + 1);
				int len = lstrlenW(aItem);
				RECT rcText;
				rcText.left = 10;
				rcText.top = n * cy;
				rcText.right = rcText.left + len * cx;
				rcText.bottom = rcText.top + cy;
				DrawText(hdcMem, aItem, -1, &rcText, DT_LEFT | DT_VCENTER);

				pIter = pIter->pNext;
				n++;
			}

			SelectObject(hdcMem, hfontOld);

			BitBlt(hdcMain,
				rcClient.left, rcClient.top,
				rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
				hdcMem,
				0, 0,
				SRCCOPY);

			SelectObject(hdcMem, hbmOld);
			DeleteObject(hbmMem);
			DeleteDC(hdcMem);

			// now we draw upwards from the bottom till we are done
			EndPaint(self->hWnd, &ps);
		}break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(self->hWnd, message, wParam, lParam);
	}
	return 0; 
};

def_open(Window, LogControl)
def_data(int, numLogPerPage)
def_data(int, numLogEntries)
def_data(int, newLogEntries)
def_data(LogEntry*, pLogLast)
def_data(int, indexLogTop)
def_data(bool, tailLog)
def_data(LogEntry*, pLogTop)
def_data(HFONT, hfontLog)
def_bind(LogControl, constructor)
def_bind(LogControl, destructor)
def_bind(LogControl, createWindow)
def_bind(LogControl, windowProc)
def_bind(LogControl, logBuffer)
def_bind(LogControl, setTailCheck)
def_bind(LogControl, clearLog)
def_close(Window, LogControl)

#endif