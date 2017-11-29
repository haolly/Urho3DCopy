//
// Created by liuhao on 2017/11/27.
//

#include <cassert>
#include "RefCounted.h"

namespace Urho3D
{
    RefCounted::RefCounted() :
        refCount_(new RefCount())
    {
        (refCount_->weakRefs_)++;
    }

    RefCounted::~RefCounted()
    {
        assert(refCount_);
        assert(refCount_->refs_ == 0);
        assert(refCount_->weakRefs_ > 0);

        refCount_->refs_ = -1;
        (refCount_->weakRefs_)--;
        if (!refCount_->weakRefs_)
        {
            delete refCount_;
        }
        //TODO, 这里是不是应该和 delete 放在一起？
        refCount_ = nullptr;
    }

    void RefCounted::AddRef()
    {
        assert(refCount_->refs_ >= 0);
        (refCount_->refs_)++;
    }

    void RefCounted::ReleaseRef()
    {
        assert(refCount_->refs_ > 0);
        (refCount_->refs_)--;
        if (!refCount_->refs_)
            delete this;
    }

    int RefCounted::Refs() const
    {
        return refCount_->refs_;
    }

    //TODO, weakref is used for what ??
    int RefCounted::WeakRefs() const
    {
        return refCount_->weakRefs_;
    }
}
