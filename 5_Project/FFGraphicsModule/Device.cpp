#include "GraphicsPch.h"
#include "Device.h"

#include <filesystem>
#include <shlobj.h>

/// GPU 캡처용
std::wstring GetLatestWinPixGpuCapturerPath_Cpp17()
{
	LPWSTR programFilesPath = nullptr;
	SHGetKnownFolderPath(FOLDERID_ProgramFiles, KF_FLAG_DEFAULT, NULL, &programFilesPath);

	std::filesystem::path pixInstallationPath = programFilesPath;
	pixInstallationPath /= "Microsoft PIX";

	std::wstring newestVersionFound;

	for (auto const& directory_entry : std::filesystem::directory_iterator(pixInstallationPath))
	{
		if (directory_entry.is_directory())
		{
			if (newestVersionFound.empty() || newestVersionFound < directory_entry.path().filename().c_str())
			{
				newestVersionFound = directory_entry.path().filename().c_str();
			}
		}
	}

	if (newestVersionFound.empty())
	{
		// TODO: Error, no PIX installation found
	}

	return pixInstallationPath / newestVersionFound / L"WinPixGpuCapturer.dll";
}


void Device::Init()
{
	///if (GetModuleHandle(L"WinPixGpuCapturer.dll") == 0)
	///{
	///	LoadLibrary(GetLatestWinPixGpuCapturerPath_Cpp17().c_str());
	///}

	// D3D12 디버그층 활성화
// VC++ 출력창에 상세한 디버깅 메세지 출력
// riid : 디바이스의 COM ID
// ppDevice : 생성된 장치가 매개변수에 설정
#ifdef _DEBUG
	::D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugController));
	m_debugController->EnableDebugLayer();
#endif

	// DXGI(DirectX Graphics Infrastructure)
	// Direct3D와 함께 쓰이는 API
	// 전체화면 모드 전환
	// 지원되는 디스플레이 모드 열거
	// CreateDXGIFactory
	// riid : 디바이스의 COM ID
	// ppDevice : 생성된 장치가 매개변수에 설정
	::CreateDXGIFactory(IID_PPV_ARGS(&m_dxgi));

	// CreateDevice
	// 디스플레이 어댑터 (그래픽 카드)를 나타내는 객체
	// pAdapter : nullptr로 지정하면 시스템 기본 디스플레이 어댑터
	// MinimumFeatureLevel : 응용 프로그램이 요구하는 최소 기능 수준
	// riid : 디바이스의 COM ID
	// ppDevice : 생성된 장치가 매개변수에 설정
	::D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
}
