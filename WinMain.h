#ifndef WINMAIN_INCLUDED
#define WINMAIN_INCLUDED

#include <windows.h>
#include <ClbLibMain.h>
#pragma comment(lib, "ClbD3DLib.lib")

#ifdef DEBUG
#include <dxgidebug.h>
#include <dxgi1_3.h>

#pragma comment(lib, "dxguid.lib")
#endif

#endif