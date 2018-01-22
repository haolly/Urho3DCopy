//
// Created by liuhao1 on 2017/12/24.
//

#ifndef URHO3DCOPY_GRAPHICSDEFS_H
#define URHO3DCOPY_GRAPHICSDEFS_H

#include "../Math/StringHash.h"

namespace Urho3D
{
	class Vector3;

	enum PrimitiveType
	{
		TRIANGLE_LIST = 0,
		LINE_LIST,
		POINT_LIST,
		TRIANGLE_STRIP,
		LINE_STRIP,
		TRIANGLE_FAN
	};

	enum BlendMode
	{
		BLEND_REPLACE = 0,
		BLEND_ADD,
		BLEND_MULTIPLY,
		BLEND_ALPHA,
		BLEND_ADDALPHS,
		BLEND_PREMULALPHA,
		BLEND_INVDESTALPHA,
		BLEND_SUBTRACT,
		BLEND_SUBSTRACTALPHA,
		MAX_BLENDMODES
	};

	// Depth or stencil compare mode
	enum CompareMode
	{
		CMP_ALWAYS = 0,
		CMP_EQUAL,
		CMP_NOTEQUAL,
		CMP_LESS,
		CMP_LESSEQUAL,
		CMP_GREATER_,
		CMP_GREATEREQUAL,
		CMP_COMPAREMODES
	};

	enum CullMode
	{
		CULL_NONE = 0,
		CULL_CCW,
		CULL_CW,
		MAX_CULLMODES
	};

	enum FillMode
	{
		FILL_SOLID = 0,
		FILL_WIREFRAME,
		FILL_POINT
	};

	//todo
	enum StencilOp
	{
		OP_KEEP = 0,
		OP_ZERO,
		OP_REF,
		OP_INCR,
		OP_DECR
	};

	enum RenderSurfaceUpdateMode
	{
		SURFACE_MANUALUPDATE = 0,
		SURFACE_UPDATEVISIBLE,
		SURFACE_UPDATEALWAYS
	};

	enum ShaderType
	{
		VS = 0,
		PS,
	};

	// todo, what is this ?
	enum ShaderParameterGroup
	{
		SP_FRAME = 0,
		//todo
		MAX_SHADER_PARAMETER_GROUPS
	};

	// todo, 每种贴图的不同用途？？
	enum TextureUnit
	{
		TU_DIFFUSE = 0,
		TU_ALBEDOBUFFER = 0,
		TU_NORMAL = 1,
		TU_NORMALBUFFER = 1,
		TU_SPECULAR = 2,
		TU_EMISSIVE = 3,
		TU_ENVIRONMENT = 4,
		TU_VOLUMEMAP = 5,
		TU_CUSTOM1 = 6,
		TU_CUSTOM2 = 7,
		TU_LIGHTRAMP = 8,
		TU_LIGHTSHAPE = 9,
		TU_SHADOWMAP = 10,
		TU_FACESELECT = 11,
		TU_INDIRECTION = 12,
		TU_DEPTHBUFFER = 13,
		TU_LIGHTBUFFER = 14,
		TU_ZONE = 15,
		MAX_MATERIAL_TEXTURE_UNITS = 8,
		MAX_TEXTURE_UNITS = 16
	};

	enum LockState
	{
		LOCK_NONE = 0,
		LOCK_HARDWARE,
		LOCK_SHADOW,
		LOCK_SCRATCH
	};

	enum LegacyVertexElement
	{
		ELEMENT_POSITION = 0,
		ELEMENT_NORMAL,
		ELEMENT_COLOR,
		ELEMENT_TEXCOORD1,
		ELEMENT_TEXCOORD2,
		ELEMENT_CUBETEXCOORD1,
		ELEMENT_CUBETEXCOORD2,
		ELEMENT_TANGENT,
		ELEMENT_BLENDWEIGHTS,
		ELEMENT_BLENDINDICES,
		ELEMENT_INSTANCEMATRIX1,
		ELEMENT_INSTANCEMATRIX2,
		ELEMENT_INSTANCEMATRIX3,
		// Custom 32-bit interger object index. Due to API lmitations, not supported on D3D9
		ELEMENT_OBJECTINDEX,
		MAX_LEGACY_VERTEX_ELEMENTS
	};

	enum VertexElementType
	{
		TYPE_INT = 0,
		TYPE_FLOAT,
		TYPE_VECTOR2,
		TYPE_VECTOR3,
		TYPE_VECTOR4,
		TYPE_UBYTE4,
		TYPE_UBYTE4_NORM,
		MAX_VERTEX_ELEMENT_TYPES
	};

	enum VertexElementSemantic
	{
		SEM_POSITION = 0,
		SEM_NORMAL,
		SEM_BINORMAL,
		SEM_TANGENT,
		SEM_TEXCOORD,
		SEM_COLOR,
		SEM_BLENDWEIGHTS,
		SEM_BLENDINDICES,
		SEM_OBJECTINDEX,
		MAX_VERTEX_ELEMENT_SEMANTICS
	};

	struct VertexElement
	{
		VertexElement() :
				type_(TYPE_VECTOR3),
				semantic_(SEM_POSITION),
				index_(0),
				perInstance_(false),
				offset_(0)
		{
		}

		VertexElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0, bool perInstance = false) :
				type_(type),
				semantic_(semantic),
				index_(index),
				perInstance_(perInstance),
				offset_(0)
		{
		}

		bool operator ==(const VertexElement& rhs) const
		{
			return type_ == rhs.type_ && semantic_ == rhs.semantic_ && index_ == rhs.index_ && perInstance_ == rhs.perInstance_;
		}

		bool operator !=(const VertexElement& rhs) const
		{
			return !(*this == rhs);
		}

		VertexElementType type_;
		VertexElementSemantic semantic_;
		unsigned char index_;
		//todo, what is this?
		bool perInstance_;
		//Todo, the offset at VertexBuffer ??
		unsigned offset_;
	};

	extern const unsigned ELEMENT_TYPESIZES[];
	extern const VertexElement LEGACY_VERTEXELEMENTS[];

	enum TextureFilterMode
	{
		FILTER_NEAREST = 0,
		FILTER_BILINEAR,
		FILTER_TRILINEAR,
		FILTER_ANISOTROPIC,
		FILTER_NEAREST_ANISOTROPIC,
		FILTER_DEFAULT,
		MAX_FILTERMODES
	};

	//Todo, what is this?
	enum TextureAddressMode
	{
		ADDRESS_WRAP = 0,
		ADDRESS_MIRROR,
		ADDRESS_CLAMP,
		ADDRESS_BORDER,
		MAX_ADDRESSMODES
	};

	enum TextureCoordinate
	{
		COORD_U = 0,
		COORD_V,
		COORD_W,
		MAX_COORDS
	};

	//todo, usage
	enum TextureUsage
	{
		TEXTURE_STATIC = 0,
		TEXTURE_DYNAMIC,
		TEXTURE_RENDERTARGET,
		TEXTURE_DEPTHSTENCIL
	};

	//todo

	static const int QUALITY_LOW = 0;
	static const int QUALITY_MEDIUM = 1;
	static const int QUALITY_HIGH = 2;
	static const int QUALITY_MAX_ = 15;

	//Legacy vertex element bitmasks
	static const unsigned MASK_NONE = 0x0;
	static const unsigned MASK_POSITION = 0x1;
	static const unsigned MASK_NORMAL = 0x2;
	static const unsigned MASK_COLOR = 0x4;
	static const unsigned MASK_TEXCOORD1 = 0x8;
	static const unsigned MASK_TEXCOORD2 = 0x10;
	static const unsigned MASK_TANGENT = 0x20;
	static const unsigned MASK_BLENDWEIGHTS = 0x40;
	//todo

	// todo, why 4 ??
	static const int MAX_RENDERTARGETS = 4;
	// todo, why defines the value to 4?
	static const int MAX_VERTEX_STREAMS = 4;
	static const int MAX_CONSTANT_REGISTERS = 256;


	// Inbuilt shader parameters
	extern const StringHash VSP_AMBIENTSTARTCOLOR;
	//TODO
	extern const StringHash PSP_MATSPECCOLOR;

}

#endif //URHO3DCOPY_GRAPHICSDEFS_H
