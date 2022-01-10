#include "Shader_RenderState.hpp"

cbuffer Matrices
{
	matrix		g_WorldMatrix = (matrix)0;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;
};

struct BoneMatrixArray
{
	matrix		Bone[128];
};

cbuffer	BoneMatricesBuffer
{
	BoneMatrixArray		g_BoneMatrices;
};

texture2D	g_DiffuseTexture;

sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

cbuffer PickingCheck {
	bool		g_PickModel = false;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight: BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN_STATIC(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;


	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;

}


VS_OUT VS_MAIN_DYNAMIC(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;


	matrix			matWV, matWVP;

	float		fWeightw = 1.f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z);


	matrix			BoneMatrix = g_BoneMatrices.Bone[In.vBlendIndex.x] * In.vBlendWeight.x +
		g_BoneMatrices.Bone[In.vBlendIndex.y] * In.vBlendWeight.y +
		g_BoneMatrices.Bone[In.vBlendIndex.z] * In.vBlendWeight.z +
		g_BoneMatrices.Bone[In.vBlendIndex.w] * In.vBlendWeight.w;


	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	vector		vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);
	vPosition = mul(vPosition, matWVP);

	Out.vPosition = vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	//PS_OUT		Out = (PS_OUT)0;
	//
	//vector pick = vector(0.f, 0.f, 0.f, 1.f);
	//vector texColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	//if (g_PickModel) {
	//	pick = vector(0.2f, 0.2f, 0.f, 1.f);
	//}

	//Out.vColor = texColor + pick;

	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}




technique11			DefaultTechnique
{
	pass StaticMesh
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 진입점함수를 지정한다. */
		VertexShader = compile vs_5_0 VS_MAIN_STATIC();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}
	pass DynamicMesh
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* 진입점함수를 지정한다. */
		VertexShader = compile vs_5_0 VS_MAIN_DYNAMIC();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}
}




