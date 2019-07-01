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


	void ShaderVariation::SetName(const String &name)
	{
		name_ = name;
	}

	Shader *ShaderVariation::GetOwner() const
	{
		return owner_;
	}

	// TODO, glType 传进来的是错误的值，不是 ps/vs，不过反正这个 glType_字段没有用处
	ShaderParameter::ShaderParameter(const String &name, unsigned glType, int location) :
				name_{name},
				glType_{glType},
				location_{location}
	{
	}
}