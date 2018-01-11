//
// Created by liuhao1 on 2018/1/11.
//

#include "ShaderVariation.h"

namespace Urho3D
{

	ShaderVariation::ShaderVariation(Shader *owner, ShaderType type) :
		GPUObject(owner->GetSubsystem<Graphics>()),
		owner_(owner),
		type_(type),
		elementHash_(0)
	{
		for(unsigned i=0; i< MAX_TEXTURE_UNITS; ++i)
			useTextureUnit_[i] = false;

		for(unsigned i=0; i< MAX_SHADER_PARAMETER_GROUPS; ++i)
			constantBufferSizes_[i] = 0;
	}

	ShaderVariation::~ShaderVariation()
	{
		Release();
	}


	void ShaderVariation::Release()
	{
		GPUObject::Release();
	}


	void ShaderVariation::SetName(const String &name)
	{
		name_ = name;
	}

	void ShaderVariation::SetDefines(const String &defines)
	{

	}

	Shader *ShaderVariation::GetOwner() const
	{
		return owner_;
	}

	bool ShaderVariation::LoadByteCode(const String &binaryShaderName)
	{
		return false;
	}

	bool ShaderVariation::Compile()
	{
		return false;
	}

	void ShaderVariation::ParseParameters(unsigned char *bufData, unsigned bufferSize)
	{

	}

	void ShaderVariation::SaveByteCode(const String &binaryShaderName)
	{

	}

	void ShaderVariation::CalculateConstantBufferSizes()
	{

	}
}