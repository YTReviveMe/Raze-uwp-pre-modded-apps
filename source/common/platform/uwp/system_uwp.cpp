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

std::string PlatformToStd(Platform::String^ inStr) {
	std::wstring str2(inStr->Begin());
	std::string str3(str2.begin(), str2.end());
	return str3;
}

Platform::String^ StdToPlatform(std::string inStr) {
	std::wstring wid_str = std::wstring(inStr.begin(), inStr.end());
	const wchar_t* w_char = wid_str.c_str();
	return ref new Platform::String(w_char);
}

FString uwp_GetCWD()
{
    char maxPath[MAX_PATH];
    GetCurrentDirectoryA(sizeof(maxPath), maxPath);
    return maxPath;
}

FString uwp_GetAppDataPath()
{
    return PlatformToStd(Windows::Storage::ApplicationData::Current->LocalFolder->Path).c_str();
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

template <typename T>
void WaitForAsync(IAsyncOperation<T>^ A)
{
    while (A->Status == Windows::Foundation::AsyncStatus::Started) {
        CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
    }

    Windows::Foundation::AsyncStatus S = A->Status;
}

int uwp_ChooseWad(WadStuff* wads, int numwads, int defaultiwad, int& autoloadflags)
{
    int selected = defaultiwad;
    Windows::UI::Popups::PopupMenu^ popupmenu = ref new Windows::UI::Popups::PopupMenu();
    for (int i = 0; i < numwads; ++i)
    {
        popupmenu->Commands->Append(
            ref new Windows::UI::Popups::UICommand(
                StdToPlatform(wads[i].Name.GetChars()),
                ref new Windows::UI::Popups::UICommandInvokedHandler([&selected, i](Windows::UI::Popups::IUICommand^ command)
                    {
                        selected = i;
                    }
                )
            )
        );
    }
    WaitForAsync(popupmenu->ShowForSelectionAsync(CoreWindow::GetForCurrentThread()->Bounds));
    return selected;
}