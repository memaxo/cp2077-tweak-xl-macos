#include "ModuleImage.hpp"

#if defined(_WIN32) || defined(_WIN64)
Core::ModuleImage::ModuleImage(HMODULE aHandle)
{
    std::wstring filePath;
    wil::GetModuleFileNameW(aHandle, filePath);

    m_path = filePath;
}
#else
Core::ModuleImage::ModuleImage(void* aHandle)
{
    std::string filePath = Core::Platform::GetModuleFileName(aHandle);
    m_path = filePath;
}
#endif

std::filesystem::path Core::ModuleImage::GetPath() const
{
    return m_path;
}

std::filesystem::path Core::ModuleImage::GetDir() const
{
    return m_path.parent_path();
}

std::string Core::ModuleImage::GetName() const
{
    return m_path.stem().string();
}

bool Core::ModuleImage::IsASI() const
{
#if defined(_WIN32) || defined(_WIN64)
    return m_path.extension() == L".asi";
#else
    return m_path.extension() == ".asi";
#endif
}
