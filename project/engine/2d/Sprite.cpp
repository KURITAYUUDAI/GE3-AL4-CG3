#include "Sprite.h"
#include "SpriteBase.h"
#include "TextureManager.h"
#include "SrvManager.h"

void Sprite::Initialize(SpriteBase* spriteBase, std::string textureFilePath)
{
	// スプライトの共通処理を受け取る
	spriteBase_ = spriteBase;

	CreateVertexResource();

	CreateMaterialResource();

	CreateTransformationMatrixResource();

	// テクスチャインデックスを受け取る
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);

	AdjustTextureSize();
}

void Sprite::Update()
{


	float left = 0.0f - anchorPoint_.x;
	float right = 1.0f - anchorPoint_.x;
	float top = 0.0f - anchorPoint_.y;
	float bottom = 1.0f - anchorPoint_.y;

	if (isFlipX_)
	{
		left = -left;
		right = -right;
	}

	if (isFlipY_)
	{
		top = -top;
		bottom = -bottom;
	}

	vertexData_[0].position = { left, bottom, 0.0f, 1.0f };
	vertexData_[1].position = { left, top, 0.0f, 1.0f };
	vertexData_[2].position = { right, bottom, 0.0f, 1.0f };
	vertexData_[3].position = { right, top, 0.0f, 1.0f };

	// transformに値を代入
	transform_.translate = Vector3{ position_.x, position_.y, 0.0f };
	transform_.rotate = Vector3{ 0.0f, 0.0f, rotation_ };
	transform_.scale = Vector3{ size_.x, size_.y, 1.0f };

	// WorldViewProjectionMatrixを作る
	worldMatrix_ = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(
		0.0f, 0.0f, float(WindowsAPI::kClientWidth), float(WindowsAPI::kClientHeight), 0.0f, 100.0f);
	worldViewProjectionMatrix_ = Multiply(worldMatrix_, Multiply(viewMatrix, projectionMatrix));

	// 座標変換行列データに各行列をコピー
	transformationMatrixData_->World = worldMatrix_;
	transformationMatrixData_->WVP = worldViewProjectionMatrix_;

	const DirectX::TexMetadata& metaData =
		TextureManager::GetInstance()->GetMetaData(textureIndex_);
	float tex_left = textureLeftTop_.x / metaData.width;
	float tex_right = (textureLeftTop_.x + textureSize_.x) / metaData.width;
	float tex_top = textureLeftTop_.y / metaData.height;
	float tex_bottom = (textureLeftTop_.y + textureSize_.y) / metaData.height;

	// 頂点リソースにデータを書き込む
	vertexData_[0].texcoord = { tex_left, tex_bottom };
	vertexData_[1].texcoord = { tex_left, tex_top };
	vertexData_[2].texcoord = { tex_right, tex_bottom };
	vertexData_[3].texcoord = { tex_right, tex_top };

	uvTransformMatrix_ = MakeScaleMatrix(uvTransform_.scale);
	uvTransformMatrix_ = Multiply(uvTransformMatrix_, MakeRotateZMatrix(uvTransform_.rotate.z));
	uvTransformMatrix_ = Multiply(uvTransformMatrix_, MakeTranslateMatrix(uvTransform_.translate));
	materialData_->uvTransform = uvTransformMatrix_;

}

void Sprite::Draw()
{
	// Spriteの描画。変更が必要なものだけ変更する。
	spriteBase_->GetDxBase()->GetCommandList()->SetPipelineState(spriteBase_->GetGraphicsPipeLineState());	// PS0を設定

	spriteBase_->GetDxBase()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	// SpriteのIndexBufferViewを設定
	spriteBase_->GetDxBase()->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	// SpriteのマテリアルのCBufferの場所を設定
	spriteBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	// TransformationMatrixCBufferの」場所を設定
	spriteBase_->GetDxBase()->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource_->GetGPUVirtualAddress());
	// SRV用のDescriptorTableの先頭を設定。2はrootParameter[2]である。
	spriteBase_->GetDxBase()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSRVHandleGPU(textureIndex_));

	// 描画！(DrawCall)
	spriteBase_->GetDxBase()->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Sprite::Finalize()
{
}

void Sprite::SetTexture(std::string textureFilePath)
{
	// テクスチャインデックスを受け取る
	textureIndex_ = TextureManager::GetInstance()->GetTextureIndexByFilePath(textureFilePath);
}

void Sprite::CreateVertexResource()
{
	// VertexResourceを作成する。indexResourceを使うので頂点数は4。
	vertexResource_ = spriteBase_->GetDxBase()->CreateBufferResource(sizeof(VertexData) * 4);

	// IndexResourceを作成する。
	indexResource_ = spriteBase_->GetDxBase()->CreateBufferResource(sizeof(uint32_t) * 6);

	// VertexBufferViewの設定
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点4つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// IndexBufferViewの設定
	// リソースの先頭のアドレスから使う
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	// VertexResourceにデータを書き込むためのアドレスを取得してVertexDataに割り当てる
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 1枚目の三角形
	vertexData_[0].position = { 0.0f, 1.0f, 0.0f, 1.0f };
	vertexData_[0].texcoord = { 0.0f, 1.0f };
	vertexData_[0].normal = { 0.0f, 0.0f, -1.0f };
	vertexData_[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexData_[1].texcoord = { 0.0f, 0.0f };
	vertexData_[1].normal = { 0.0f, 0.0f, -1.0f };
	vertexData_[2].position = { 1.0f, 1.0f, 0.0f, 1.0f };
	vertexData_[2].texcoord = { 1.0f, 1.0f };
	vertexData_[2].normal = { 0.0f, 0.0f, -1.0f };
	vertexData_[3].position = { 1.0f, 0.0f, 0.0f, 1.0f };
	vertexData_[3].texcoord = { 1.0f, 0.0f };
	vertexData_[3].normal = { 0.0f, 0.0f, -1.0f };

	// IndexResourceにデータを書き込むためのアドレスを取得してIndexDataに割り当てる
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
	indexData_[0] = 0;		indexData_[1] = 1;		indexData_[2] = 2;
	indexData_[3] = 1;		indexData_[4] = 3;		indexData_[5] = 2;
}

void Sprite::CreateMaterialResource()
{
	// マテリアルリソースを作成する。
	materialResource_ = spriteBase_->GetDxBase()->CreateBufferResource(sizeof(Material));
	// MaterialResourceにデータを書き込むためのアドレスを取得してMaterialDataに割り当てる
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	
	// マテリアルデータの初期値を書き込む
	materialData_->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();
}

void Sprite::CreateTransformationMatrixResource()
{
	// 座標変換行列リソースを作成する。Matrix4x41つ分のサイズを用意する。
	transformationMatrixResource_ = spriteBase_->GetDxBase()->CreateBufferResource(sizeof(TransformationMatrix));
	// TransformationMatrixResourceにデータを書き込むためのアドレスを取得してTransformationMatrixDataに割り当てる
	transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));

	// 座標変換行列データの初期値を書き込む
	transformationMatrixData_->World = MakeIdentity4x4();
	transformationMatrixData_->WVP = MakeIdentity4x4();

}

void Sprite::AdjustTextureSize()
{
	// テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureIndex_);

	textureSize_.x = static_cast<float>(metadata.width);
	textureSize_.y = static_cast<float>(metadata.height);

	// 画像サイズをテクスチャサイズに合わせる
	size_ = textureSize_;
}
