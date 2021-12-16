
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
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		fShade : TEXCOORD1;
	float		fSpecular : TEXCOORD2;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV * 20.f;

	vector		vWorldNormal = mul(vector(In.vNormal, 0.f), g_WorldMatrix);
	vector		vWorldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	Out.fShade = saturate(dot(normalize(g_vLightDirection) * -1.f, normalize(vWorldNormal)));

	vector		vReflect = reflect(normalize(g_vLightDirection), normalize(vWorldNormal));
	vector		vLook = vWorldPosition - g_vCamPosition;

	Out.fSpecular = pow(saturate(dot(normalize(vReflect) * -1.f, normalize(vLook))), 30.f);


	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexUV : TEXCOORD0;
	float		fShade : TEXCOORD1;
	float		fSpecular : TEXCOORD2;
};

struct PS_OUT
{
	vector		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(DefaultSampler, In.vTexUV);

	Out.vColor = (g_vLightDiffuse * vMtrlDiffuse) * saturate(In.fShade + (g_vLightAmbient * g_vMtrlAmbient))
		+ (g_vLightSpecular * g_vMtrlSpecular) * In.fSpecular;

	return Out;
}




technique11			DefaultTechnique
{
	pass Default
	{
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0  PS_MAIN();
	}
}




