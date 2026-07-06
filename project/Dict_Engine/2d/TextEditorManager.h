#pragma once
#include "Text.h"

#include <memory>
#include <string>
#include <vector>

class TextEditorManager
{
public:
    static TextEditorManager* GetInstance();
    void Finalize();

    class ConstructorKey
    {
    private:
        ConstructorKey() = default;
        friend class TextEditorManager;
    };

    explicit TextEditorManager(ConstructorKey) {}

private:
    static std::unique_ptr<TextEditorManager> instance_;

    ~TextEditorManager() = default;
    TextEditorManager(TextEditorManager&) = delete;
    TextEditorManager& operator=(TextEditorManager&) = delete;

    friend struct std::default_delete<TextEditorManager>;

public:
    void Initialize();
    void Update(const std::string& sceneName);
    void Draw(const std::string& sceneName);

    bool Save();
    bool Load();

private:
    struct TextInputState
    {
        int cursorPos = 0;
        int selectionStart = 0;
        int selectionEnd = 0;
    };

    struct EditableText
    {
        uint32_t id = 0;
        std::string name;
        std::string sceneName;
        std::string richTextUtf8;
        std::string fontPath;
        std::string boldFontPath;
        std::string italicFontPath;
        std::string boldItalicFontPath;
        uint32_t pixelSize = 32;
        Vector2 position = { 0.0f, 0.0f };
        Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        bool visible = true;
        std::unique_ptr<Text> text;
        TextInputState textInputState;
    };

    void DrawEditorWindow(const std::string& sceneName);
    void DrawTextList(const std::string& sceneName);
    void DrawSelectedTextInspector();

    EditableText CreateDefaultText(const std::string& sceneName);
    void AddDefaultSample();
    void DuplicateSelectedText();
    void DeleteSelectedText();

    EditableText* FindText(uint32_t id);
    EditableText* FindSelectedText();
    void SelectFirstTextInScene(const std::string& sceneName);

    void EnsureTextObject(EditableText& entry);
    void ApplyTextContent(EditableText& entry);
    void ApplyFont(EditableText& entry);
    void ApplyTransformAndColor(EditableText& entry);
    void InsertTag(EditableText& entry, const std::string& openTag, const std::string& closeTag);

    std::string MakeTextName() const;
    std::string MakeColorTag() const;

private:
    std::vector<EditableText> texts_;
    uint32_t nextId_ = 1;
    uint32_t selectedId_ = 0;
    Vector4 tagColor_ = { 1.0f, 0.25f, 0.25f, 1.0f };
    std::string statusMessage_;
};
