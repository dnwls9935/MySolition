#pragma once

#include <d3d11.h>
#include <typeinfo>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

using namespace DirectX;

#include <map>
#include <list>
#include <vector>
#include <algorithm>

using namespace std;

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 

#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 

#endif
#endif // _DEBUG

#include "EngineFunction.h"
#include "EngineDefines.h"
#include "EngineTypedef.h"

using namespace Engine;
