/*
//D3D11_FILL_MODE FillMode;
//D3D11_CULL_MODE CullMode;
//BOOL FrontCounterClockwise;
//INT DepthBias;
//FLOAT DepthBiasClamp;
//FLOAT SlopeScaledDepthBias;
//BOOL DepthClipEnable;
//BOOL ScissorEnable;
//BOOL MultisampleEnable;
//BOOL AntialiasedLineEnable;
*/

RasterizerState CullMode_Default{
	FillMode = SOLID;
	CullMode = BACK;
	FrontCounterClockwise = FALSE;
};

RasterizerState CullMode_None{
	CullMode = NONE;
};

RasterizerState FillMode_WireFrame{
	FillMode = WIREFRAME;
};


//BOOL DepthEnable;
//D3D11_DEPTH_WRITE_MASK DepthWriteMask;
//D3D11_COMPARISON_FUNC DepthFunc;
//BOOL StencilEnable;
//UINT8 StencilReadMask;
//UINT8 StencilWriteMask;
//D3D11_DEPTH_STENCILOP_DESC FrontFace;
//D3D11_DEPTH_STENCILOP_DESC BackFace;

DepthStencilState ZBuffer_Default {
	DepthEnable = TRUE;
	DepthWriteMask = ALL;
	DepthFunc = LESS;
};

DepthStencilState ZBuffer_Disable {
	DepthEnable = TRUE;
	DepthWriteMask = Zero;
	DepthFunc = LESS;
};



//BOOL BlendEnable;
//D3D11_BLEND SrcBlend;
//D3D11_BLEND DestBlend;
//D3D11_BLEND_OP BlendOp;
//D3D11_BLEND SrcBlendAlpha;
//D3D11_BLEND DestBlendAlpha;
//D3D11_BLEND_OP BlendOpAlpha;
//UINT8 RenderTargetWriteMask;

BlendState AlphaBlending {
	BlendEnable[0] = TRUE;
	BlendEnable[1] = TRUE;

	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = ADD;
};

BlendState BlendDisable
{
	BlendEnable[0] = FALSE;	
};