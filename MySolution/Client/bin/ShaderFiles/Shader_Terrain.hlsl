#include "Shader_RenderState.hpp"

cbuffer Matrices
{
	matrix		g_WorldMatrix = (matrix)0;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;
};

cbuffer CamDesc
{
	vector		g_vCamPosition;
};

cbuffer	BrushDesc {
	vector		g_vBrushPos = { 20.f, 0.f, 10.f, 1.f };
	float		g_fRadius = 5.f;
};

cbuffer LightDesc
{
	vector		g_vLightDirection = vector(1.f, -1.f, 1.f, 0.f);
	vector		g_vLightDiffuse = vector(1.f, 1.f, 1.f, 1.f);
	vector		g_vLightAmbient = vector(1.f, 1.f, 1.f, 1.f);
	vector		g_vLightSpecular = vector(1.f, 1.f, 1.f, 1.f);
};

cbuffer MtrlDesc
{
	vector		g_vMtrlDiffuse = vector(1.f, 1.f, 1.f, 1.f);
	vector		g_vMtrlAmbient = vector(0.4f, 0.4f, 0.4f, 1.f);
	vector		g_vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
};

texture2D	g_DiffuseSourTexture;

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
	float2		vTexUV : TEXCOORD0;
	float		fShade : TEXCOORD1;
	float		fSpecular : TEXCOORD2;
	float4		vWorldPos	: WORLDPOS;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	vector		vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
	vector		vWorldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	Out.fShade = saturate(dot(normalize(g_vLightDirection) * -1.f, normalize(vWorldNormal)));

	vector		vReflect = reflect(normalize(g_vLightDirection), normalize(vWorldNormal));
	vector		vLook = vWorldPosition - g_vCamPosition;

	Out.fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);

	Out.vWorldPos = vWorldPosition;

	return Out;
};


struct RANGE_IN
{
	float4		vWorldPos	: WORLDPOS;
};


struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		fShade : TEXCOORD1;
	float		fSpecular : TEXCOORD2;
	float4		vWorldPos	: WORLDPOS;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;
	RANGE_IN	RANGEIN = (RANGE_IN)0;

	vector	vSourDiffuse = g_DiffuseSourTexture.Sample(DefaultSampler, In.vTexUV * 10.f);

	vector vDiffuse = vSourDiffuse;

	Out.vColor = (g_vLightDiffuse * vDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ (g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;

	return Out;
};


technique11			DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(CullMode_None);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}
	pass FillMode_WireFrame
	{
		SetRasterizerState(FillMode_WireFrame);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}
};
