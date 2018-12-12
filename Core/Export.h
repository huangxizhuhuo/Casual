/*--------------------------------------------
GameCore Component
Programming Role-Playing Games with DirectX, 2nd Edition
by Jim Adams (Jan 2004)
2007-2008 SCY modified
--------------------------------------------*/
#pragma once

#define DIRECTINPUT_VERSION 0x0800
#define ReleaseCOM(x) if(x) { x->Release(); x = NULL; }

#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>
#include <dsound.h>
#include <dshow.h>

#include "Graphics.h"
#include "Input.h"
#include "Sound.h"
#include "System.h"
