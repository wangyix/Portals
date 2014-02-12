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


SamplerState samAnisotropicBlackBorder
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 4;

	AddressU = BORDER;
	AddressV = BORDER;

	BorderColor = float4(0, 0, 0, 0);
};



// Renderstates DepthBiasRS and DepthBias2Rs defined in LightHelper.fx

cbuffer cbPerFrame
{
	DirectionalLight gDirLights[3];
	float3 gEyePosW;

	float3 gClipPlanePosition;
	float3 gClipPlaneNormal;
	float gClipPlaneOffset;

	float4x4 gPortalA;			// room-to-portalA transform, includes scaling by portal PhysicalRadius
	float gPortalATexRadRatio;	// portalA's TextureRadiusRatio
	float4x4 gPortalB;			// room-to-portalB transform, includes scaling by portal PhysicalRadius
	float gPortalBTexRadRatio;	// portalB's TextureRadiusRatio
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

Texture2D gDiffuseMap;

Texture2D gPortalADiffuseMap;
Texture2D gPortalBDiffuseMap;



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
	float2 Tex     : TEXCOORD0;
	
	float3 PosPA	: POSITION1;		// position in portalA space
	float3 PosPB	: POSITION2;		// position in portalB space
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

	// transform from room's object space to portal space of both portals
	float4 PosP;
	PosP = mul(float4(vin.PosL, 1.0f), gPortalA);
	vout.PosPA = PosP.xyz / PosP.w;
	PosP = mul(float4(vin.PosL, 1.0f), gPortalB);
	vout.PosPB = PosP.xyz / PosP.w;

	return vout;
}
 
float4 PS(VertexOut pin, uniform bool gClipWithPlane, 
		uniform bool gDrawPortalA, uniform bool gDrawPortalB, uniform bool gDrawHoles,
		uniform int gLightCount, uniform bool gUseTexture) : SV_Target
{
	// clip everything that's behind the clip plane, offset towards its normal direction by some amount.
	if (gClipWithPlane)
	{
		clip( dot(pin.PosW-gClipPlanePosition, gClipPlaneNormal) - gClipPlaneOffset );
	}

	// the room color will be blended with this using PortalColor's alpha right before fogging.
	float4 PortalColor = float4(0, 0, 0, 0);

	// check if this point is in the hole/ring of PortalA
	if (gDrawPortalA)
	{
		if (abs(pin.PosPA.z) <= 0.001f)		// check if this point is in the portal plane
		{
			if (gDrawHoles)
				clip( length(pin.PosPA.xy) - 1.0f );	// drop this pixel if it's within the portal hole
			
			// calculate texcoord of this point for the portal texture
			// x:[-TexRatio, TexRatio] -> u:[1, 0]
			// y:[-TexRatio, TexRatio] -> v:[1, 0]
			float2 TexPA = (1.0f - pin.PosPA.xy / gPortalATexRadRatio) / 2.0f;
			PortalColor += gPortalADiffuseMap.Sample(samAnisotropicBlackBorder, TexPA);
		}
	}
	if (gDrawPortalB)
	{
		if (abs(pin.PosPB.z) <= 0.001f)		// check if this point is in the portal plane
		{
			if (gDrawHoles)
				clip( length(pin.PosPB.xy) - 1.0f );	// drop this pixel if it's within the portal hole
			
			// calculate texcoord of this point for the portal texture
			// x:[-TexRatio, TexRatio] -> u:[1, 0]
			// y:[-TexRatio, TexRatio] -> v:[1, 0]
			float2 TexPB = (1.0f - pin.PosPB.xy / gPortalBTexRadRatio) / 2.0f;
			PortalColor += gPortalBDiffuseMap.Sample(samAnisotropicBlackBorder, TexPB);
		}
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

	// blend the current color with PortalColor
	if ( gDrawPortalA || gDrawPortalB )
	{
		litColor.rgb = PortalColor.a * PortalColor.rgb + (1.0f-PortalColor.a) * litColor.rgb;
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




technique11 Light3TexPortalAClip
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(true, true, false, true, 3, true) ) );

		SetRasterizerState(0);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);
    }
}

technique11 Light3TexPortalBClip
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(true, false, true, true, 3, true) ) );

		SetRasterizerState(0);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);
    }
}

technique11 Light3TexPortalAPortalB
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false, true, true, true, 3, true) ) );

		SetRasterizerState(0);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);
    }
}

technique11 Light3TexPortalAPortalBClip
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(true, true, true, true, 3, true) ) );

		SetRasterizerState(0);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);
    }
}

technique11 Light3TexPortalAPortalBNoHoles
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false, true, true, false, 3, true) ) );

		SetRasterizerState(0);
		SetBlendState(0, float4(0,0,0,0), 0xffffffff);
    }
}