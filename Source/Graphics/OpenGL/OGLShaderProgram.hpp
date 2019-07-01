//
// Created by liuhao on 2019-04-21.
//

#ifndef URHO3DCOPY_OGLSHADERPROGRAM_HPP
#define URHO3DCOPY_OGLSHADERPROGRAM_HPP

#include "../../Container/RefCounted.h"
#include "../GPUObject.h"
#include "../ShaderVariation.h"

namespace Urho3D
{
	class ShaderProgram : public RefCounted, public GPUObject
	{
	public:
		ShaderProgram(Graphics* graphics, ShaderVariation* vertexShadre, ShaderVariation* pixelShader);

		~ShaderProgram() override;

		void OnDeviceLost() override;
		void Release() override;

		bool Link();

		ShaderVariation* GetVertexShader() const;
		ShaderVariation* GetPixelShader() const;

		bool HasParameter(StringHash param) const;
		bool HasTextureUnit(TextureUnit unit) const
		{
			return useTextureUnits_[unit];
		}

		const ShaderParameter* GetParameter(StringHash param) const;

		const String& GetLinkerOutput() const { return linkerOutput_; }
		const HashMap<Pair<unsigned char, unsigned char>, unsigned >& GetVertexAttributes() const
		{
			return vertexAttributes_;
		}

		unsigned GetUsedVertexAttributes() const
		{
			return usedVertexAttributes_;
		}

		const SharedPtr<ConstantBuffer>* GetConstantBuffers() const
		{
			return &constantBuffers_[0];
		}

		bool NeedParameterUpdate(ShaderParameterGroup group, const void* source);
		void ClearParameterSource(ShaderParameterGroup group);

		static void ClearParameterSources();
		static void ClearGlobalParameterSource(ShaderParameterGroup group);

	private:
		WeakPtr<ShaderVariation> vertexShader_;
		WeakPtr<ShaderVariation> pixelShader_;
		HashMap<StringHash, ShaderParameter> shaderParameters_;
		bool useTextureUnits_[MAX_TEXTURE_UNITS]{};
		// todo, what is this
		HashMap<Pair<unsigned char, unsigned char>, unsigned > vertexAttributes_;
		unsigned usedVertexAttributes_{};
		// todo , why * 2?
		SharedPtr<ConstantBuffer> constantBuffers_[MAX_SHADER_PARAMETER_GROUPS * 2];
		const void* parameterSources_[MAX_SHADER_PARAMETER_GROUPS]{};
		String linkerOutput_;
		unsigned frameNumber_{};

		static unsigned globalFrameNumber;
		static const void* globalParameterSources[MAX_SHADER_PARAMETER_GROUPS];
	};
}



#endif //URHO3DCOPY_OGLSHADERPROGRAM_HPP
