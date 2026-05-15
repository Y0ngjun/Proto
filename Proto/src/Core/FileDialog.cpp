/*
 * 운영체제별 파일 탐색기 다이얼로그(열기, 저장) 기능을 제공하는 클래스입니다.
 * 현재 Windows 플랫폼의 네이티브 다이얼로그를 지원합니다.
 */

#include "FileDialog.h"
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <shobjidl.h>
#include <shlguid.h>
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "shell32.lib")
#endif

namespace Proto
{
#ifdef _WIN32
	// COM 라이브러리 초기화 및 해제를 자동화하는 래퍼 구조체입니다.
	struct ScopedCOM
	{
		HRESULT Hr;

		ScopedCOM()
		{
			Hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
		}

		~ScopedCOM()
		{
			if (SUCCEEDED(Hr))
			{
				CoUninitialize();
			}
		}

		bool Succeeded() const
		{
			return SUCCEEDED(Hr);
		}
	};

	static std::vector<COMDLG_FILTERSPEC> ParseFilter(const char* filter, std::vector<std::wstring>& storage)
	{
		std::vector<COMDLG_FILTERSPEC> specs;
		if (!filter || filter[0] == '\0')
		{
			storage.push_back(L"All Files");
			storage.push_back(L"*.*");
			specs.push_back({ storage[0].c_str(), storage[1].c_str() });
			return specs;
		}

		const std::string filterStr(filter);
		size_t pos = 0;
		while (pos < filterStr.length())
		{
			const std::string desc = &filter[pos];
			pos += desc.length() + 1;
			if (pos >= filterStr.length())
			{
				break;
			}

			const std::string spec = &filter[pos];
			pos += spec.length() + 1;

			wchar_t wDesc[256];
			MultiByteToWideChar(CP_UTF8, 0, desc.c_str(), -1, wDesc, 256);
			storage.push_back(wDesc);

			wchar_t wSpec[256];
			MultiByteToWideChar(CP_UTF8, 0, spec.c_str(), -1, wSpec, 256);
			storage.push_back(wSpec);
		}

		for (size_t i = 0; i < storage.size(); i += 2)
		{
			specs.push_back({ storage[i].c_str(), storage[i + 1].c_str() });
		}

		return specs;
	}

	static std::optional<std::string> GetDialogResult(IFileDialog* pDialog)
	{
		IShellItem* pItem;
		if (FAILED(pDialog->GetResult(&pItem)))
		{
			return std::nullopt;
		}

		PWSTR pszFilePath;
		if (FAILED(pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath)))
		{
			pItem->Release();
			return std::nullopt;
		}

		const int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, NULL, 0, NULL, NULL);
		std::string result(bufferSize - 1, 0);
		WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, &result[0], bufferSize, NULL, NULL);

		CoTaskMemFree(pszFilePath);
		pItem->Release();

		return result;
	}
#endif

	std::optional<std::string> FileDialog::OpenFile(const char* filter)
	{
#ifdef _WIN32
		const ScopedCOM com;
		if (!com.Succeeded())
		{
			return std::nullopt;
		}

		IFileOpenDialog* pFileOpen;
		if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen))))
		{
			return std::nullopt;
		}

		std::vector<std::wstring> storage;
		const auto specs = ParseFilter(filter, storage);
		if (!specs.empty())
		{
			pFileOpen->SetFileTypes(static_cast<UINT>(specs.size()), specs.data());
			pFileOpen->SetFileTypeIndex(1);
		}

		std::optional<std::string> result = std::nullopt;
		if (SUCCEEDED(pFileOpen->Show(NULL)))
		{
			result = GetDialogResult(pFileOpen);
		}

		pFileOpen->Release();
		return result;
#else
		return std::nullopt;
#endif
	}

	std::optional<std::string> FileDialog::SaveFile(const char* filter)
	{
#ifdef _WIN32
		const ScopedCOM com;
		if (!com.Succeeded())
		{
			return std::nullopt;
		}

		IFileSaveDialog* pFileSave;
		if (FAILED(CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave))))
		{
			return std::nullopt;
		}

		std::vector<std::wstring> storage;
		const auto specs = ParseFilter(filter, storage);
		if (!specs.empty())
		{
			pFileSave->SetFileTypes(static_cast<UINT>(specs.size()), specs.data());
			pFileSave->SetFileTypeIndex(1);

			// 기본 확장자 설정
			const std::wstring ext = storage[1];
			const size_t dotPos = ext.find_last_of(L'.');
			if (dotPos != std::wstring::npos)
			{
				pFileSave->SetDefaultExtension(ext.substr(dotPos + 1).c_str());
			}
		}

		std::optional<std::string> result = std::nullopt;
		if (SUCCEEDED(pFileSave->Show(NULL)))
		{
			result = GetDialogResult(pFileSave);
		}

		pFileSave->Release();
		return result;
#else
		return std::nullopt;
#endif
	}
}
