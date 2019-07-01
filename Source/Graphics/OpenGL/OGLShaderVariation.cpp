//
// Created by liuhao on 2019-05-04.
//
#include "../../Graphics/Shader.h"
#include "../../Graphics/ShaderProgram.hpp"
#include "../../Graphics/ShaderVariation.h"
#include "../../Graphics/Graphics.h"
#include "../ShaderVariation.h"


namespace Urho3D
{
	const char* ShaderVariation::elementSemanticNames[] = {
		"POS",
		"NORMAL",
		"BINORMAL",
		"TANGENT",
		"TEXCOORD",
		"COLOR",
		"BLENDWEIGHT",
		"BLENDINDICES",
		"OBJECTINDEX"
	};


	void ShaderVariation::Release()
	{
		//todo
		GPUObject::Release();
	}

	void ShaderVariation::OnDeviceLost()
	{
		GPUObject::OnDeviceLost();
	}

	bool ShaderVariation::Create()
	{
		//todo
	}

	void ShaderVariation::SetDefines(const String &defines)
	{

	}

}
