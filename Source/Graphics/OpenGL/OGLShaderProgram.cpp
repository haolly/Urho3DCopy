//
// Created by liuhao on 2019-04-21.
//
#include "../../Graphics/Graphics.h"
#include "../../Graphics/ShaderProgram.hpp"
#include "../../Graphics/GraphicsImpl.hpp"
#include "../../Graphics/ShaderVariation.h"
#include "OGLShaderProgram.hpp"


namespace Urho3D
{
	static const char* shaderParameterGroups[] = {
			"frame",
			"camera",
			"zone",
			"light",
			"material",
			"object",
			"custom"
	};

	//todo

	ShaderProgram::ShaderProgram(Graphics *graphics, ShaderVariation *vertexShader, ShaderVariation *pixelShader) :
								GPUObject(graphics),
								vertexShader_(vertexShader),
								pixelShader_(pixelShader)
	{
		for(auto& parameterSource : parameterSources_)
			parameterSource = (const void*)M_MAX_UNSIGNED;
	}

	const ShaderParameter *ShaderProgram::GetParameter(StringHash param) const
	{
		auto i = shaderParameters_.Find(param);
		if(i != shaderParameters_.End())
			return &i->second_;
		else
			return nullptr;
	}

	ShaderProgram::~ShaderProgram()
	{
		Release();
	}

	void ShaderProgram::Release()
	{
		if(object_.name_)
		{
			if(!graphics_)
				return;
			if(!graphics_->IsDeviceLost())
			{
				if(graphics_->GetShaderProgram() == this)
					graphics_->SetShaders(nullptr, nullptr);

				glDeleteProgram(object_.name_);
			}

			object_.name_ = 0;
			linkerOutput_.Clear();
			shaderParameters_.Clear();
			vertexAttributes_.Clear();
			usedVertexAttributes_ = 0;

			for(bool& useTextureUnit : useTextureUnits_)
				useTextureUnit = false;

			for(unsigned i=0; i< MAX_SHADER_PARAMETER_GROUPS; ++i)
				constantBuffers_[i].Reset();
		}
	}

	void ShaderProgram::OnDeviceLost()
	{
		GPUObject::OnDeviceLost();

		if(graphics_ && graphics_->GetShaderProgram() == this)
			graphics_->SetShaders(nullptr, nullptr);

		linkerOutput_.Clear();
	}

	bool ShaderProgram::Link()
	{
		Release();

		if(!vertexShader_ || !pixelShader_ || !vertexShader_->GetGPUObjectName() || !pixelShader_->GetGPUObjectName())
			return false;

		object_.name_ = glCreateProgram();
		if(!object_.name_)
		{
			linkerOutput_ = "Could not create shader program";
			return false;
		}

		glAttachShader(object_.name_, vertexShader_->GetGPUObjectName());
		glAttachShader(object_.name_, pixelShader_->GetGPUObjectName());
		glLinkProgram(object_.name_);

		int linked, length;
		glGetProgramiv(object_.name_, GL_LINK_STATUS, &linked);
		if(!linked)
		{
			glGetProgramiv(object_.name_, GL_INFO_LOG_LENGTH, &length);
			linkerOutput_.Resize((unsigned)length);
			int ouLenght;
			glGetProgramInfoLog(object_.name_, length, &ouLenght, &linkerOutput_[0]);
			glDeleteProgram(object_.name_);
			object_.name_ = 0;
		}
		else
			linkerOutput_.Clear();

		if(!object_.name_)
			return false;

		const int MAX_NAME_LENGTH = 256;
		char nameBuffer[MAX_NAME_LENGTH];
		int attributeCount, uniformCount, elementCount, nameLenght;
		GLenum type;

		glUseProgram(object_.name_);

		//Check for vertex attributes
		glGetProgramiv(object_.name_, GL_ACTIVE_ATTRIBUTES, &attributeCount);
		for(int i=0; i < attributeCount; ++i)
		{
			// note, type is what ? GL_FLOAT, GL_FLOAT_VEC2 etc
			glGetActiveAttrib(object_.name_, i, (GLsizei)MAX_NAME_LENGTH, &nameLenght, &elementCount, &type, nameBuffer);

			String name = String(nameBuffer, nameLenght);
			VertexElementSemantic semantic = MAX_VERTEX_ELEMENT_SEMANTICS;
			unsigned char semanticIndex = 0;

			//Go in reverse order so that "binormal" is detected before "normal"
			for(unsigned j= MAX_VERTEX_ELEMENT_SEMANTICS -1; j < MAX_VERTEX_ELEMENT_SEMANTICS; --j)
			{
				//todo, 这里的语义名字是怎么来的？ 固定写死在 shader 里面的吗？ yes, ref Transform.glsl
				// ref https://urho3d.github.io/documentation/HEAD/_shaders.html
				if(name.Contains(ShaderVariation::elementSemanticNames[j], false))
				{
					semantic = (VertexElementSemantic)j;
					unsigned index = NumberPostfix(name);
					if(index != M_MAX_UNSIGNED)
						semanticIndex = (unsigned char)index;
					break;
				}
			}

			if(semantic == MAX_VERTEX_ELEMENT_SEMANTICS)
			{
				URHO3D_LOGWARNING("Found vertex attribute " + name + " with no know semantic in shader program " +
				vertexShader_->GetFullName() + " " + pixelShader_->GetFullName());
				continue;
			}

			int location = glGetAttribLocation(object_.name_, name.CString());
			vertexAttributes_[MakePair((unsigned char)semantic, semanticIndex)] = location;
			usedVertexAttributes_ |= (1u << location);
		}

		// Check for constant buffers
#ifndef GL_ES_VERSION_2_0
		HashMap<unsigned, unsigned> blockToBinding;

		if(Graphics::GetGL3Support())
		{
			int numUniformBlocks = 0;
			glGetProgramiv(object_.name_, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlocks);
			for(int i=0; i< numUniformBlocks; ++i)
			{
				glGetActiveUniformBlockName(object_.name_, (GLuint)i, MAX_NAME_LENGTH, &nameLenght, nameBuffer);
				String name(nameBuffer, nameLenght);
				unsigned blockIndex = glGetUniformBlockIndex(object_.name_, name.CString());
				unsigned group = M_MAX_UNSIGNED;

				for(unsigned j=0; j< MAX_SHADER_PARAMETER_GROUPS; ++j)
				{
					if(name.Contains(shaderParameterGroups[j], false))
					{
						group = j;
						break;
					}
				}

				if(group == M_MAX_UNSIGNED)
					group = NumberPostfix(name);

				if(group >= MAX_SHADER_PARAMETER_GROUPS)
				{
					URHO3D_LOGWARNING("Skipping unrecognized uniform block " + name + " in shader program " +
					vertexShader_->GetFullName() + " " + pixelShader_->GetFullName());
					continue;
				}

				int dataSize;
				glGetActiveUniformBlockiv(object_.name_, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &dataSize);
				if(!dataSize)
					continue;

				//NOTE: 这个 bindingIndex 是随便设置的呀
				unsigned bindingIndex = group;
				ShaderType shaderType = VS;
				if(name.Contains("PS", false))
				{
					bindingIndex += MAX_SHADER_PARAMETER_GROUPS;
					shaderType = PS;
				}

				//note: what does this do ?
				// ref https://learnopengl.com/Advanced-OpenGL/Advanced-GLSL UBO
				glUniformBlockBinding(object_.name_, blockIndex, bindingIndex);
				blockToBinding[blockIndex] = bindingIndex;

				constantBuffers_[bindingIndex] = graphics_->GetOrCreateConstantBuffer(shaderType, bindingIndex, (unsigned)dataSize);
			}
		}
#endif
		// Check for shader parameters and texture units
		glGetProgramiv(object_.name_, GL_ACTIVE_UNIFORMS, &uniformCount);
		for(int i=0; i< uniformCount; ++i)
		{
			glGetActiveUniform(object_.name_, (GLuint)i, MAX_NAME_LENGTH, nullptr, &elementCount, &type, nameBuffer);
			int location = glGetUniformLocation(object_.name_, nameBuffer);

			String name(nameBuffer);
			unsigned index = name.Find('[');
			if(index != String::NPOS)
			{
				if(name.Find("[0]", index) == String::NPOS)
					continue;
				// First index
				name = name.SubString(0, index);
			}

			// constant
			if(name[0] == 'c')
			{
				String paramName = name.SubString(1);
				ShaderParameter parameter{paramName, type, location};
				//todo, ??
				bool store = location >= 0;
#ifndef GL_ES_VERSION_2_0
				// If running in OpenGL3, the uniform may be inside a constant buffer
				// todo, what ??
				if(parameter.location_ < 0 && Graphics::GetGL3Support())
				{
					int blockIndex, blockOffset;
					glGetActiveUniformsiv(object_.name_, 1, (const GLuint*)&i, GL_UNIFORM_BLOCK_INDEX, &blockIndex);
					glGetActiveUniformsiv(object_.name_, 1, (const GLuint*)&i, GL_UNIFORM_OFFSET, &blockOffset);

					if(blockIndex >=0)
					{
						parameter.offset_ = blockOffset;
						parameter.bufferPtr_ = constantBuffers_[blockToBinding[blockIndex]];
						store = true;
					}
				}
#endif

				if(store)
					shaderParameters_[StringHash(paramName)] = parameter;
			}
			// Sampler
			// ref https://learnopengl.com/Getting-started/Textures  Texture Units part
			else if(location >=0 && name[0] == 's')
			{
				unsigned unit = graphics_->GetTextureUnit(name.SubString(1));
				if(unit >= MAX_TEXTURE_UNITS)
					unit = NumberPostfix(name);

				if(unit < MAX_TEXTURE_UNITS)
				{
					useTextureUnits_[unit] = true;
					glUniform1iv(location, 1, reinterpret_cast<int*>(&unit));
				}
			}
		}

		vertexAttributes_.Rehash(NextPowerOfTwo(vertexAttributes_.Size()));
		shaderParameters_.Rehash(NextPowerOfTwo(shaderParameters_.Size()));
	}

	ShaderVariation *ShaderProgram::GetVertexShader() const
	{
		return vertexShader_;
	}

	ShaderVariation *ShaderProgram::GetPixelShader() const
	{
		return pixelShader_;
	}

	bool ShaderProgram::HasParameter(StringHash param) const
	{
		return shaderParameters_.Find(param) != shaderParameters_.End();
	}

	bool ShaderProgram::NeedParameterUpdate(ShaderParameterGroup group, const void *source)
	{
		//todo
	}
}

