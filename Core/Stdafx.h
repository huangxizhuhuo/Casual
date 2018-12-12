#pragma once

#define DIRECTINPUT_VERSION 0x0800
#define ReleaseCOM(x) if(x) { x->Release(); x = NULL; }

#include <string>

#include <d3d9.h>
#include <d3dx9.h>
#include <dsound.h>
#include <dinput.h>
#include <dshow.h>
