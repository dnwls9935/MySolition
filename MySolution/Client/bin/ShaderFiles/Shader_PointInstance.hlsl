#include "Shader_RenderState.hpp"

cbuffer CameraDesc
{
	vector		g_vCamPosition;
};

cbuffer SnowDesc {
	int			g_X;
	int			g_Y;
};

cbuffer Matrices
{
	matrix		g_WorldMatrix = (matrix)0;
	matrix		g_ViewMatrix;
	matrix		g_ProjMatrix;
};

texture2D	g_DiffuseTexture;

sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};
struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vPSize : PSIZE;

	row_major matrix TransformMatrix : WORLD;
};

struct VS_OUT
{
	float4		vPosition : POSITION0;
	float2		vPSize : PSIZE0;
	bool		vShow : SHOW0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	vector	vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);
	vector	vSpawn = (37.f, 0.f,  21.f, 1.f);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = In.vPSize;


	float Distance = length(vPosition - vSpawn);

	if (50 >= Distance)
		Out.vShow = false;
	else
		Out.vShow = true;

	return Out;
}

struct GS_IN
{
	float4		vPosition : POSITION0;
	float2		vPSize : PSIZE0;
	bool		vShow : SHOW0;
};

struct GS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	bool		vShow : SHOW0;
};


[maxvertexcount(6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
	GS_OUT		Out[6];
	Out[0] = (GS_OUT)0;
	Out[1] = (GS_OUT)0;
	Out[2] = (GS_OUT)0;
	Out[3] = (GS_OUT)0;
	Out[4] = (GS_OUT)0;
	Out[5] = (GS_OUT)0;

	if (true == In[0].vShow)
	{
		vector		vAxisY = vector(0.f, 1.f, 0.f, 0.f);

		vector		vLook = normalize(g_vCamPosition - In[0].vPosition);
		vector		vRight = vector(normalize(cross(vAxisY.xyz, vLook.xyz)), 0.f);
		vector		vUp = vector(normalize(cross(vLook.xyz, vRight.xyz)), 0.f);

		matrix		matVP = mul(g_ViewMatrix, g_ProjMatrix);

		Out[0].vPosition = In[0].vPosition + (vRight * In[0].vPSize.x * 0.06f) +
			(vUp * In[0].vPSize.y * 0.06f);
		Out[0].vTexUV = float2( (g_X - 1) * 0.25f, (g_Y - 1) * 0.25f);
		Out[0].vPosition = mul(Out[0].vPosition, matVP);

		Out[1].vPosition = In[0].vPosition + (vRight * In[0].vPSize.x * -0.06f) +
			(vUp * In[0].vPSize.y * 0.06f);
		Out[1].vTexUV = float2(g_X * 0.25f, (g_Y - 1) * 0.25f);
		Out[1].vPosition = mul(Out[1].vPosition, matVP);

		Out[2].vPosition = In[0].vPosition + (vRight * In[0].vPSize.x * -0.06f) +
			(vUp * In[0].vPSize.y * -0.06f);
		Out[2].vTexUV = float2(g_X * 0.25f, (g_Y * 0.25f));
		Out[2].vPosition = mul(Out[2].vPosition, matVP);

		OutStream.Append(Out[0]);
		OutStream.Append(Out[1]);
		OutStream.Append(Out[2]);
		OutStream.RestartStrip();


		Out[3] = Out[0];

		Out[4] = Out[2];

		Out[5].vPosition = In[0].vPosition + (vRight * In[0].vPSize.x * 0.06f) +
			(vUp * In[0].vPSize.y * -0.06f);
		Out[5].vTexUV = float2((g_X - 1) * 0.25f, (g_Y * 0.25f));
		Out[5].vPosition = mul(Out[5].vPosition, matVP);

		OutStream.Append(Out[3]);
		OutStream.Append(Out[4]);
		OutStream.Append(Out[5]);
		OutStream.RestartStrip();
	}
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	bool		vShow : SHOW0;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

/*
	if (Out.vColor.r == 1 &&
		Out.vColor.b == 1 &&
		Out.vColor.g == 1)
		discard;*/

	return Out;
}

VS_OUT VS_BURROW(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	vector	vPosition = mul(vector(In.vPosition, 1.f), In.TransformMatrix);

	Out.vPosition = mul(vPosition, g_WorldMatrix);
	Out.vPSize = In.vPSize;

	Out.vShow = true;

	return Out;
}


PS_OUT PS_BURROW(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
/*
	Out.vColor.gb = 0.f;
	Out.vColor.r = 1.f;*/

	return Out;
}


technique11			DefaultTechnique
{
	pass Snow
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0  PS_MAIN();
	}
	pass BurrowDust
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_BURROW();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0  PS_BURROW();
	}
}




