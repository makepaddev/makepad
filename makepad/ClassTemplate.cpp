#pragma once
#ifndef as_header
#define def_Template
#endif

#define as_header
#include "Base.cpp"

dec_open(Base, Template)
//dec_data(int, someInt)
//dec_weak(Base, someWk)
//dec_strong(Base, someStr)
//dec_callbk(void, onSomething, sTemplate self)
dec_table(Base, Template)
dec_method(Template, void, methodThing, bool arg)
dec_close(Base, Template)

#ifdef def_Template

def_override(Base, Template, void, constructor) {
	self_override(Template)
}

def_method(Template, void, methodThing, bool arg) {
}

def_open(Base, Template)
//def_data(int, someInt)
//def_weak(Base, someWk)
//def_strong(Base, someStr)
//def_callbk(void, onSomething, sTemplate self)
def_bind(Template, constructor)
def_bind(Template, methodThing)
def_close(Base, Window)
  
#endif