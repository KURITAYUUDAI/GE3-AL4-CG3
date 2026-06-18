#include "ResourcePath.h"
#include "Logger.h"
#include <algorithm>

std::filesystem::path ResourcePath::Root()
{
    return "resources";
}

std::filesystem::path ResourcePath::Make(const std::string& relativePath)
{
    std::string path = relativePath;

    // 区切り文字を / に統一
    std::replace(path.begin(), path.end(), '\\', '/');

    // 先頭の / を削る
    while (!path.empty() && path.front() == '/')
    {
        path.erase(path.begin());
    }

    // すでに resources/ から始まっている場合は二重にしない
    if (path.starts_with("resources/") || path.starts_with("Resources/"))
    {
        return std::filesystem::path(path);
    }

   /* Logger::Log(("relativePath = [" + relativePath + "]\n").c_str());
    Logger::Log(("path.is_absolute = " + std::to_string(path.is_absolute()) + "\n").c_str());
    Logger::Log(("result = [" + (Root() / path).generic_string() + "]\n").c_str());*/

    /*OutputDebugStringA*/

    return Root() / path;
}

std::string ResourcePath::MakeString(const std::string& relativePath)
{
    return Make(relativePath).generic_string();
}
