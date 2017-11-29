//
// Created by liuhao on 2017/11/27.
//

#ifndef URHO3DCOPY_REFCOUNTED_H
#define URHO3DCOPY_REFCOUNTED_H

namespace Urho3D
{
    struct RefCount
    {
        RefCount():refs_(0), weakRefs_(0)
        {

        }
        ~RefCount()
        {
            refs_ = -1;
            weakRefs_ = -1;
        }
        int refs_;
        int weakRefs_;
    };


    class RefCounted
    {
    public:
        RefCounted();
        virtual ~RefCounted();

        void AddRef();
        void ReleaseRef();
        int Refs() const;
        int WeakRefs() const;
        RefCount* RefCountPtr()
        {
            return refCount_;
        }

    private:
        RefCounted(const RefCounted& rhs);
        RefCounted& operator=(const RefCounted& rhs);
        RefCount* refCount_;
    };
}



#endif //URHO3DCOPY_REFCOUNTED_H
