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
    // 필터 문자열을 COMDLG_FILTERSPEC 배열로 변환하는 헬퍼 함수
    // 형식: "Description\0*.ext\0" -> { {L"Description", L"*.ext"} }
    static std::vector<COMDLG_FILTERSPEC> ParseFilter(const char* filter, std::vector<std::wstring>& storage)
    {
        std::vector<COMDLG_FILTERSPEC> specs;
        if (!filter || filter[0] == '\0')
        {
            storage.push_back(L"All Files"); storage.push_back(L"*.*");
            specs.push_back({ storage[0].c_str(), storage[1].c_str() });
            return specs;
        }

        std::string f(filter);
        size_t pos = 0;
        while (pos < f.length())
        {
            // Description 추출
            std::string desc = &filter[pos];
            pos += desc.length() + 1;
            if (pos >= f.length()) break;

            // Pattern 추출
            std::string spec = &filter[pos];
            pos += spec.length() + 1;

            // Wide string으로 변환하여 저장 (포인터 유지를 위해 storage 사용)
            wchar_t wDesc[256];
            MultiByteToWideChar(CP_UTF8, 0, desc.c_str(), -1, wDesc, 256);
            storage.push_back(wDesc);

            wchar_t wSpec[256];
            MultiByteToWideChar(CP_UTF8, 0, spec.c_str(), -1, wSpec, 256);
            storage.push_back(wSpec);
        }

        for (size_t i = 0; i < storage.size(); i += 2)
        {
            specs.push_back({ storage[i].c_str(), storage[i+1].c_str() });
        }

        return specs;
    }
#endif

    std::optional<std::string> FileDialog::OpenFile(const char* filter)
    {
#ifdef _WIN32
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr)) return std::nullopt;

        IFileOpenDialog* pFileOpen;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            std::vector<std::wstring> storage;
            auto specs = ParseFilter(filter, storage);
            
            if (!specs.empty())
            {
                pFileOpen->SetFileTypes((UINT)specs.size(), specs.data());
                pFileOpen->SetFileTypeIndex(1);
            }

            hr = pFileOpen->Show(NULL);

            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr))
                    {
                        int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, NULL, 0, NULL, NULL);
                        std::string result(bufferSize - 1, 0);
                        WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, &result[0], bufferSize, NULL, NULL);

                        CoTaskMemFree(pszFilePath);
                        pItem->Release();
                        pFileOpen->Release();
                        CoUninitialize();
                        return result;
                    }
                    pItem->Release();
                }
            }
            pFileOpen->Release();
        }
        CoUninitialize();
#endif
        return std::nullopt;
    }

    std::optional<std::string> FileDialog::SaveFile(const char* filter)
    {
#ifdef _WIN32
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr)) return std::nullopt;

        IFileSaveDialog* pFileSave;
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

        if (SUCCEEDED(hr))
        {
            std::vector<std::wstring> storage;
            auto specs = ParseFilter(filter, storage);
            
            if (!specs.empty())
            {
                pFileSave->SetFileTypes((UINT)specs.size(), specs.data());
                pFileSave->SetFileTypeIndex(1);
            }

            // 기본 확장자 설정 (첫 번째 필터 기준)
            if (!storage.empty())
            {
                std::wstring ext = storage[1];
                size_t dotPos = ext.find_last_of(L'.');
                if (dotPos != std::wstring::npos)
                {
                    pFileSave->SetDefaultExtension(ext.substr(dotPos + 1).c_str());
                }
            }

            hr = pFileSave->Show(NULL);

            if (SUCCEEDED(hr))
            {
                IShellItem* pItem;
                hr = pFileSave->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    PWSTR pszFilePath;
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                    if (SUCCEEDED(hr))
                    {
                        int bufferSize = WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, NULL, 0, NULL, NULL);
                        std::string result(bufferSize - 1, 0);
                        WideCharToMultiByte(CP_UTF8, 0, pszFilePath, -1, &result[0], bufferSize, NULL, NULL);

                        CoTaskMemFree(pszFilePath);
                        pItem->Release();
                        pFileSave->Release();
                        CoUninitialize();
                        return result;
                    }
                    pItem->Release();
                }
            }
            pFileSave->Release();
        }
        CoUninitialize();
#endif
        return std::nullopt;
    }
}
