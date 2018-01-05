//
// Created by liuhao1 on 2018/1/3.
//

#include "Technique.h"
#include "../Core/Context.h"
#include "../Resource/XMLFile.h"

namespace Urho3D
{
	extern const char* cullModeNames[];
	const char* blendModeNames[] =
			{
				"replace",
				"add",
				"multipy",
				"alpha",
				"addalpha",
				"premulalpha",
				"invdestalpha",
				"substract",
				"substractalpha",
				nullptr
			};
	static const char* compareModeNames[] =
			{
				"always",
				"equal",
				"notequal",
				"less",
				"lessequal",
				"greater",
				"greaterequal",
				nullptr
			};

	static const char* lightingModeNames[] =
			{
				"unlit",
				"pervertex",
				"perpixel",
				nullptr
			};

	Pass::Pass(const String &passName) :
		blendMode_(BLEND_REPLACE),
		cullMode_(MAX_CULLMODES),
		depthTestMode_(CMP_LESSEQUAL),
		lightingMode_(LIGHTING_UNLIT),
		shadersLoadedFrameNumber_(0),
		depthWrite_(true),
		isDesktop_(false)
	{
		name_ = passName.ToLower();
		index_ = Technique::GetPassIndex(name_);

		// Guess default lighting mode from pass name
		if(index_ == Technique::basePassIndex || index_ == Technique::alphaPassIndex || index_ == Technique::materialPassIndex ||
		   index_ == Technique::deferredPassIndex)
		{
			lightingMode_ = LIGHTING_PERVERTEX;
		}
		else if(index_ == Technique::lightPassIndex || index_ == Technique::litBasePassIndex ||
				index_ == Technique::litAlphaPassIndex)
		{
			lightingMode_ = LIGHTING_PERPIXEL;
		}
	}

	Pass::~Pass()
	{

	}

	void Pass::SetBlendMode(BlendMode mode)
	{
		blendMode_ = mode;
	}

	void Pass::SetCullMode(CullMode mode)
	{
		cullMode_ = mode;
	}

	void Pass::SetDepthTestMode(CompareMode mode)
	{
		depthTestMode_ = mode;
	}

	void Pass::SetDepthWrite(bool enable)
	{
		depthWrite_ = enable;
	}

	void Pass::SetIsDesktop(bool enable)
	{
		isDesktop_ = enable;
	}

	void Pass::SetVertexShader(const String &name)
	{
		vertexShaderName_ = name;
		ReleaseShaders();
	}

	void Pass::SetPixelShader(const String &name)
	{
		pixelShaderName_ = name;
		ReleaseShaders();
	}

	void Pass::SetVertexShaderDefines(const String &defines)
	{
		vertexShaderDefines_ = defines;
		ReleaseShaders();
	}

	void Pass::SetPixelShaderDefines(const String &defines)
	{
		pixelShaderDefines_ = defines;
		ReleaseShaders();
	}

	void Pass::SetVertexShaderDefineExcludes(const String &excludes)
	{
		vertexShaderDefineExcludes_ = excludes;
		ReleaseShaders();
	}

	void Pass::SetPixelShaderDefineExcludes(const String &excludes)
	{
		pixelShaderDefineExcludes_ = excludes;
		ReleaseShaders();
	}

	void Pass::ReleaseShaders()
	{
		vertexShaders_.Clear();
		pixelShaders_.Clear();
		extraVertexShaders_.Clear();
		extraPixelShaders_.Clear();
	}

	void Pass::MarkShadersLoaded(unsigned frameNumber)
	{
		shadersLoadedFrameNumber_ = frameNumber;
	}

	Vector<SharedPtr<ShaderVariation>> &Pass::GetVertexShaders(const StringHash &extraDefinesHash)
	{
		if(!extraDefinesHash.Value())
			return vertexShaders_;
		else
			return extraVertexShaders_[extraDefinesHash];
	}

	Vector<SharedPtr<ShaderVariation>> &Pass::GetPixelShaders(const StringHash &extraDefinesHash)
	{
		if(!extraDefinesHash.Value())
			return pixelShaders_;
		else
			return extraPixelShaders_[extraDefinesHash];
	}

	String Pass::GetEffectiveVertexShaderDefines() const
	{
		if(vertexShaderDefineExcludes_.Empty())
			return vertexShaderDefines_;
		Vector<String> vsDefines = vertexShaderDefines_.Split(' ');
		Vector<String> vsExcludes = vertexShaderDefineExcludes_.Split(' ');
		for(unsigned i = 0; i<vsExcludes.Size(); ++i)
			vsDefines.Remove(vsExcludes[i]);

		return String::Joined(vsDefines, " ");
	}

	String Pass::GetEffectivePixelShaderDefines() const
	{
		if(pixelShaderDefineExcludes_.Empty())
			return pixelShaderDefines_;

		Vector<String> psDefines = pixelShaderDefines_.Split(' ');
		Vector<String> psExcludes = pixelShaderDefineExcludes_.Split(' ');
		for(unsigned i=0; i<psExcludes.Size(); ++i)
		{
			psDefines.Remove(psExcludes[i]);
		}

		return String::Joined(psDefines, " ");
	}

	void Pass::SetLightingMode(PassLightingMode mode)
	{
		lightingMode_ = mode;
	}

	unsigned Technique::basePassIndex = 0;
	unsigned Technique::alphaPassIndex = 0;
	unsigned Technique::materialPassIndex = 0;
	unsigned Technique::deferredPassIndex = 0;
	unsigned Technique::lightPassIndex = 0;
	unsigned Technique::litBasePassIndex = 0;
	unsigned Technique::litAlphaPassIndex = 0;
	unsigned Technique::shadowPassIndex = 0;

	HashMap<String, unsigned > Technique::passIndices;

	Technique::Technique(Context *context) :
		Resource(context),
		isDesktop_(false)
	{
#ifdef DESKTOP_GRAPHICS
		desktopSupport_ = true;
#else
		desktopSupport_ = false;
#endif
	}

	Technique::~Technique()
	{

	}

	void Technique::RegisterObject(Context *context)
	{
		context->RegisterFactory<Technique>();
	}

	bool Technique::BeginLoad(Deserializer &source)
	{
		passes_.Clear();
		cloneTechniques_.Clear();

		SetMemoryUse(sizeof(Technique));

		SharedPtr<XMLFile> xml(new XMLFile(context_));
		if(!xml->Load(source))
			return false;

		XMLElement rootElem = xml->GetRoot();
		if(rootElem.HasAttribute("desktop"))
			isDesktop_ = rootElem.GetBool("desktop");

		String globalVS = rootElem.GetAttribute("vs");
		String globalPS = rootElem.GetAttribute("ps");
		String globalVSDefines = rootElem.GetAttribute("vsdefines");
		String globalPSDefines = rootElem.GetAttribute("psdefines");
		// End with space so that the pass-specific defines can be sppended
		if(!globalVSDefines.Empty())
			globalVSDefines += ' ';
		if(!globalPSDefines.Empty())
			globalPSDefines += ' ';

		XMLElement passElem = rootElem.GetChild("pass");
		while (passElem)
		{
			if(passElem.HasAttribute("name"))
			{
				Pass* newPass = CreatePass(passElem.GetAttribute("name"));

				if(passElem.HasAttribute("desktop"))
				{
					newPass->SetIsDesktop(passElem.GetBool("desktop"));
				}

				// Note append global defines only when pass does NOTE REDEFINE the sahder
				if(passElem.HasAttribute("vs"))
				{
					newPass->SetVertexShader(passElem.GetAttribute("vs"));
					newPass->SetVertexShaderDefines(passElem.GetAttribute("vsdefines"));
				}
				else
				{
					newPass->SetVertexShader(globalVS);
					newPass->SetVertexShaderDefines(globalVSDefines + passElem.GetAttribute("vsdefines"));
				}

				if(passElem.HasAttribute("ps"))
				{
					newPass->SetPixelShader(passElem.GetAttribute("ps"));
					newPass->SetPixelShaderDefines(passElem.GetAttribute("psdefines"));
				}
				else
				{
					newPass->SetPixelShader(globalPS);
					newPass->SetPixelShaderDefines(globalPSDefines + passElem.GetAttribute("psdefines"));
				}

				newPass->SetVertexShaderDefineExcludes(passElem.GetAttribute("vsexcludes"));
				newPass->SetPixelShaderDefineExcludes(passElem.GetAttribute("psexcludes"));

				if(passElem.HasAttribute("lighting"))
				{
					String lighting = passElem.GetAttributeLower("lighting");
					newPass->SetLightingMode((PassLightingMode)GetStringListIndex(lighting.CString(), lightingModeNames, LIGHTING_UNLIT));
				}

				if(passElem.HasAttribute("blend"))
				{
					String blend = passElem.GetAttributeLower("blend");
					newPass->SetBlendMode((BlendMode)GetStringListIndex(blend.CString(), blendModeNames, BLEND_REPLACE));
				}

				if(passElem.HasAttribute("cull"))
				{
					String cull = passElem.GetAttributeLower("cull");
					newPass->SetCullMode((CullMode)GetStringListIndex(cull.CString(), cullModeNames, MAX_CULLMODES));
				}

				if(passElem.HasAttribute("depthtest"))
				{
					String depthTest = passElem.GetAttributeLower("depthtest");
					if(depthTest == "false")
						newPass->SetDepthTestMode(CMP_ALWAYS);
					else
						newPass->SetDepthTestMode((CompareMode)GetStringListIndex(depthTest.CString(), compareModeNames, CMP_LESS));
				}

				if(passElem.HasAttribute("depthwrite"))
				{
					newPass->SetDepthWrite(passElem.GetBool("depthwrite"));
				}

				if(passElem.HasAttribute("alphatocoverage"))
				{
					//todo
				}
			}
			else
				URHO3D_LOGERROR("Missing pass name");
		}

		passElem = passElem.GetNext("pass");
	}

	void Technique::SetIsDesktop(bool enable)
	{
		isDesktop_ = enable;
	}

	Pass *Technique::CreatePass(const String &passName)
	{
		Pass* oldPass = GetPass(passName);
		if(oldPass)
			return oldPass;

		SharedPtr<Pass> newPass(new Pass(passName));
		unsigned passIndex = newPass->GetIndex();
		if(passIndex >= passes_.Size())
			passes_.Resize(passIndex + 1);
		passes_[passIndex] = newPass;

		SetMemoryUse((unsigned)(sizeof(Technique) + GetNumPasses() * sizeof(Pass)));
		return newPass;
	}

	void Technique::RemovePass(const String &passName)
	{
		auto iter = passIndices.Find(passName.ToLower());
		if(iter == passIndices.End())
			return;
		else if(iter->second_ < passes_.Size() && passes_[iter->second_].Get())
		{
			passes_[iter->second_].Reset();
			SetMemoryUse((unsigned)(sizeof(Technique) + GetNumPasses() * sizeof(Pass)));
		}

	}

	void Technique::ReleaseShaders()
	{
		for(auto iter = passes_.Begin(); iter != passes_.End(); ++iter)
		{
			Pass* pass = iter->Get();
			if(pass)
				pass->ReleaseShaders();
		}
	}

	SharedPtr<Technique> Technique::Clone(const String &cloneName) const
	{
		SharedPtr<Technique> ret(new Technique(context_));
		ret->SetIsDesktop(isDesktop_);
		ret->SetName(cloneName);

		// Deep copy passes
		for(auto iter = passes_.Begin(); iter != passes_.End(); ++iter)
		{
			Pass* srcPass = iter->Get();
			if(!srcPass)
				continue;

			Pass* newPasss = ret->CreatePass(srcPass->GetName());
			newPasss->SetBlendMode(srcPass->GetBlendMode());
			newPasss->SetDepthTestMode((srcPass->GetDepthTestMode()));
			newPasss->SetLightingMode(srcPass->GetLightingMode());
			newPasss->SetDepthWrite(srcPass->GetDepthWrite());
			//todo
			newPasss->SetIsDesktop(srcPass->IsDesktop());
			newPasss->SetVertexShader(srcPass->GetVertexShader());
			newPasss->SetPixelShader(srcPass->GetPixelShader());
			newPasss->SetVertexShaderDefines(srcPass->GetVertexShaderDefines());
			newPasss->SetPixelShaderDefines(srcPass->GetPixelShaderDefines());
			newPasss->SetVertexShaderDefineExcludes(srcPass->GetVertexShaderDefineExcludes());
			newPasss->SetPixelShaderDefineExcludes(srcPass->GetPixelShaderDefineExclude());
		}
		return ret;
	}

	bool Technique::HasPass(const String &passName) const
	{
		auto i = passIndices.Find(passName.ToLower());
		return i != passIndices.End() ? HasPass(i->second_) : false;
	}

	Pass *Technique::GetPass(const String &passName) const
	{
		auto iter = passIndices.Find(passName.ToLower());
		return iter != passIndices.End() ? GetPass(iter->second_) : nullptr;
	}

	Pass *Technique::GetSupportedPass(const String &passName) const
	{
		auto iter = passIndices.Find(passName.ToLower());
		return iter != passIndices.End() ? GetSupportedPass(iter->second_) : nullptr;
	}

	unsigned Technique::GetNumPasses() const
	{
		unsigned ret = 0;
		for(auto i = passes_.Begin(); i != passes_.End(); ++i)
		{
			if(i->Get())
				++ret;
		}
		return ret;
	}

	Vector<String> Technique::GetPassNames() const
	{
		Vector<String> ret;
		for(auto iter = passes_.Begin();  iter != passes_.End(); ++iter)
		{
			Pass* pass = iter->Get();
			if(pass)
				ret.Push(pass->GetName());
		}
		return ret;
	}

	PODVector<Pass *> Technique::GetPasses() const
	{
		PODVector<Pass*> ret;
		for(auto iter = passes_.Begin();  iter != passes_.End(); ++iter)
		{
			Pass* pass = iter->Get();
			if(pass)
				ret.Push(pass);
		}
		return ret;
	}

	SharedPtr<Technique> Technique::CloneWithDefines(const String &vsDefines, const String &psDefines)
	{
		if(vsDefines.Empty() && psDefines.Empty())
			return SharedPtr<Technique>(this);

		Pair<StringHash, StringHash> key = MakePair(StringHash(vsDefines), StringHash(psDefines));

		auto iter = cloneTechniques_.Find(key);
		if(iter != cloneTechniques_.End())
			return iter->second_;

		iter = cloneTechniques_.Insert(MakePair(key, Clone(GetName())));

		for(auto passIter = iter->second_->passes_.Begin(); passIter != iter->second_->passes_.End(); ++passIter)
		{
			Pass* pass = *passIter;
			if(!pass)
				continue;

			//Todo, usage
			if(!vsDefines.Empty())
				pass->SetVertexShaderDefines(pass->GetVertexShaderDefines() + " " + vsDefines);
			if(!psDefines.Empty())
				pass->SetPixelShaderDefines(pass->GetPixelShaderDefines() + " " + psDefines);
		}
		return iter->second_;
	}

	unsigned Technique::GetPassIndex(const String &passName)
	{
		if(passIndices.Empty())
		{
			basePassIndex = passIndices["base"] = 0;
			alphaPassIndex = passIndices["alpha"] = 1;
			materialPassIndex = passIndices["material"] = 2;
			deferredPassIndex = passIndices["deferred"] = 3;
			lightPassIndex = passIndices["light"] = 4;
			litBasePassIndex = passIndices["litbase"] = 5;
			litAlphaPassIndex = passIndices["litalpha"] = 6;
			shadowPassIndex = passIndices["shadow"] = 7;
		}

		String nameLow = passName.ToLower();
		auto iter = passIndices.Find(nameLow);
		if(iter != passIndices.End())
			return iter->second_;
		else
		{
			unsigned newPassIndex = passIndices.Size();
			passIndices[nameLow] = newPassIndex;
			return newPassIndex;
		}
	}
}