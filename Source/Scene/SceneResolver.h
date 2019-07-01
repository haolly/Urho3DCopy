//
// Created by liuhao1 on 2018/1/23.
//

#ifndef URHO3DCOPY_SCENERESOLVER_H
#define URHO3DCOPY_SCENERESOLVER_H

#include "../Container/Ptr.h"
#include "../Container/HashMap.h"

namespace Urho3D
{
	class Component;
	class Node;

	class SceneResolver
	{
	public:
		SceneResolver();
		~SceneResolver();

		void Reset();
		void AddNode(unsigned oldID, Node* node);
		void AddComponent(unsigned oldID, Component* component);
		void Resolve();
	private:
		HashMap<unsigned, WeakPtr<Node>> nodes_;
		HashMap<unsigned, WeakPtr<Component>> components_;
	};

}



#endif //URHO3DCOPY_SCENERESOLVER_H
