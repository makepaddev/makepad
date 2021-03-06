#define as_header
#include "LogWindow.cpp"
#include "ProcManager.cpp"
//#include "JSProcess.cpp"

#pragma comment(lib, "shcore.lib")
#include <ShellScalingAPI.h>

#ifdef _DEBUG
#pragma comment(lib, "ChakraCore\\Build\\VcBuild\\bin\\x64_debug\\ChakraCore.lib")
#else
#pragma comment(lib, "ChakraCore\\Build\\VcBuild\\bin\\x64_release\\ChakraCore.lib")
#endif
#include "ChakraCore\lib\Jsrt\ChakraCore.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

void* localMalloc(Base *self, const char *file, int line, size_t size) {
	return malloc(size);
};

void localFree(Base *self, const char *file, int line, void *block) {
	return free(block);
};

#define MAX_TEXT_BUFFER 65536

sLogWindow logWindow;

void localLogImpl(Base *self, const char *file, int line, bool error, const char *fmt, va_list &argList) {
	sLogControl logControl = logWindow->logControl;
	
	char buf[MAX_TEXT_BUFFER];
	vsnprintf_s(buf, MAX_TEXT_BUFFER, MAX_TEXT_BUFFER, fmt, argList);
	char *beginLine = buf;
	for (int i = 0; i < MAX_TEXT_BUFFER; i++) {
		if (buf[i] == '\r') buf[i] = ' ';
		if (buf[i] == '\n') {
			buf[i] = 0;
			//OutputDebugStringA(beginLine);
			(*logControl)->logBuffer(logControl, error, beginLine);
			beginLine = &buf[i + 1];
		}
		else if (buf[i] == 0) {
			//OutputDebugStringA(beginLine);
			(*logControl)->logBuffer(logControl, error, beginLine);
			//pRootWindow->logFrame.Log(iter);
			break;
		}
	}
}

void localError(Base *self, const char *file, int line, const char *fmt, ...) {
	va_list argList;
	va_start(argList, fmt);
	localLogImpl(self, file, line, true, fmt, argList);
	va_end(argList);
}

#undef selfLog
#define selfLog(...) logWindow->_vt->log(logWindow.ptr(), __FILE__, __LINE__, __VA_ARGS__);

void localLog(Base *self, const char *file, int line, const char *fmt, ...) {
	va_list argList;
	va_start(argList, fmt);
	localLogImpl(self, file, line, false, fmt, argList);
	va_end(argList);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

	CoInitialize(NULL);

	def_vtable_fn defaultTable[] = {
		vtable_Base,
		vtable_Window,
		vtable_LogControl,
		vtable_ToolBar,
		vtable_LogWindow,
		vtable_ProcManager,
		0
	};
	sBase root = vtable_LoadFromArray(
		defaultTable, 
		localMalloc, localFree, localLog, localError
	);

	// create a log window
	logWindow.create(root);

	//logWindow = 0;

	(*logWindow)->createWindow(logWindow, hInstance, nCmdShow);

	sProcManager procManager;
	procManager.create(root);
	(*procManager)->createManager(procManager, hInstance, logWindow);

	selfLog("HELLO WORLD");  
	//RootWindow rootWindow;
	//pRootWindow = &rootWindow;
	//rootWindow.Create(hInstance, nCmdShow);
	//JsRuntimeHandle runtime;
	//JsContextRef context;
	//JsValueRef result;
	//unsigned currentSourceContext = 0;
	/*
	// Your script; try replace hello-world with something else
	std::wstring script = L"(()=>{return \'Hello world!\';})()";

	// Create a runtime.
	JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime);

	// Create an execution context.
	JsCreateContext(runtime, &context);

	// Now set the current execution context.
	JsSetCurrentContext(context);

	// Run the script.
	JsRunScript(script.c_str(), currentSourceContext++, L"", &result);

	// Convert your script result to String in JavaScript; redundant if your script returns a String
	JsValueRef resultJSString;
	JsConvertValueToString(result, &resultJSString);

	// Project script result back to C++.
	const wchar_t *resultWC;
	size_t stringLength;
	JsStringToPointer(resultJSString, &resultWC, &stringLength);

	std::wstring resultW(resultWC);
	std::cout << std::string(resultW.begin(), resultW.end()) << std::endl;
	system("pause");

	// Dispose runtime
	JsSetCurrentContext(JS_INVALID_REFERENCE);
	JsDisposeRuntime(runtime);
	*/
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MAKEPAD));

	MSG msg;
	//for (auto i = 0; i < 1; i++) {
		//Log("Logging line %d",i);
	//}

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
