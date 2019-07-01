//
// Created by liuhao on 2019-04-14.
//

#ifndef URHO3DCOPY_OGLGRAPHICSIMPL_HPP
#define URHO3DCOPY_OGLGRAPHICSIMPL_HPP

#include "../../Container/HashMap.h"
#include "../../Graphics/ConstantBuffer.h"
#include "../../Graphics/RenderSurface.h"
#include "../../Graphics/ShaderProgram.hpp"
#include "../../Graphics/Texture2D.h"

#include <GLEW/glew.h>

using SDL_GLContext = void*;

namespace Urho3D
{
	class Context;
	using ConstantBufferMap = HashMap<unsigned, SharedPtr<ConstantBuffer>>;
	using ShaderProgramMap = HashMap<Pair<ShaderVariation*, ShaderVariation*>, SharedPtr<ShaderProgram>>;

	struct FrameBufferObject
	{
		unsigned fbo_{};
		RenderSurface* colorAttachments_[MAX_RENDERTARGETS];
		RenderSurface* depthAttachment_{};
		unsigned readBuffers_{M_MAX_UNSIGNED};
		unsigned drawBuffers_{M_MAX_UNSIGNED};
	};

	class GraphicsImpl
	{
		friend class Graphics;

	public:
		GraphicsImpl() = default;

		const SDL_GLContext& GetGLContext()
		{
			return context_;
		}

	private:
		SDL_GLContext context_{};
		unsigned systemFBO_{};
		unsigned activeTexture_{};
		unsigned enabledVertexAttributes_{};
		unsigned usedVertexAttributes_{};
		unsigned instancingVertexAttributes_{};

		const HashMap<Pair<unsigned char, unsigned char>, unsigned >* vertexAttributes_{};
		unsigned boundFBO_{};
		unsigned boundVBO_{};
		unsigned boundUBO_{};

		unsigned resolveSrcFBO_{};
		unsigned resolveDestFBO_{};

		int pixelFormat{};
		HashMap<unsigned long long, FrameBufferObject> frameBuffers_;

		unsigned textureTypes_[MAX_TEXTURE_UNITS];
		ConstantBufferMap allConstantBuffers_;
		// Currently bound const buffers
		ConstantBuffer* constantBuffers_[MAX_SHADER_PARAMETER_GROUPS];
		PODVector<ConstantBuffer*> dirtyConstantBuffers_;
		unsigned lastInstanceOffset_{};
		HashMap<unsigned, SharedPtr<Texture2D>> depthTextures_;
		ShaderProgram* shaderProgram_{};
		ShaderProgramMap shaderPrograms_;
		bool fboDirty_{};
		bool vertexBuffersDirty_{};
		bool sRGBWrite_{};
	};
}

#endif //URHO3DCOPY_OGLGRAPHICSIMPL_HPP
