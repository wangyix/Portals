//=============================================================================
// Basic.fx by Frank Luna (C) 2011 All Rights Reserved.
//
// Basic effect that currently supports transformations, lighting, and texturing.
//=============================================================================

#include "LightHelper.fx"


SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = WRAP;
	AddressV = WRAP;
};

cbuffer cbPerFrame
{
	float3 gClipPlanePosition;
	float3 gClipPlaneNormal;
	float gClipPlaneOffset;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldViewProj;
}; 


struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
};



// PORTAL BOX **********************************************************************************************

struct VertexOut_Box
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION; // necessary for clipping
};



VertexOut_Box VS_Box(VertexIn vin, uniform bool gClearDepth)
{
	VertexOut_Box vout;
	
	// Transform to world space space.
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// set depth to max value
	if (gClearDepth)
	{
		vout.PosH.z = vout.PosH.w;
	}

	return vout;
}



float4 PS_Box(VertexOut_Box pin, uniform bool gClipWithPlane) : SV_TARGET
{
	if (gClipWithPlane)
	{
		clip( dot(pin.PosW-gClipPlanePosition, gClipPlaneNormal) - gClipPlaneOffset );
	}
	return FOG_COLOR;
	//return float4(0,0,1,1);
}

/*
// FOR TESTINH!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
float4 PS_Box2(VertexOut_Box pin, uniform bool gClipWithPlane) : SV_TARGET
{
	if (gClipWithPlane)
	{
		clip( dot(pin.PosW-gClipPlanePosition, gClipPlaneNormal) - gClipPlaneOffset );
	}

	return float4(1,0,0,1);
}*/


technique11 PortalBox
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Box(false) ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Box(false) ) );

		SetRasterizerState(DepthBiasRS);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);	
	}
}

technique11 PortalBoxClip
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Box(false) ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Box(true) ) );

		SetRasterizerState(DepthBiasRS);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);	
	}
}

technique11 PortalBoxClearDepth
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS_Box(true) ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Box(false) ) );

		SetRasterizerState(0);	// no depthbias since all pixels will have max depth
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);	
	}
}