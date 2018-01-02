//
// Created by liuhao1 on 2017/12/24.
//

#ifndef URHO3DCOPY_GRAPHICSDEFS_H
#define URHO3DCOPY_GRAPHICSDEFS_H

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

	enum CullMode
	{
		CULL_NONE = 0,
		CULL_CCW,
		CULL_CW,
		MAX_CULLMODES
	};

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
		TU_LIGHTMAP = 8,
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
		bool perInstance_;
		//Todo, the offset at VertexBuffer ??
		unsigned offset_;
	};

	extern const unsigned ELEMENT_TYPESIZES[];
	extern const VertexElement LEGACY_VERTEXELEMENTS[];

	//todo

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

	static const int MAX_VERTEX_STREAMS = 4;

}

#endif //URHO3DCOPY_GRAPHICSDEFS_H
