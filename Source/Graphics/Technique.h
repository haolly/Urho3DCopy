//
// Created by liuhao1 on 2018/1/3.
//

#ifndef URHO3DCOPY_TECHNIQUE_H
#define URHO3DCOPY_TECHNIQUE_H

#include "../Container/RefCounted.h"
#include "../Container/Str.h"
#include "GraphicsDefs.h"
#include "../Container/Ptr.h"
#include "../Math/StringHash.h"
#include "../Container/HashMap.h"
#include "../Resource/Resource.h"

namespace Urho3D
{
	class ShaderVariation;

	enum PassLightingMode
	{
		LIGHTING_UNLIT = 0,
		LIGHTING_PERVERTEX,
		LIGHTING_PERPIXEL
	};

	class Pass : public RefCounted
	{
	public:
		Pass(const String& passName);

		virtual ~Pass() override ;

		void SetBlendMode(BlendMode mode);
		void SetCullMode(CullMode mode);
		void SetDepthTestMode(CompareMode mode);
		void SetLightingMode(PassLightingMode mode);
		void SetDepthWrite(bool enable);
		//todo
		void SetIsDesktop(bool enable);
		void SetVertexShader(const String& name);
		void SetPixelShader(const String& name);
		void SetVertexShaderDefines(const String& defines);
		void SetPixelShaderDefines(const String& defines);
		void SetVertexShaderDefineExcludes(const String& excludes);
		void SetPixelShaderDefineExcludes(const String& excludes);
		void ReleaseShaders();
		void MarkShadersLoaded(unsigned frameNumber);

		const String& GetName() const { return name_; }
		unsigned GetIndex() const { return index_; }
		BlendMode GetBlendMode() const { return blendMode_; }
		CullMode GetCullMode() const { return cullMode_; }
		CompareMode GetDepthTestMode() const  { return depthTestMode_; }
		PassLightingMode GetLightingMode() const { return lightingMode_; }

		unsigned GetShadersLoadedFrameNumber() const { return shadersLoadedFrameNumber_; }
		bool GetDepthWrite() const { return depthWrite_; }

		bool IsDesktop() const { return isDesktop_; }
		const String& GetVertexShader() const { return vertexShaderName_; }
		const String& GetPixelShader() const { return pixelShaderName_; }
		const String& GetVertexShaderDefines() const { return vertexShaderDefines_; }
		const String& GetPixelShaderDefines() const { return pixelShaderDefines_; }

		const String& GetVertexShaderDefineExcludes() const { return vertexShaderDefineExcludes_; }
		const String& GetPixelShaderDefineExclude() const { return pixelShaderDefineExcludes_; }

		Vector<SharedPtr<ShaderVariation> >& GetVertexShaders() { return vertexShaders_; }
		Vector<SharedPtr<ShaderVariation> >& GetPixelShaders() const { return pixelShaders_; }

		Vector<SharedPtr<ShaderVariation>>& GetVertexShaders(const StringHash& extraDefinesHash);
		Vector<SharedPtr<ShaderVariation>>& GetPixelShaders(const StringHash& extraDefinesHash);

		String GetEffectiveVertexShaderDefines() const;
		String GetEffectivePixelShaderDefines() const;

	private:
		//Pass index
		unsigned index_;
		BlendMode blendMode_;
		CullMode cullMode_;
		CompareMode depthTestMode_;
		PassLightingMode lightingMode_;
		unsigned shadersLoadedFrameNumber_;
		bool depthWrite_;
		//todo, what is this ?
		bool alphaToCoverage_;
		bool isDesktop_;
		String vertexShaderName_;
		String pixelShaderName_;
		String vertexShaderDefines_;
		String pixelShaderDefines_;
		String vertexShaderDefineExcludes_;
		String pixelShaderDefineExcludes_;
		Vector<SharedPtr<ShaderVariation> > vertexShaders_;
		Vector<SharedPtr<ShaderVariation> > pixelShaders_;

		//todo, why need this ??
		HashMap<StringHash, Vector<SharedPtr<ShaderVariation> > > extraVertexShaders_;
		HashMap<StringHash, Vector<SharedPtr<ShaderVariation> > > extraPixelShaders_;

		String name_;
	};

	//Consists of several passes
	//todo, what is the purpose of this, why does not use shader class directly?
	class Technique : public Resource
	{
		URHO3D_OBJECT(Technique, Resource);
	public:
		Technique(Context* context);
		virtual ~Technique() override ;
		static void RegisterObject(Context* context);

		virtual bool BeginLoad(Deserializer& source) override ;

		void SetIsDesktop(bool enable);
		Pass* CreatePass(const String& passName);
		void RemovePass(const String& passName);
		void ReleaseShaders();

		SharedPtr<Technique> Clone(const String& cloneName = String::EMPTY) const;

		bool IsDesktop() const
		{
			return isDesktop_;
		}

		bool IsSupoorted() const
		{
			return !isDesktop_ || desktopSupport_;
		}

		bool HasPass(unsigned passIndex) const
		{
			return passIndex < passes_.Size() && passes_[passIndex].Get() != nullptr;
		}

		bool HasPass(const String& passName) const;

		Pass* GetPass(unsigned passIndex) const
		{
			return passIndex < passes_.Size() ? passes_[passIndex] : nullptr;
		}

		Pass* GetPass(const String& passName) const;

		Pass* GetSupportedPass(unsigned passIndex) const
		{
			Pass* pass = passIndex < passes_.Size() ? passes_[passIndex] : nullptr;
			return pass && (!pass->IsDesktop() || desktopSupport_) ? pass : nullptr;
		}

		Pass* GetSupportedPass(const String& passName) const;

		unsigned GetNumPasses() const;
		Vector<String> GetPassNames() const;
		PODVector<Pass*> GetPasses() const;

		SharedPtr<Technique> CloneWithDefines(const String& vsDefines, const String& psDefines);

		static unsigned GetPassIndex(const String& passName);

		static unsigned basePassIndex;
		static unsigned alphaPassIndex;
		static unsigned materialPassIndex;
		static unsigned deferredPassIndex;
		static unsigned lightPassIndex;
		static unsigned litBasePassIndex;
		static unsigned litAlphaPassIndex;
		static unsigned shadowPassIndex;


	private:
		bool isDesktop_;
		bool desktopSupport_;
		Vector<SharedPtr<Pass> > passes_;
		//cache
		HashMap<Pair<StringHash, StringHash>, SharedPtr<Technique>> cloneTechniques_;

		static HashMap<String, unsigned > passIndices;
	};

}



#endif //URHO3DCOPY_TECHNIQUE_H
