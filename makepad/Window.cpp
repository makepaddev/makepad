#pragma once
#ifndef as_header
#define def_Window
#endif

#define as_header
#include "Base.cpp"

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include "resource.h"
  
dec_open(Base, Window)
dec_data(HWND, hWnd)
dec_weak(Window, parentWindow)
dec_table(Base, Window)
dec_close(Base, Window)

#ifdef def_Window

def_override(Base, Window, void, constructor) {
	self_override(Window)
}

def_open(Base, Window)
def_data(HWND, hWnd)
def_weak(Window, parentWindow)
def_bind(Window, constructor)
def_close(Base, Window)
  
#endif