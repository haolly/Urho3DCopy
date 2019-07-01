//
// Created by liuhao1 on 2018/1/10.
//

#ifndef URHO3DCOPY_SHADER_H
#define URHO3DCOPY_SHADER_H

#include "../Resource/Resource.h"
#include "../Core/Context.h"
#include "GraphicsDefs.h"

namespace Urho3D
{
	class ShaderVariation;

	class Shader : public Resource
	{
		URHO3D_OBJECT(Shader, Resource);
	public:
		explicit Shader(Context* context);

		~Shader() override ;

		static void RegisterObject(Context* context);

		bool BeginLoad(Deserializer& source) override ;

		bool EndLoad() override ;

		ShaderVariation* GetVariation(ShaderType type, const String& defines);
		ShaderVariation* GetVariation(ShaderType type, const char* defines);

		const String& GetSourceCode(ShaderType type)
		{
			return type == VS ? vsSourceCode_ : psSourceCode_;
		}

		unsigned GetTimeStamp() const { return timeStamp_;}

	private:
		bool ProcessSource(String& code, Deserializer& file);
		String NormalizeDefines(const String& defines);

		void RefreshMemoryUse();

		String vsSourceCode_;
		String psSourceCode_;

		HashMap<StringHash, SharedPtr<ShaderVariation>> vsVariations_;
		HashMap<StringHash, SharedPtr<ShaderVariation>> psVariations_;

		unsigned timeStamp_;
		unsigned numVariations_;

	};
}


#endif //URHO3DCOPY_SHADER_H
