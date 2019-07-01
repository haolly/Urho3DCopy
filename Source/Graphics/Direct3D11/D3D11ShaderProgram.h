//
// Created by liuhao1 on 2018/1/15.
//

#ifndef URHO3DCOPY_D3D11SHADERPROGRAM_H
#define URHO3DCOPY_D3D11SHADERPROGRAM_H

#include "../../Container/RefCounted.h"
#include "../ShaderVariation.h"
#include "../../Graphics/Graphics.h"

namespace Urho3D
{
	class ShaderProgram : public RefCounted
	{
	public:
		ShaderProgram(Graphics* graphics, ShaderVariation* vertexShader, ShaderVariation* pixelShader)
		{
			// Create needed constant buffers
			const unsigned* vsBufferSizes = vertexShader->GetConstantBufferSize();
			for(unsigned i=0; i<MAX_SHADER_PARAMETER_GROUPS; ++i)
			{
				if(vsBufferSizes[i])
					vsConstantBuffers_[i] = graphics->GetOrCreateConstantBuffer(VS, i, vsBufferSizes[i]);
			}

			const unsigned* psBufferSizes = pixelShader->GetConstantBufferSize();
			for(unsigned i=0; i<MAX_SHADER_PARAMETER_GROUPS; ++i)
			{
				if(psBufferSizes[i])
					psConstantBuffers_[i] = graphics->GetOrCreateConstantBuffer(PS, i, psBufferSizes[i]);
			}

			// Copy parameters, add direct links to constant buffers
			const HashMap<StringHash, ShaderParameter>& vsParams = vertexShader->GetParameters();
			for(auto iter = vsParams.Begin(); iter != vsParams.End(); ++iter)
			{
				parameters_[iter->first_] = iter->second_;
				parameters_[iter->first_].bufferPtr_ = vsConstantBuffers_[iter->second_.buffer_].Get();
			}

			const HashMap<StringHash, ShaderParameter>& psParams = pixelShader->GetParameters();
			for(auto iter = psParams.Begin(); iter != psParams.End(); ++iter)
			{
				parameters_[iter->first_] = iter->second_;
				parameters_[iter->first_].bufferPtr_ = psConstantBuffers_[iter->second_.buffer_].Get();
			}

			//Optimize shader parameter lookup by rehashing to next power of two
			parameters_.Rehash(NextPowerOfTwo(parameters_.Size()));
		}

		virtual ~ShaderProgram() override
		{

		}

		HashMap<StringHash, ShaderParameter> parameters_;
		SharedPtr<ConstantBuffer> vsConstantBuffers_[MAX_SHADER_PARAMETER_GROUPS];
		SharedPtr<ConstantBuffer> psConstantBuffers_[MAX_SHADER_PARAMETER_GROUPS];
	};
}

#endif //URHO3DCOPY_D3D11SHADERPROGRAM_H
