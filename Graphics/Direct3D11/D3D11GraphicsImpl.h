//
// Created by liuhao1 on 2018/1/9.
//

#ifndef URHO3DCOPY_D3D11GRAPHICSIMPL_H
#define URHO3DCOPY_D3D11GRAPHICSIMPL_H

#include <d3d11.h>
#include <dxgi.h>
#include <unknown.h>

namespace Urho3D
{
#define URHO3D_SAFE_RELEASE(p) if(p) {((IUnknown*)p)->Release(); p = 0;}
}

#endif //URHO3DCOPY_D3D11GRAPHICSIMPL_H
