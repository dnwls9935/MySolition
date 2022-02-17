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

cbuffer CamDesc {
	vector g_vCamPosition = (vector)0;
	bool g_RimLight = false;
};

cbuffer BurnDesc {
	bool g_Burned = false;
	float g_Time = 0.f;
	float3 g_BurnColor = (float3)0;
};

texture2D	g_DiffuseTexture;
texture2D	g_BurnedTexture;

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
	float3		vTangent : TANGENT;
	uint4		vBlendIndex : BLENDINDEX;
	float4		vBlendWeight: BLENDWEIGHT;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;

	float4		vRimPos : RIM;
};

VS_OUT VS_MAIN_STATIC(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;


	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

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

	vector		vNormal = mul(vector(In.vNormal, 0.f), BoneMatrix);
	Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	vector Rim = mul(vector(In.vPosition, 1.f), BoneMatrix);
	Rim = mul(Rim, g_WorldMatrix);
	Out.vRimPos = Rim;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;

	float4		vRimPos : RIM;
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

	Out.vDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	
	if (true == g_RimLight)
	{
		float Rim = 0;
		vector ViewDir = normalize(g_vCamPosition - In.vRimPos);

		Rim = 1 - saturate(dot(In.vNormal, ViewDir));

		Rim = pow(Rim, 6.f);

		float3 RimColor = float3(0.8f, 0.2f, 0.1f);

		RimColor = Rim * RimColor;

		Out.vDiffuse += float4(RimColor, 1);
		Out.vDiffuse.a = 1.f;
	}
	if (true == g_Burned)
	{
		vector Burned = g_BurnedTexture.Sample(DefaultSampler, In.vTexUV);

		if (Burned.r <= g_Time + 0.2f)
		{
			Out.vDiffuse.r += g_BurnColor.r;
			Out.vDiffuse.g += g_BurnColor.g;
			Out.vDiffuse.b += g_BurnColor.b;
		}
		
		if (Burned.r <= g_Time)
			discard;
	}

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 300.0f, 0.0f, 0.0f);

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

	pass AnimMesh
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