//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Effects.h"
#include "PosEffect.h"
#pragma region Effects

BasicEffect* Effects::BasicFX = 0;
BasicEffect* Effects::BasicTexFX = 0;
SkinnedBasicEffect* Effects::skinnedBasicTexFX = 0;
ScreenImageEffect* Effects::screenImageFX = 0;
PosEffect* Effects::Pos = 0;

void Effects::InitAll(ID3D11Device* device)
{
    BasicFX = new BasicEffect(device, L"./Basic.cso");
    BasicTexFX = new BasicEffect(device, L"./BasicTex.cso");
    skinnedBasicTexFX = new SkinnedBasicEffect(device, L"./SkinnedBasicTex.cso");
    screenImageFX = new ScreenImageEffect(device, L"./ScreenImage.cso");
    Pos = new PosEffect(device, L"./Pos.cso");

    //BasicFX = new BasicEffect(device, L"Basic.cso");
    //BasicTexFX = new BasicEffect(device, L"BasicTex.cso");
    //Pos = new PosEffect(device, L"Pos.cso");
}

void Effects::DestroyAll()
{
    SafeDelete(BasicFX);
    SafeDelete(BasicTexFX);
    SafeDelete(skinnedBasicTexFX);
    SafeDelete(Pos);
}
#pragma endregion


