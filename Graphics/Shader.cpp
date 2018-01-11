//
// Created by liuhao1 on 2018/1/10.
//

#include "Shader.h"
#include "../Resource/ResourceCache.h"
#include "Graphics.h"
#include "ShaderVariation.h"
#include "../Container/Sort.h"

namespace Urho3D
{
	void CommentOutFunction(String &code, const String &signature)
	{
		unsigned startPos = code.Find(signature);
		unsigned braceLevel = 0;
		if(startPos == String::NPOS)
			return;

		code.Insert(startPos, "/*");
		for(unsigned i= startPos + 2 + signature.Length(); i< code.Length(); ++i)
		{
			if(code[i] == '{')
				++braceLevel;
			else if(code[i] == '}')
			{
				--braceLevel;
				if(braceLevel == 0)
				{
					code.Insert(i+1, "*/");
					return;
				}
			}
		}
	}

	Shader::Shader(Context *context) :
		Resource(context),
		timeStamp_(0),
		numVariations_(0)
	{
	}

	Shader::~Shader()
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();
		if(cache)
			cache->ResetDependencies(this);
	}

	void Shader::RegisterObject(Context *context)
	{
		context->RegisterFactory<Shader>();
	}

	bool Shader::BeginLoad(Deserializer &source)
	{
		Graphics* graphics = GetSubsystem<Graphics>();
		if(!graphics)
			return false;

		// Load the shader source code and resolve any includes
		timeStamp_ = 0;
		String shaderCode;
		if(!ProcessSource(shaderCode, source))
			return false;

		// Comment out the unneeded shader function
		vsSourceCode_ = shaderCode;
		psSourceCode_ = shaderCode;
		CommentOutFunction(vsSourceCode_, "void PS(");
		CommentOutFunction(psSourceCode_, "void PS(");

		// OpenGL: rename either VS() or PS() to main()
#ifdef URHO3D_OPENGL
		vsSourceCode_.Replace("void VS(", "void main(");
		psSourceCode_.Replace("void PS(", "void main(");
#endif
		RefreshMemoryUse();
		return true;
	}

	bool Shader::EndLoad()
	{
		// If variations had already been crated, release them and require recompile
		for(auto iter = vsVariations_.Begin(); iter != vsVariations_.End(); ++iter)
			iter->second_->Release();
		for(auto iter = psVariations_.Begin(); iter != psVariations_.End(); ++iter)
			iter->second_->Release();
		return true;
	}

	ShaderVariation *Shader::GetVariation(ShaderType type, const String &defines)
	{
		return GetVariation(type, defines.CString());
	}

	ShaderVariation *Shader::GetVariation(ShaderType type, const char *defines)
	{
		StringHash definesHash(defines);
		auto& variations(type == VS ? vsVariations_ : psVariations_);
		auto iter = variations.Find(definesHash);
		if(iter == variations.End())
		{
			// If shader not found, normalize the defines (to prevent duplicates) and check again. In that case make an alias(nameHash)
			// so that further queries are faster
			String normalizedDefines = NormalizeDefines(defines);
			StringHash normalizedHash(normalizedDefines);

			iter = variations.Find(normalizedHash);
			if(iter != variations.End())
				variations.Insert(MakePair(definesHash, iter->second_));
			else
			{
				// No shader variation found. Crate new
				iter = variations.Insert(MakePair(normalizedHash, SharedPtr<ShaderVariation>(new ShaderVariation(this, type))));
				if (definesHash != normalizedHash)
					variations.Insert(MakePair(definesHash, iter->second_));

				iter->second_->SetName(GetFileName(GetName()));
				iter->second_->SetDefines(normalizedDefines);
				++numVariations_;
				RefreshMemoryUse();
			}
		}

		return iter->second_;
	}

	bool Shader::ProcessSource(String &code, Deserializer &source)
	{
		ResourceCache* cache = GetSubsystem<ResourceCache>();

		File* file = dynamic_cast<File*>(&source);
		if(file && !file->IsPackaged())
		{
			FileSystem* fileSystem = GetSubsystem<FileSystem>();
			String fullName = cache->GetResourceFileName(file->GetName());
			unsigned fileTimeStamp = fileSystem->GetLastModifiedTime(fullName);
			if(fileTimeStamp > timeStamp_)
				timeStamp_ = fileTimeStamp;
		}

		// Store resource dependencies for includes so that we know to reload if any of them changes
		// todo, when did this happen? when is the same, when is it different?
		if(source.GetName() != GetName())
			cache->StoreResourceDependency(this, source.GetName());

		while (source.IsEof())
		{
			String line = source.ReadLine();
			String inc = "#include";
			if(line.StartWith(inc))
			{
				String includeFileName = GetPath(source.GetName()) + line.SubString(inc.Length() + 1).Replaced("\"", "").Trimmed();

				SharedPtr<File> includeFile = cache->GetFile(includeFileName);
				if(!includeFile)
					return false;

				// Add the include file into the current code recursively
				if(!ProcessSource(code, *includeFile))
					return false;
			}
			else
			{
				code += line;
				code += "\n";
			}
		}

		code += "\n";

		return true;
	}

	String Shader::NormalizeDefines(const String &defines)
	{
		Vector<String> definesVec = defines.ToUpper().Split(' ');
		Sort(definesVec.Begin(), definesVec.End());
		return String::Joined(definesVec, " ");
	}

	void Shader::RefreshMemoryUse()
	{
		SetMemoryUse(
				(unsigned) (sizeof(Shader) + vsSourceCode_.Length() + psSourceCode_.Length() + numVariations_ * sizeof(ShaderVariation))
		);
	}
}