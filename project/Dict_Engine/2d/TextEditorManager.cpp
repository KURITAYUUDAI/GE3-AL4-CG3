#include "TextEditorManager.h"

#include "ResourcePath.h"
#include "StringUtility.h"
#include "json.hpp"

#ifdef USE_IMGUI
#include "imgui.h"
#endif

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>

namespace
{
    constexpr const char* kDefaultFontPath = "fonts/x8y12pxTheStrongGamer.ttf";
    constexpr const char* kSaveRelativePath = "text_editor/texts.json";
    constexpr int kJsonVersion = 1;

#ifdef USE_IMGUI
    struct InputTextCallbackUserData
    {
        std::string* text = nullptr;
        int* cursorPos = nullptr;
        int* selectionStart = nullptr;
        int* selectionEnd = nullptr;
    };

    int InputTextCallback(ImGuiInputTextCallbackData* data)
    {
        auto* userData = static_cast<InputTextCallbackUserData*>(data->UserData);
        if (!userData || !userData->text)
        {
            return 0;
        }

        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        {
            std::string& text = *userData->text;
            text.resize(static_cast<size_t>(data->BufTextLen));
            data->Buf = text.data();
        }

        if (userData->cursorPos)
        {
            *userData->cursorPos = data->CursorPos;
        }
        if (userData->selectionStart)
        {
            *userData->selectionStart = data->SelectionStart;
        }
        if (userData->selectionEnd)
        {
            *userData->selectionEnd = data->SelectionEnd;
        }

        return 0;
    }

    bool InputTextString(
        const char* label,
        std::string& value,
        int* cursorPos = nullptr,
        int* selectionStart = nullptr,
        int* selectionEnd = nullptr,
        ImGuiInputTextFlags extraFlags = 0,
        const ImVec2& size = ImVec2(0.0f, 0.0f),
        bool multiline = false)
    {
        value.reserve(value.size() + 256);

        InputTextCallbackUserData userData{
            &value,
            cursorPos,
            selectionStart,
            selectionEnd
        };

        ImGuiInputTextFlags flags =
            extraFlags |
            ImGuiInputTextFlags_CallbackResize |
            ImGuiInputTextFlags_CallbackAlways;

        bool changed = false;
        if (multiline)
        {
            changed = ImGui::InputTextMultiline(
                label,
                value.data(),
                value.capacity() + 1,
                size,
                flags,
                InputTextCallback,
                &userData);
        }
        else
        {
            changed = ImGui::InputText(
                label,
                value.data(),
                value.capacity() + 1,
                flags,
                InputTextCallback,
                &userData);
        }

        if (changed)
        {
            value.resize(std::strlen(value.c_str()));
        }

        return changed;
    }
#endif

    uint8_t FloatToByte(float value)
    {
        value = std::clamp(value, 0.0f, 1.0f);
        return static_cast<uint8_t>(std::round(value * 255.0f));
    }

    Vector2 ReadVector2(const nlohmann::json& value, const Vector2& fallback)
    {
        if (!value.is_object())
        {
            return fallback;
        }

        return Vector2{
            value.value("x", fallback.x),
            value.value("y", fallback.y)
        };
    }

    Vector4 ReadVector4(const nlohmann::json& value, const Vector4& fallback)
    {
        if (!value.is_object())
        {
            return fallback;
        }

        return Vector4{
            value.value("x", fallback.x),
            value.value("y", fallback.y),
            value.value("z", fallback.z),
            value.value("w", fallback.w)
        };
    }
}

std::unique_ptr<TextEditorManager> TextEditorManager::instance_ = nullptr;

TextEditorManager* TextEditorManager::GetInstance()
{
    if (instance_ == nullptr)
    {
        instance_ = std::make_unique<TextEditorManager>(ConstructorKey());
    }

    return instance_.get();
}

void TextEditorManager::Finalize()
{
    texts_.clear();
    instance_.reset();
}

void TextEditorManager::Initialize()
{
    if (!Load())
    {
        AddDefaultSample();
        statusMessage_ = "No saved text file. Created default sample.";
    }
}

void TextEditorManager::Update(const std::string& sceneName)
{
#ifdef USE_IMGUI
    DrawEditorWindow(sceneName);
#endif

    for (EditableText& entry : texts_)
    {
        if (entry.sceneName != sceneName || !entry.visible)
        {
            continue;
        }

        EnsureTextObject(entry);
        entry.text->Update(false);
    }
}

void TextEditorManager::Draw(const std::string& sceneName)
{
    for (EditableText& entry : texts_)
    {
        if (entry.sceneName != sceneName || !entry.visible)
        {
            continue;
        }

        EnsureTextObject(entry);
        entry.text->Draw();
    }
}

bool TextEditorManager::Save()
{
    nlohmann::json root;
    root["version"] = kJsonVersion;
    root["texts"] = nlohmann::json::array();

    for (const EditableText& entry : texts_)
    {
        root["texts"].push_back({
            { "id", entry.id },
            { "name", entry.name },
            { "sceneName", entry.sceneName },
            { "richTextUtf8", entry.richTextUtf8 },
            { "fontPath", entry.fontPath },
            { "boldFontPath", entry.boldFontPath },
            { "italicFontPath", entry.italicFontPath },
            { "boldItalicFontPath", entry.boldItalicFontPath },
            { "pixelSize", entry.pixelSize },
            { "position", { { "x", entry.position.x }, { "y", entry.position.y } } },
            { "color", {
                { "x", entry.color.x },
                { "y", entry.color.y },
                { "z", entry.color.z },
                { "w", entry.color.w }
            } },
            { "visible", entry.visible }
        });
    }

    const std::filesystem::path savePath = ResourcePath::Make(kSaveRelativePath);
    std::filesystem::create_directories(savePath.parent_path());

    std::ofstream ofs(savePath);
    if (!ofs.is_open())
    {
        statusMessage_ = "Save failed.";
        return false;
    }

    ofs << std::setw(4) << root << '\n';
    statusMessage_ = "Saved " + savePath.generic_string();
    return true;
}

bool TextEditorManager::Load()
{
    const std::filesystem::path savePath = ResourcePath::Make(kSaveRelativePath);
    if (!std::filesystem::exists(savePath))
    {
        return false;
    }

    std::ifstream ifs(savePath);
    if (!ifs.is_open())
    {
        statusMessage_ = "Load failed.";
        return false;
    }

    nlohmann::json root;
    try
    {
        ifs >> root;
    }
    catch (const std::exception&)
    {
        statusMessage_ = "Load failed: invalid json.";
        return false;
    }

    if (!root.contains("texts") || !root["texts"].is_array())
    {
        statusMessage_ = "Load failed: missing texts.";
        return false;
    }

    std::vector<EditableText> loadedTexts;
    uint32_t maxId = 0;

    for (const nlohmann::json& item : root["texts"])
    {
        if (!item.is_object())
        {
            continue;
        }

        EditableText entry{};
        entry.id = item.value("id", 0u);
        if (entry.id == 0)
        {
            entry.id = nextId_++;
        }
        maxId = (std::max)(maxId, entry.id);

        entry.name = item.value("name", "Text");
        entry.sceneName = item.value("sceneName", "TITLE");
        entry.richTextUtf8 = item.value("richTextUtf8", "");
        entry.fontPath = item.value("fontPath", std::string(kDefaultFontPath));
        entry.boldFontPath = item.value("boldFontPath", "");
        entry.italicFontPath = item.value("italicFontPath", "");
        entry.boldItalicFontPath = item.value("boldItalicFontPath", "");
        entry.pixelSize = (std::max)(1u, item.value("pixelSize", 32u));
        entry.position = ReadVector2(item.value("position", nlohmann::json::object()), { 120.0f, 320.0f });
        entry.color = ReadVector4(item.value("color", nlohmann::json::object()), { 1.0f, 1.0f, 1.0f, 1.0f });
        entry.visible = item.value("visible", true);

        EnsureTextObject(entry);
        loadedTexts.push_back(std::move(entry));
    }

    texts_ = std::move(loadedTexts);
    nextId_ = (std::max)(nextId_, maxId + 1);
    selectedId_ = texts_.empty() ? 0 : texts_.front().id;
    statusMessage_ = "Loaded " + savePath.generic_string();
    return true;
}

void TextEditorManager::DrawEditorWindow(const std::string& sceneName)
{
#ifdef USE_IMGUI
    SelectFirstTextInScene(sceneName);

    ImGui::Begin("Runtime Text Editor");
    ImGui::Text("Scene: %s", sceneName.c_str());

    if (ImGui::Button("Add Text"))
    {
        EditableText entry = CreateDefaultText(sceneName);
        selectedId_ = entry.id;
        texts_.push_back(std::move(entry));
    }
    ImGui::SameLine();
    if (ImGui::Button("Duplicate"))
    {
        DuplicateSelectedText();
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete"))
    {
        DeleteSelectedText();
        SelectFirstTextInScene(sceneName);
    }
    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        Save();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        Load();
        SelectFirstTextInScene(sceneName);
    }

    if (!statusMessage_.empty())
    {
        ImGui::TextUnformatted(statusMessage_.c_str());
    }

    ImGui::Separator();
    DrawTextList(sceneName);
    ImGui::SameLine();
    DrawSelectedTextInspector();
    ImGui::End();
#else
    (void)sceneName;
#endif
}

void TextEditorManager::DrawTextList(const std::string& sceneName)
{
#ifdef USE_IMGUI
    ImGui::BeginChild("TextList", ImVec2(220.0f, 0.0f), true);
    for (EditableText& entry : texts_)
    {
        if (entry.sceneName != sceneName)
        {
            continue;
        }

        ImGui::PushID(static_cast<int>(entry.id));
        ImGui::Checkbox("##visible", &entry.visible);
        ImGui::SameLine();
        if (ImGui::Selectable(entry.name.c_str(), selectedId_ == entry.id))
        {
            selectedId_ = entry.id;
        }
        ImGui::PopID();
    }
    ImGui::EndChild();
#else
    (void)sceneName;
#endif
}

void TextEditorManager::DrawSelectedTextInspector()
{
#ifdef USE_IMGUI
    EditableText* entry = FindSelectedText();
    ImGui::BeginChild("TextInspector", ImVec2(0.0f, 0.0f), true);
    if (!entry)
    {
        ImGui::TextUnformatted("No text selected.");
        ImGui::EndChild();
        return;
    }

    ImGui::PushID(static_cast<int>(entry->id));

    if (InputTextString("Name", entry->name))
    {
        if (entry->name.empty())
        {
            entry->name = "Text";
        }
    }

    bool fontChanged = false;
    fontChanged |= InputTextString("Font", entry->fontPath);
    fontChanged |= InputTextString("Bold Font", entry->boldFontPath);
    fontChanged |= InputTextString("Italic Font", entry->italicFontPath);
    fontChanged |= InputTextString("Bold Italic Font", entry->boldItalicFontPath);

    int pixelSize = static_cast<int>(entry->pixelSize);
    if (ImGui::InputInt("Pixel Size", &pixelSize))
    {
        entry->pixelSize = static_cast<uint32_t>((std::max)(1, pixelSize));
        fontChanged = true;
    }

    if (fontChanged)
    {
        if (entry->fontPath.empty())
        {
            entry->fontPath = kDefaultFontPath;
        }
        ApplyFont(*entry);
        ApplyTextContent(*entry);
    }

    if (ImGui::DragFloat2("Position", &entry->position.x, 1.0f))
    {
        ApplyTransformAndColor(*entry);
    }
    if (ImGui::ColorEdit4("Color", &entry->color.x))
    {
        ApplyTransformAndColor(*entry);
    }

    ImGui::Separator();
    if (ImGui::Button("B"))
    {
        InsertTag(*entry, "<b>", "</b>");
    }
    ImGui::SameLine();
    if (ImGui::Button("I"))
    {
        InsertTag(*entry, "<i>", "</i>");
    }
    ImGui::SameLine();
    if (ImGui::Button("U"))
    {
        InsertTag(*entry, "<u>", "</u>");
    }
    ImGui::SameLine();
    ImGui::SetNextItemWidth(160.0f);
    ImGui::ColorEdit4("Tag Color", &tagColor_.x, ImGuiColorEditFlags_NoInputs);
    ImGui::SameLine();
    if (ImGui::Button("Color"))
    {
        InsertTag(*entry, MakeColorTag(), "</color>");
    }

    if (InputTextString(
        "Rich Text",
        entry->richTextUtf8,
        &entry->textInputState.cursorPos,
        &entry->textInputState.selectionStart,
        &entry->textInputState.selectionEnd,
        ImGuiInputTextFlags_AllowTabInput,
        ImVec2(-1.0f, 220.0f),
        true))
    {
        ApplyTextContent(*entry);
    }

    ImGui::PopID();
    ImGui::EndChild();
#endif
}

TextEditorManager::EditableText TextEditorManager::CreateDefaultText(const std::string& sceneName)
{
    EditableText entry{};
    entry.id = nextId_++;
    entry.name = "Text " + std::to_string(entry.id);
    entry.sceneName = sceneName;
    entry.richTextUtf8 =
        "TITLE <b><color=#ff4040>SCENE</color></b>\n"
        "<i><color=#40a0ff>ITALIC</color></i> <u><color=#40ff80>UNDER</color></u> "
        "<b><i><u><color=#ffd040cc>BOTH</color></u></i></b>";
    entry.fontPath = kDefaultFontPath;
    entry.pixelSize = 50;
    entry.position = { 120.0f, 320.0f };
    entry.color = { 1.0f, 1.0f, 1.0f, 1.0f };

    EnsureTextObject(entry);
    return entry;
}

void TextEditorManager::AddDefaultSample()
{
    EditableText entry = CreateDefaultText("TITLE");
    selectedId_ = entry.id;
    texts_.push_back(std::move(entry));
}

void TextEditorManager::DuplicateSelectedText()
{
    EditableText* source = FindSelectedText();
    if (!source)
    {
        return;
    }

    EditableText copy{};
    copy.id = nextId_++;
    copy.name = source->name + " Copy";
    copy.sceneName = source->sceneName;
    copy.richTextUtf8 = source->richTextUtf8;
    copy.fontPath = source->fontPath;
    copy.boldFontPath = source->boldFontPath;
    copy.italicFontPath = source->italicFontPath;
    copy.boldItalicFontPath = source->boldItalicFontPath;
    copy.pixelSize = source->pixelSize;
    copy.position = { source->position.x + 24.0f, source->position.y + 24.0f };
    copy.color = source->color;
    copy.visible = source->visible;

    EnsureTextObject(copy);
    selectedId_ = copy.id;
    texts_.push_back(std::move(copy));
}

void TextEditorManager::DeleteSelectedText()
{
    if (selectedId_ == 0)
    {
        return;
    }

    const uint32_t deletedId = selectedId_;
    texts_.erase(
        std::remove_if(
            texts_.begin(),
            texts_.end(),
            [deletedId](const EditableText& entry)
            {
                return entry.id == deletedId;
            }),
        texts_.end());

    selectedId_ = texts_.empty() ? 0 : texts_.front().id;
}

TextEditorManager::EditableText* TextEditorManager::FindText(uint32_t id)
{
    auto it = std::find_if(
        texts_.begin(),
        texts_.end(),
        [id](const EditableText& entry)
        {
            return entry.id == id;
        });

    return it == texts_.end() ? nullptr : &*it;
}

TextEditorManager::EditableText* TextEditorManager::FindSelectedText()
{
    return FindText(selectedId_);
}

void TextEditorManager::SelectFirstTextInScene(const std::string& sceneName)
{
    EditableText* selected = FindSelectedText();
    if (selected && selected->sceneName == sceneName)
    {
        return;
    }

    for (const EditableText& entry : texts_)
    {
        if (entry.sceneName == sceneName)
        {
            selectedId_ = entry.id;
            return;
        }
    }

    selectedId_ = 0;
}

void TextEditorManager::EnsureTextObject(EditableText& entry)
{
    if (entry.text)
    {
        return;
    }

    if (entry.fontPath.empty())
    {
        entry.fontPath = kDefaultFontPath;
    }
    entry.pixelSize = (std::max)(1u, entry.pixelSize);

    entry.text = std::make_unique<Text>();
    entry.text->InitializeRichText(
        entry.fontPath,
        entry.pixelSize,
        StringUtility::ConvertUtf8ToUtf32(entry.richTextUtf8));
    entry.text->SetFontStylePaths(
        entry.fontPath,
        entry.boldFontPath,
        entry.italicFontPath,
        entry.boldItalicFontPath);
    ApplyTransformAndColor(entry);
}

void TextEditorManager::ApplyTextContent(EditableText& entry)
{
    EnsureTextObject(entry);
    entry.text->SetRichText(StringUtility::ConvertUtf8ToUtf32(entry.richTextUtf8));
}

void TextEditorManager::ApplyFont(EditableText& entry)
{
    EnsureTextObject(entry);
    entry.pixelSize = (std::max)(1u, entry.pixelSize);
    entry.text->SetFont(entry.fontPath, entry.pixelSize);
    entry.text->SetFontStylePaths(
        entry.fontPath,
        entry.boldFontPath,
        entry.italicFontPath,
        entry.boldItalicFontPath);
}

void TextEditorManager::ApplyTransformAndColor(EditableText& entry)
{
    EnsureTextObject(entry);
    entry.text->SetPosition(entry.position);
    entry.text->SetColor(entry.color);
}

void TextEditorManager::InsertTag(EditableText& entry, const std::string& openTag, const std::string& closeTag)
{
    std::string& text = entry.richTextUtf8;
    int start = entry.textInputState.selectionStart;
    int end = entry.textInputState.selectionEnd;
    if (start > end)
    {
        std::swap(start, end);
    }

    start = std::clamp(start, 0, static_cast<int>(text.size()));
    end = std::clamp(end, 0, static_cast<int>(text.size()));

    if (start != end)
    {
        text.insert(static_cast<size_t>(end), closeTag);
        text.insert(static_cast<size_t>(start), openTag);
        entry.textInputState.cursorPos = end + static_cast<int>(openTag.size() + closeTag.size());
    }
    else
    {
        int cursor = std::clamp(entry.textInputState.cursorPos, 0, static_cast<int>(text.size()));
        text.insert(static_cast<size_t>(cursor), openTag + closeTag);
        entry.textInputState.cursorPos = cursor + static_cast<int>(openTag.size());
    }

    entry.textInputState.selectionStart = entry.textInputState.cursorPos;
    entry.textInputState.selectionEnd = entry.textInputState.cursorPos;
    ApplyTextContent(entry);
}

std::string TextEditorManager::MakeTextName() const
{
    return "Text " + std::to_string(nextId_);
}

std::string TextEditorManager::MakeColorTag() const
{
    char buffer[32]{};
    std::snprintf(
        buffer,
        sizeof(buffer),
        "<color=#%02x%02x%02x%02x>",
        FloatToByte(tagColor_.x),
        FloatToByte(tagColor_.y),
        FloatToByte(tagColor_.z),
        FloatToByte(tagColor_.w));
    return buffer;
}
