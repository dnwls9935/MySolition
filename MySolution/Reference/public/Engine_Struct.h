#pragma once

namespace Engine
{
	typedef struct MeshMaterial
	{
		class CTexture*		pMeshTexture[AI_TEXTURE_TYPE_MAX];
	}MESHMATERIAL;

	typedef struct tagVertex_Mesh
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
		XMFLOAT3		vTangent;
		XMUINT4			vBlendIndex;
		XMFLOAT4		vBlendWeight;
	}VTXMESH;

	typedef struct tagLightDesc
	{
		XMFLOAT3		vDirection;

		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
	}LIGHTDESC;

	typedef struct tagMtrlDesc
	{
		XMFLOAT4		vDiffuse;
		XMFLOAT4		vAmbient;
		XMFLOAT4		vSpecular;
		XMFLOAT4		vEmissive;
	}MTRLDESC;

	typedef struct tagVertex_Matrix
	{
		XMFLOAT4		vRight;
		XMFLOAT4		vUp;
		XMFLOAT4		vLook;
		XMFLOAT4		vPosition;
	} VTXMATRIX;

	typedef struct tagVertex_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT2		vTexUV;
	} VTXTEX;

	typedef struct tagVertex_Texture_Cube
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vTexUV;
	} VTXTEXCUBE;

	typedef struct tagVertex_Normal_Texture
	{
		XMFLOAT3		vPosition;
		XMFLOAT3		vNormal;
		XMFLOAT2		vTexUV;
	} VTXNORTEX;

	typedef struct tagLineIndices_16
	{
		unsigned short		_0, _1;
	} LINEINDICES16;

	typedef struct tagFaceIndices_16
	{
		unsigned short		_0, _1, _2;
	} FACEINDICES16;

	typedef struct tagFaceIndices_32
	{
		unsigned long		_0, _1, _2;
	} FACEINDICES32;
}


