
texture2D	g_DiffuseTexture;

sampler DefaultSampler = sampler_state
{
	filter = min_mag_mip_linear;
	AddressU = wrap;
	AddressV = wrap;
};

cbuffer LightDesc
{
	vector		g_vLightDir;
	vector		g_vLightPos;
	float		g_fRange;
	vector		g_vLightDiffuse;
	vector		g_vLightAmbient;
	vector		g_vLightSpecular;
};

cbuffer MaterialDesc
{
	vector		g_vMtrlAmbient = (vector)1.f;
	vector		g_vMtrlSpecular = (vector)1.f;
};

cbuffer CameraDesc
{
	vector		g_vCamPosition;
};

cbuffer MatrixInverse
{
	matrix		g_ProjMatrixInv;
	matrix		g_ViewMatrixInv;
};

texture2D	g_NormalTexture;
texture2D	g_ShadeTexture;
texture2D	g_DepthTexture;
texture2D	g_SpecularTexture;

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

VS_OUT VS_MAIN_VIEWPORT(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	Out.vPosition = vector(In.vPosition, 1.f);
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
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	return Out;
}


struct PS_OUT_LIGHTACC
{
	vector		vShade : SV_TARGET0;
	vector		vSpecular : SV_TARGET1;
};


PS_OUT_LIGHTACC PS_MAIN_LIGHTACC_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHTACC		Out = (PS_OUT_LIGHTACC)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.y * 300.f;
	/* 0 ~ 1*/
	/* -1 ~ 1*/
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	float fShade = g_vLightDiffuse * (saturate(dot(normalize(g_vLightDir) * -1.f, vNormal)));
	fShade = (ceil(fShade * 6.f) / 6.f);
	Out.vShade = fShade + (g_vLightAmbient * g_vMtrlAmbient);
	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(g_vLightDir), vNormal);

	vector		vWorldPos;

	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.r;
	vWorldPos.w = 1.0f;

	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);


	vector		vLook = normalize(vWorldPos - g_vCamPosition);

	Out.vSpecular.xyz = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), 30.f);

	return Out;
}

PS_OUT_LIGHTACC PS_MAIN_LIGHTACC_POINT(PS_IN In)
{
	PS_OUT_LIGHTACC		Out = (PS_OUT_LIGHTACC)0;

	vector		vNormalDesc = g_NormalTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vDepthDesc = g_DepthTexture.Sample(DefaultSampler, In.vTexUV);
	float		fViewZ = vDepthDesc.y * 300.f;

	vector		vWorldPos;

	vWorldPos.x = In.vTexUV.x * 2.f - 1.f;
	vWorldPos.y = In.vTexUV.y * -2.f + 1.f;
	vWorldPos.z = vDepthDesc.r;
	vWorldPos.w = 1.0f;

	vWorldPos = vWorldPos * fViewZ;
	vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
	vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	vector		vLightDir = vWorldPos - g_vLightPos;
	float		fDistance = length(vLightDir);

	float		fAtt = saturate((g_fRange - fDistance) / g_fRange);


	/* 0 ~ 1*/
	/* -1 ~ 1*/
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	//Out.vShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	//Out.vShade.a = 1.f;


	float fShade = g_vLightDiffuse * (saturate(dot(normalize(vLightDir) * -1.f, vNormal)));
	fShade = (ceil(fShade * 10.f) / 10.f) * fAtt;
	Out.vShade = fShade + (g_vLightAmbient * g_vMtrlAmbient) ;
	Out.vShade.a = 1.f;

	vector		vReflect = reflect(normalize(vLightDir), vNormal);

	vector		vLook = normalize(vWorldPos - g_vCamPosition);

	Out.vSpecular.xyz = (g_vLightSpecular * g_vMtrlSpecular) * pow(saturate(dot(normalize(vReflect) * -1.f, vLook)), 30.f) * fAtt;

	return Out;
}

struct PS_OUT_BLEND
{
	vector		vColor : SV_TARGET0;
};


PS_OUT_BLEND PS_MAIN_BLEND(PS_IN In)
{
	PS_OUT_BLEND		Out = (PS_OUT_BLEND)0;

	vector		vDiffuseDesc = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vShadeDesc = g_ShadeTexture.Sample(DefaultSampler, In.vTexUV);
	vector		vSpecularDesc = g_SpecularTexture.Sample(DefaultSampler, In.vTexUV);


	Out.vColor = vDiffuseDesc * vShadeDesc + vSpecularDesc;

	if (Out.vColor.a == 0.f)
		discard;

	return Out;
}



BlendState BlendDisable
{
	BlendEnable[0] = false;
};

BlendState OneBlending
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = One;
	DestBlend = One;
	BlendOp = Add;
};


BlendState AlphaBlending
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;

	SrcBlend = Src_Alpha;
	DestBlend = Inv_Src_Alpha;
	BlendOp = Add;
};

RasterizerState CullMode_Default
{
	FillMode = solid;
	CullMode = BACK;
	FrontCounterClockwise = false;
};

DepthStencilState ZDefault
{
	DepthEnable = true;
	DepthWriteMask = all;
	DepthFunc = less;
};

DepthStencilState ZTestDiable
{
	DepthEnable = false;
	DepthWriteMask = zero;
};


technique11			DefaultTechnique
{
	pass Viewport
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZTestDiable, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VIEWPORT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}

	pass Light_Directional
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZTestDiable, 0);
		SetBlendState(OneBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VIEWPORT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN_LIGHTACC_DIRECTIONAL();
	}

	pass Light_Point
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZTestDiable, 0);
		SetBlendState(OneBlending, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VIEWPORT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN_LIGHTACC_POINT();
	}

	pass Blend
	{
		SetRasterizerState(CullMode_Default);
		SetDepthStencilState(ZTestDiable, 0);
		SetBlendState(BlendDisable, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN_VIEWPORT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN_BLEND();
	}

}




