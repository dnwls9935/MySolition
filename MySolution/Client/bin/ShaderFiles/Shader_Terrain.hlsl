
#include "Shader_RenderState.hpp"

cbuffer Matrices
{
	matrix		g_WorldMatrix = (matrix)0;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;
};

cbuffer BrushDesc
{
	vector		g_vBrushPos = vector(20.f, 0.f, 10.f, 1.f);
	float		g_fRadius = 10.f;
};

texture2D	g_DiffuseSourTexture;
texture2D	g_DiffuseDestTexture;
texture2D	g_FilterTexture;
texture2D	g_BrushTexture;

sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;

};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vWorldPos = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vProjPos = Out.vPosition;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vWorldPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vDiffuse : SV_TARGET0;
	vector		vNormal : SV_TARGET1;
	vector		vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vSourDiffuse = g_DiffuseSourTexture.Sample(DefaultSampler, In.vTexUV * 10.f);
	vector	vDestDiffuse = g_DiffuseDestTexture.Sample(DefaultSampler, In.vTexUV * 20.f);
	vector	vFilterColor = g_FilterTexture.Sample(DefaultSampler, In.vTexUV);

	vector	vBrushColor = (vector)0;

	if (g_vBrushPos.x - g_fRadius <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPos.x + g_fRadius &&
		g_vBrushPos.z - g_fRadius <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPos.z + g_fRadius)
	{
		float2		vTexUV = float2((In.vWorldPos.x - (g_vBrushPos.x - g_fRadius)) / (2.f * g_fRadius),
			((g_vBrushPos.z + g_fRadius) - In.vWorldPos.z) / (2.f * g_fRadius));
		vBrushColor = g_BrushTexture.Sample(DefaultSampler, vTexUV);

	}

	Out.vDiffuse = vSourDiffuse * vFilterColor.r + vDestDiffuse * (1.f - vFilterColor.r) + vBrushColor;
	Out.vDiffuse.a = 1.f;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

	return Out;
}




technique11			DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}
}





