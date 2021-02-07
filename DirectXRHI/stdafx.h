#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN          
#endif

#include <windows.h>

#include <stdlib.h>
#include <sstream>
#include <iomanip>

#include <list>
#include <string>
#include <wrl.h>
#include <memory>
#include <shellapi.h>
#include <atlbase.h>
#include <assert.h>

#include <dxgi1_6.h>
#include <d3d12.h>
#include <atlbase.h>
#include "d3dx12.h"

#ifdef _DEBUG
#include <dxgidebug.h>
#endif