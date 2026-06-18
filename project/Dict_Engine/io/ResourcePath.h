#pragma once
#include <string>
#include <filesystem>

class ResourcePath
{
public:
    static std::filesystem::path Root();

    static std::filesystem::path Make(const std::string& relativePath);

    static std::string MakeString(const std::string& relativePath);
};