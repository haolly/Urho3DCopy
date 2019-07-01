//
// Created by liuhao1 on 2018/1/11.
//

#ifndef URHO3DCOPY_SHADERVARIATION_H
#define URHO3DCOPY_SHADERVARIATION_H

#include "GPUObject.h"
#include "GraphicsDefs.h"
#include "Shader.h"

namespace Urho3D
{
	class ConstantBuffer;
	class Shader;

	struct ShaderParameter
	{
		ShaderParameter() = default;
		ShaderParameter(const String& name, unsigned glType, int location);
		//todo

		ShaderType type_{};
		String name_{};

		// Todo, how do I decide which fields to put in the same union ?
		union
		{
			// Offset in constant buffer
			unsigned offset_;
			// OpenGL uniform location
			int location_;
			// Direct3D9 register index
			unsigned register_;
		};

		union
		{
			// Parameter size. Used only on Direct3D11 to calculate constant buffer size
			unsigned size_;
			// Parameter OpenGL type
			unsigned glType_;
			// Number of registers on Direct3D9
			unsigned regCount_;
		};

		// todo, ShaderParameterGroup ??
		// The offset of constant buffer ??
		unsigned buffer_{};
		// Constant buffer pointer. Defined only in shader programs
		ConstantBuffer* bufferPtr_{};
	};

	class ShaderVariation : public RefCounted, public GPUObject
	{
	public:
		ShaderVariation(Shader* owner, ShaderType type);

		~ShaderVariation() override ;

		void OnDeviceLost() override ;

		void Release() override ;

		bool Create();
		void SetName(const String& name);
		void SetDefines(const String& defines);

		Shader* GetOwner() const;
		ShaderType GetShaderType() const { return type_; }
		const String& GetName() const { return name_; }

		String GetFullName() const { return name_ + "(" + defines_ + ")"; }

		bool HasParameter(StringHash param) const
		{
			return parameters_.Contains(param);
		}

		bool HasTextureUnit(TextureUnit unit) const
		{
			return useTextureUnit_[unit];
		}

		const HashMap<StringHash, ShaderParameter>& GetParameters() const
		{
			return parameters_;
		};

		unsigned long long GetElementHash() const
		{
			return elementHash_;
		}

		const PODVector<unsigned char>& GetByteCode() const { return byteCode_; }

		const String& GetDefines() const { return defines_; }

		const String& GetCompilerOutput() const { return compilerOutput_; }
		// note, equals return constantBufferSizes
		const unsigned* GetConstantBufferSize() const { return &constantBufferSizes_[0]; }
		const String& GetDefinesClipPlane() const { return definesClipPlane_; }
		static const char* elementSemanticNames[];

	private:
		bool LoadByteCode(const String& binaryShaderName);
		bool Compile();
		void ParseParameters(unsigned char* bufData, unsigned bufferSize);
		void SaveByteCode(const String& binaryShaderName);
		void CalculateConstantBufferSizes();

		WeakPtr<Shader> owner_;
		ShaderType type_;
		// Note, Vertex element hash for vertex shader, Zero for pixel shaders. Note that hashing is different than vertex buffers
		//todo, Have a purpose ?
		unsigned long long elementHash_;
		// todo, where is this value set?
		HashMap<StringHash, ShaderParameter> parameters_;
		bool useTextureUnit_[MAX_TEXTURE_UNITS];
		// todo, Have a purpose?
		unsigned constantBufferSizes_[MAX_SHADER_PARAMETER_GROUPS];
		PODVector<unsigned char> byteCode_;
		String name_;
		String defines_;
		String definesClipPlane_;
		String compilerOutput_;
	};
}



#endif //URHO3DCOPY_SHADERVARIATION_H
