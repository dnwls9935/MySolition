#pragma once

#pragma warning (disable : 4251)

#include <d3d11.h>
#include <typeinfo>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#include "d3dx11effect.h"
#include "d3dcompiler.h"
#include "DirectXTex.h"

using namespace DirectX;

#include <map>
#include <unordered_map>
#include <list>
#include <vector>
#include <algorithm>

using namespace std;


#ifndef _AFX
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#endif
#endif // _DEBUG
#endif


#include "EngineFunction.h"
#include "EngineDefines.h"
#include "EngineTypedef.h"
#include "EngineStruct.h"

using namespace Engine;
