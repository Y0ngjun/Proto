#include "FileDialog.h"

#ifdef _WIN32
    #include <windows.h>
    #include <shobjidl.h>
    #include <shlguid.h>
    #pragma comment(lib, "ole32.lib")
    #pragma comment(lib, "shell32.lib")
#endif

namespace Proto
{
    std::optional<std::string> FileDialog::OpenFile(const char* filter)
    {
#ifdef _WIN32
        // COM 초기화
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr))
            return std::nullopt;

        IFileOpenDialog* pFileOpen;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // 필터 설정 (간단하게 YAML 파일로)
            COMDLG_FILTERSPEC rgSpec[] = {
                { L"YAML Files", L"*.yaml" },
                { L"All Files", L"*.*" }
            };
            pFileOpen->SetFileTypes(2, rgSpec);
            pFileOpen->SetFileTypeIndex(1);

            // 초기 경로를 scenes 폴더로 설정
            IShellItem* psiFolder;
            hr = SHCreateItemFromParsingName(L"scenes", NULL, IID_IShellItem, reinterpret_cast<void**>(&psiFolder));
            if (SUCCEEDED(hr))
            {
                pFileOpen->SetDefaultFolder(psiFolder);
                psiFolder->Release();
            }

            // 다이얼로그 표시
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
                        // Wide string을 일반 string으로 변환
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
        // COM 초기화
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        if (FAILED(hr))
            return std::nullopt;

        IFileSaveDialog* pFileSave;
        hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

        if (SUCCEEDED(hr))
        {
            // 필터 설정
            COMDLG_FILTERSPEC rgSpec[] = {
                { L"YAML Files", L"*.yaml" },
                { L"All Files", L"*.*" }
            };
            pFileSave->SetFileTypes(2, rgSpec);
            pFileSave->SetFileTypeIndex(1);

            // 초기 경로를 scenes 폴더로 설정
            IShellItem* psiFolder;
            hr = SHCreateItemFromParsingName(L"scenes", NULL, IID_IShellItem, reinterpret_cast<void**>(&psiFolder));
            if (SUCCEEDED(hr))
            {
                pFileSave->SetDefaultFolder(psiFolder);
                psiFolder->Release();
            }

            // 기본 파일명 설정
            pFileSave->SetFileName(L"scene.yaml");

            // 다이얼로그 표시
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
                        // Wide string을 일반 string으로 변환
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
