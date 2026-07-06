#define NOMINMAX
#include "FreeTypeManager.h"
#include "SRVManager.h"
#include "Logger.h"
#include "ResourcePath.h"

std::unique_ptr<FreeTypeManager> FreeTypeManager::instance_ = nullptr;


FreeTypeManager* FreeTypeManager::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = std::make_unique<FreeTypeManager>(ConstructorKey());
    }

    return instance_.get();
}

void FreeTypeManager::Finalize()
{
    for (auto& [path, face] : faces_)
    {
        FT_Done_Face(face);
    }
    faces_.clear();

    if (library_)
    {
        FT_Done_FreeType(library_);
        library_ = nullptr;
    }

    instance_.reset();
}

void FreeTypeManager::Initialize()
{
    FT_Error error = FT_Init_FreeType(&library_);
    if (error)
    {
        OutputDebugStringA("FT_Init_FreeType failed\n");
    }
}

FT_Face FreeTypeManager::GetOrLoadFace(const std::string& fontPath)
{
    assert(library_ != nullptr && "FreeTypeManager::Initialize()が呼ばれていません");

    auto it = faces_.find(fontPath);
    if (it != faces_.end())
    {
        return it->second;
    }

    FT_Face face = nullptr;

	std::string  fullPath = ResourcePath::MakeString(fontPath);
    FT_Error error = FT_New_Face(library_, fullPath.c_str(), 0, &face);
    if (error)
    {
        char buffer[256]{};
        sprintf_s(buffer, "FT_New_Face failed. path=%s error=0x%02X\n", fullPath.c_str(), error);
        OutputDebugStringA(buffer);
        return nullptr;
    }

    faces_[fontPath] = face;
    return face;
}

