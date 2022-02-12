#include "Shader_RenderState.hpp"

cbuffer Matrices
{
	matrix		g_WorldMatrix  = (matrix)0;
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

cbuffer PickCheck{
	bool			g_PickCheck;
};

cbuffer BarPercent {
	bool			g_LRCheck;
	float			g_BarPercent;
};

cbuffer CrossIsCollision {
	bool			g_CrossCollision;
};

cbuffer CameraDesc {
	vector		g_vCamPosition;
};

/* 1. m_pDeviceContext->DrawIndexed() */
/* 2. �ε����� ������ �ִ� �ε��� ������ �ش��ϴ� ���� ������ �������۷κ��� ���´�. */
/* 3. VS_MAIN�Լ��� ȣ���ϸ鼭 ������ ���� ������ �ϳ��� �������ش�.  */
struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;	
};


/* 1. ������ �����̽� ��ȯ. */
/* 2. �ȼ����� ������ �����͸� �����Ѵ�. */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;	

	// VS_IN In : �������ۿ� ���ǵǾ��ִ� ���������� �״�� �޾ƿ°��̴�. 
	matrix			matWV, matWVP;	

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);
		
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vTexUV = In.vTexUV;	

	return Out;
}


/* SV_POSITION�� ���� ����ó�����ؼ��� ��������.(������ w������ xyzw�� ������.) */
/* ����Ʈ�� ��ȯ�Ѵ�. */
/* �����Ͷ�����.(�ѷ��׿��� ������ ������ �������� �Ͽ�. �ȼ������� �����Ѵ�.) */

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;	
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};


/* 1. �ȼ��� ���� �����Ѵ�. */
// vector PS_MAIN(PS_IN In) : SV_TARGET0
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
/*
	if (Out.vColor.a <= 0.5)
		discard;*/

	return Out;	
};

PS_OUT PS_MAIN_TOOL(PS_IN In) 
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (true == g_PickCheck)
	{
		Out.vColor.rgb += 0.2f;
	}

	if (Out.vColor.a <= 0.5)
		discard;

	return Out;
}

PS_OUT PS_LEFT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (In.vTexUV.x > g_BarPercent)
		discard;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
/*
	if (Out.vColor.a <= 0.5)
		discard;
*/
	return Out;
};
PS_OUT PS_RIGHT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (In.vTexUV.x < 1 - g_BarPercent)
		discard;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
/*
	if (Out.vColor.a <= 0.5)
		discard;*/

	return Out;
};

PS_OUT PS_CROSS(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.r <= 0)
		discard;

	if (true == g_CrossCollision)
		Out.vColor.r += 0.5f;

	return Out;
};

PS_OUT PS_ENEMY_HP(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	if (In.vTexUV.x > g_BarPercent)
		discard;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a <= 0.5)
		discard;

	return Out;
};

cbuffer UVControl {
	float			g_X;
	float			g_Y;
};

VS_OUT VS_HIT_EFFECT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	In.vTexUV.x = (In.vTexUV.x / 6) + (g_X * 0.16);
	In.vTexUV.y = (In.vTexUV.y / 4) + (g_Y * 0.25);

	Out.vTexUV = In.vTexUV;

	return Out;
}

PS_OUT PS_HIT_EFFECT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	if (Out.vColor.a <= 0.9)
		discard;

	return Out;
};


cbuffer AlphaDesc {
	float		g_AlphaValue;
};

VS_OUT VS_AAT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	In.vTexUV.x = (In.vTexUV.x / 4) + (0 * 0.25);
	In.vTexUV.y = (In.vTexUV.y / 4) + (0 * 0.25);

	Out.vTexUV = In.vTexUV;
	return Out;
}

PS_OUT PS_AAT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor.a = (1 - g_AlphaValue);

	if (Out.vColor.r < 0.1 ||
		Out.vColor.b < 0.1 ||
		Out.vColor.g < 0.1)
		discard;

	return Out;
};


technique11			DefaultTechnique
{
	/* ���̴� ����� ĸ��ȭ. */
	/* ������(��Ӱ�, ����ŧ��) + �׸��� + ��ָ��� */
	pass Normal
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������Լ��� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}
	pass Tool 
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������Լ��� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN_TOOL();
	}
	pass LeftUI
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������Լ��� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_LEFT();
	}
	pass RightUI
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������Լ��� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_RIGHT();
	}
	pass CrossSight
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������Լ��� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_CROSS();
	}
	pass EnemyUI
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������Լ��� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_ENEMY_HP();
	}
	pass HitEffect
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������Լ��� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_HIT_EFFECT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_HIT_EFFECT();
	}
	pass AlphaAccTime
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZBuffer_Default, 0);
		SetBlendState(AlphaBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		/* �������Լ��� �����Ѵ�. */
		VertexShader = compile vs_5_0 VS_AAT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_AAT();
	}
}