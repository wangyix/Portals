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

// Renderstates DepthBiasRS and DepthBias2Rs defined in LightHelper.fx

cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;

	float3 gClipPlanePosition;
	float3 gClipPlaneNormal;
	float gClipPlaneOffset;
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float gViewScale;
	float4x4 gTexTransform;
	Material gMaterial;
}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;



// BASIC ******************************************************************************

struct VertexIn
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex     : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float2 Tex     : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}
 
float4 PS(VertexOut pin, uniform bool gClipWithPlane,
			uniform int gLightCount, uniform bool gUseTexture) : SV_Target
{
	// clip everything that's behind the clip plane, offset towards its normal direction by some amount.
	if (gClipWithPlane)
	{
		clip( dot(pin.PosW-gClipPlanePosition, gClipPlaneNormal) - gClipPlaneOffset );
	}


	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;
	
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);
    if(gUseTexture)
	{
		// Sample texture.
		texColor = gDiffuseMap.Sample( samAnisotropic, pin.Tex );
	}
	 
	//
	// Lighting.
	//

	float4 litColor = texColor;
	if( gLightCount > 0  )
	{  
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.  
		[unroll]
		for(int i = 0; i < gLightCount; ++i)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye, 
				A, D, S);

			ambient += A;
			diffuse += D;
			spec    += S;
		}

		// Modulate with late add.
		litColor = texColor*(ambient + diffuse) + spec;
	}

	//
	// Fogging
	//
	float fogT = saturate( (distToEye/gViewScale - FOG_START) / FOG_RANGE);
	litColor = lerp(litColor, FOG_COLOR, fogT);

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;


    return litColor;
}



technique11 Light3
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false, 3, false) ) );

		SetRasterizerState(0);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);
    }
}


technique11 Light3Tex
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false, 3, true) ) );

		SetRasterizerState(0);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);
    }
}

technique11 Light3TexClip
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(true, 3, true) ) );

		SetRasterizerState(0);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);
    }
}



// WIREFRAME ***************************************************************************
RasterizerState WireframeRS
{
	FillMode = Wireframe;
	CullMode = Back;
	FrontCounterClockwise = false;
	DepthBias = 0;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 0.0f;
	DepthClipEnable = true;
	ScissorEnable = false;
	MultisampleEnable = false;
	AntialiasedLineEnable = false;
};

struct VertexOut_Wireframe
{
	float4 PosH    : SV_POSITION;
};
VertexOut_Wireframe VS_Wireframe(VertexIn vin)
{
	VertexOut_Wireframe vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	return vout;
}
float4 PS_Wireframe(VertexOut_Wireframe pin) : SV_Target
{
	return float4(1,1,1,1);
}


technique11 Wireframe
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS_Wireframe() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS_Wireframe() ) );

		SetRasterizerState(WireframeRS);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);
    }
}
