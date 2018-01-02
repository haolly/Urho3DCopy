//
// Created by liuhao1 on 2017/12/24.
//

#ifndef URHO3DCOPY_VERTEXBUFFER_H
#define URHO3DCOPY_VERTEXBUFFER_H

#include "GPUObject.h"
#include "../Core/Object.h"
#include "../Container/ArrayPtr.h"
#include "GraphicsDefs.h"

namespace Urho3D
{
	class VertexBuffer : public Object, public GPUObject
	{
		URHO3D_OBJECT(VertexBuffer, Object);
	public:
		VertexBuffer(Context* context, bool forceHandless = false);
		virtual ~VertexBuffer() override ;

		virtual void OnDeviceLost() override ;
		virtual void OnDeviceReset() override ;
		virtual void Release() override ;

		void SetShadowed(bool enable);
		bool SetSize(unsigned vertexCount, const PODVector<VertexElement>& element, bool dynamic = false);
		bool SetSize(unsigned vertexCount, unsigned elementMask, bool dynamic = false);
		bool SetData(const void* data);
		bool SetDataRange(const void* data, unsigned start, unsigned count, bool discard = false);
		//Note, lock the buffer for write only editing, Return data pointer if successful,Optionally discard data
		// outside the range
		void* Lock(unsigned start, unsigned count, bool discard = false);

		void Unlock();
		bool IsShadowed() const { return shadowed_; }
		bool IsDynamic() const { return dynamic_; }
		bool IsLocked() const { return lockState_ != LOCK_NONE; }
		unsigned GetVertexCount() const { return vertexCount_; }
		unsigned GetVertexSize() const { return vertexSize_; }

		const PODVector<VertexElement>& GetElements() const { return elements_; }
		const VertexElement* GetElement(VertexElementSemantic semantic, unsigned char index = 0) const;
		const VertexElement* GetElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index) const;

		bool HasElement(VertexElementSemantic semantic, unsigned char index = 0) const
		{
			return GetElement(semantic, index) != nullptr;
		}

		bool HasElement(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0) const
		{
			return GetElement(type, semantic, index) != nullptr;
		}

		unsigned GetElementOffset(VertexElementSemantic semantic, unsigned char index = 0) const
		{
			const VertexElement* element = GetElement(semantic, index);
			return element ? element->offset_ : M_MAX_UNSIGNED;
		}

		unsigned GetElementOffset(VertexElementType type, VertexElementSemantic semantic, unsigned char index = 0) const
		{
			const VertexElement* element = GetElement(type, semantic, index);
			return element ? element->offset_ : M_MAX_UNSIGNED;
		}

		unsigned GetElementMask() const
		{
			return elementMask_;
		}

		unsigned char* GetShadowData() const
		{
			return shadowData_.Get();
		}

		SharedArrayPtr<unsigned char> GetShadowDataShared() const
		{
			return shadowData_;
		}

		unsigned long long GetBufferHash(unsigned streamIndex)
		{
			return elementHash_ << (streamIndex * 16);
		}

		static const VertexElement* GetElement(const PODVector<VertexElement>& elements, VertexElementType type, VertexElementSemantic semantic,
		                                       unsigned char index = 0);

		static bool HasElement(const PODVector<VertexElement>& elements, VertexElementType type,
		                       VertexElementSemantic semantic, unsigned char index = 0);

		static unsigned GetElementOffset(const PODVector<VertexElement>& elements, VertexElementType type,
		                                 VertexElementSemantic semantic, unsigned char index = 0);


		// Return a vertex element list from a legacy element bitmask
		static PODVector<VertexElement> GetElements(unsigned elementMask);

		static unsigned GetVertexSize(const PODVector<VertexElement>& elements);

		static unsigned GetVertexSize(unsigned elementMask);

		static void UpdateOffsets(PODVector<VertexElement>& elements);

	private:
		void UpdateOffset();
		bool Create();
		bool UpdateToGPU();
		void* MapBuffer(unsigned start, unsigned count, bool discard);
		void UnmapBuffer();

		SharedArrayPtr<unsigned char> shadowData_;
		//todo, 这两个是怎么确定的？？
		unsigned vertexCount_;
		//Todo, the total size of all elements ??
		unsigned vertexSize_;
		PODVector<VertexElement> elements_;
		unsigned long long elementHash_;
		//Todo, what is this ?
		// Vertex element legacy bitmask
		unsigned elementMask_;
		LockState lockState_;
		unsigned lockStart_;
		unsigned lockCount_;
		void* lockScratchData_;
		bool dynamic_;
		bool shadowed_;
		bool discardLock_;
	};
}



#endif //URHO3DCOPY_VERTEXBUFFER_H
