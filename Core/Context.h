//
// Created by liuhao on 2017/11/27.
//

#ifndef URHO3DCOPY_CONTEXT_H
#define URHO3DCOPY_CONTEXT_H

#include "../Container/RefCounted.h"
#include "Object.h"
#include "../Math/StringHash.h"

namespace Urho3D
{

    class EventReceiverGroup : public RefCounted
    {
    public:
	    EventReceiverGroup() :
			inSend_(0),
			dirty_(false)
	    {
	    }

	    void BeginSendEvent();
	    void EndSendEvent();
	    void Add(Object* object);
	    void Remove(Object* object);

		//todo

    private:
	    unsigned inSend_;
	    bool dirty_;
    };

    class Context : public RefCounted
    {

    };
}


#endif //URHO3DCOPY_CONTEXT_H
