#include "Text.h"
#include "PSOManager.h"
#include "ImGuiManager.h"

void Text::Initialize(const std::string& fontPath, uint32_t pixelSize, const std::u32string& text)
{
	textManager_ = TextManager::GetInstance();

	fontKey_ = 
    {
		fontPath,
		pixelSize
    };

	fontAtlas_ = FontManager::GetInstance()->LoadFont(fontPath, pixelSize);

    CreateVertexResource();
    CreateMaterialResource();

    worldTransform_.Initialize();
    worldTransform_.translate_ = Vector3{ position_.x, position_.y, 0.0f };

    SetText(text);
}

void Text::Update()
{
#ifdef _DEBUG

    ImGui::Begin("Text Setting");

	Vector3 newTranslate = worldTransform_.translate_;
    if(ImGui::DragFloat3("translate", &newTranslate.x, 0.1f))
    {
        worldTransform_.translate_ = newTranslate;
    }

	Vector4 newColor = materialData_->color;
	if (ImGui::ColorEdit4("color", &newColor.x))
	{
		materialData_->color = newColor;
	}

    ImGui::End();

#endif

    worldTransform_.SetRotate(Vector3{ 0.0f, 0.0f, 0.0f });
    worldTransform_.scale_ = Vector3{ 1.0f, 1.0f, 1.0f };

    worldTransform_.UpdateMatrix();

    // WorldViewProjectionMatrixを作る
    Matrix4x4 viewMatrix = MakeIdentity4x4();
    Matrix4x4 projectionMatrix = MakeOrthographicMatrix(
        0.0f, 0.0f, float(WindowsAPI::kClientWidth), float(WindowsAPI::kClientHeight), 0.0f, 100.0f);
    Matrix4x4 viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);

    worldTransform_.TransferMatrix(viewProjectionMatrix);

    
}

void Text::Draw()
{
    auto psoSet = PSOManager::GetInstance()->GetPSOData(
       textManager_->GetDefaultPsoName(), textManager_->GetDefaultBlendMode(), PSOManager::FillMode::kSolid);

    auto commandList = DirectXBase::GetInstance()->GetCommandList();
    commandList->SetPipelineState(psoSet.pipelineState.Get());

    commandList->SetGraphicsRootSignature(psoSet.rootSignature.Get());
    
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    
    commandList->IASetIndexBuffer(&indexBufferView_);
    
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    
    worldTransform_.SetCBufferTransformationResource(1);
    
    commandList->SetGraphicsRootDescriptorTable(2, FontManager::GetInstance()->GetSRVHandleGPU(fontKey_));

    commandList->DrawIndexedInstanced(currentIndexCount_, 1, 0, 0, 0);
}

void Text::Finalize()
{

}

void Text::SetText(const std::u32string & text)
{
	text_ = text;

    RebuildMesh();
}

void Text::CreateVertexResource()
{
    const uint32_t vertexCount = kMaxTextLength * 4;
    const uint32_t indexCount = kMaxTextLength * 6;

    vertexResource_ = DirectXBase::GetInstance()->CreateBufferResource(
        sizeof(VertexData) * vertexCount
    );

    indexResource_ = DirectXBase::GetInstance()->CreateBufferResource(
        sizeof(uint32_t) * indexCount
    );

    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * vertexCount;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);

    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * indexCount;
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

    indexData_[0] = 0;
    indexData_[1] = 1;
    indexData_[2] = 2;
    indexData_[3] = 1;
    indexData_[4] = 3;
    indexData_[5] = 2;
}

void Text::CreateMaterialResource()
{
    // マテリアルリソースを作成する。
    materialResource_ = DirectXBase::GetInstance()->CreateBufferResource(sizeof(Material));
    // MaterialResourceにデータを書き込むためのアドレスを取得してMaterialDataに割り当てる
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

    // マテリアルデータの初期値を書き込む
    materialData_->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
}

void Text::RebuildMesh()
{
	fontAtlas_ = FontManager::GetInstance()->LoadFont(fontKey_.path, fontKey_.pixelSize);

    if (!vertexData_ || !indexData_ || !fontAtlas_)
    {
        return;
    }

    currentIndexCount_ = 0;

    float penX = 0.0f;
    float penY = 0.0f;

    uint32_t quadCount = 0;

    for (char32_t c : text_)
    {
        if (quadCount >= kMaxTextLength)
        {
            break;
        }

        if (c == U'\n')
        {
            penX = 0.0f;
            penY += lineHeight_;
            continue;
        }

        auto it = fontAtlas_->glyphs.find(c);
        if (it == fontAtlas_->glyphs.end())
        {
            // 未対応文字は ? に置き換える
            it = fontAtlas_->glyphs.find(U'?');
            if (it == fontAtlas_->glyphs.end())
            {
                continue;
            }
        }

        const GlyphInfo& glyph = it->second;

        // スペースなど、画像を持たない文字
        if (glyph.width == 0 || glyph.height == 0)
        {
            penX += static_cast<float>(glyph.advance);
            continue;
        }

        const float left =
            penX + static_cast<float>(glyph.bearingX);

        const float top =
            penY + (lineHeight_ - static_cast<float>(glyph.bearingY));

        const float right =
            left + static_cast<float>(glyph.width);

        const float bottom =
            top + static_cast<float>(glyph.height);

        const uint32_t vertexBase = quadCount * 4;
        const uint32_t indexBase = quadCount * 6;

        vertexData_[vertexBase + 0].position = { left,  bottom, 0.0f, 1.0f };
        vertexData_[vertexBase + 1].position = { left,  top,    0.0f, 1.0f };
        vertexData_[vertexBase + 2].position = { right, bottom, 0.0f, 1.0f };
        vertexData_[vertexBase + 3].position = { right, top,    0.0f, 1.0f };

        vertexData_[vertexBase + 0].texcoord = { glyph.uvMin.x, glyph.uvMax.y };
        vertexData_[vertexBase + 1].texcoord = { glyph.uvMin.x, glyph.uvMin.y };
        vertexData_[vertexBase + 2].texcoord = { glyph.uvMax.x, glyph.uvMax.y };
        vertexData_[vertexBase + 3].texcoord = { glyph.uvMax.x, glyph.uvMin.y };

        vertexData_[vertexBase + 0].normal = { 0.0f, 0.0f, -1.0f };
        vertexData_[vertexBase + 1].normal = { 0.0f, 0.0f, -1.0f };
        vertexData_[vertexBase + 2].normal = { 0.0f, 0.0f, -1.0f };
        vertexData_[vertexBase + 3].normal = { 0.0f, 0.0f, -1.0f };

        indexData_[indexBase + 0] = vertexBase + 0;
        indexData_[indexBase + 1] = vertexBase + 1;
        indexData_[indexBase + 2] = vertexBase + 2;
        indexData_[indexBase + 3] = vertexBase + 1;
        indexData_[indexBase + 4] = vertexBase + 3;
        indexData_[indexBase + 5] = vertexBase + 2;

        penX += static_cast<float>(glyph.advance);

        quadCount++;
    }

    currentIndexCount_ = quadCount * 6;
}
