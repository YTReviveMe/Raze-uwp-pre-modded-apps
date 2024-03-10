#include <wrl.h>
#include <wrl/client.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <pix.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <agile.h>
#include <concrt.h>

#include "i_interface.h"

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;

FString uwp_GetCWD()
{
    char maxPath[MAX_PATH];
    GetCurrentDirectoryA(sizeof(maxPath), maxPath);
    return maxPath;
}

unsigned int GenerateRandomNumber()
{
    // Generate a random number.
    unsigned int random = Windows::Security::Cryptography::CryptographicBuffer::GenerateRandomNumber();
    return random;
}

unsigned int uwp_MakeRNGSeed()
{
    return GenerateRandomNumber();
}

void RedrawProgressBar(int CurPos, int MaxPos)
{
    //todo
}
void CleanProgressBar()
{
    //todo
}