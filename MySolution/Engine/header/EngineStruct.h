#pragma once

namespace Engine {

	typedef struct tagVertexTexture {
		XMFLOAT3		position;
		XMFLOAT2		texUV;
	} VTXTEX;

	typedef struct tagFaceIndex16 {
		unsigned short		_0, _1, _2;
	} FACEIDX16;

	typedef struct tagFaceIndex32 {
		unsigned long		_0, _1, _2;
	} FACEIDX32;

}