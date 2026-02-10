#include "Model.h"
#include "ModelBase.h"
//　ファイルやディレクトリに関する操作を行うライブラリ
#include <filesystem>
// ファイルに書いたり読んだりするライブラリ
#include <fstream>
#include "TextureManager.h"
#include "SrvManager.h"

void Model::Initialize(ModelBase* modelBase, const std::string& directoryPath, const std::string& filename)
{
	modelBase_ = modelBase;

	LoadObjFile(directoryPath, filename);

	CreateVertexResource();

	CreateMaterialResource();

	// .objの参照しているテクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
	// 読み込んだテクスチャの番号を取得
	modelData_.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);
	materialTextureIndex_ = modelData_.material.textureIndex;

}

void Model::Draw(const UINT& instanceCount)
{
	if (instanceCount <= 0)
	{
		return;
	}

	instanceCount_ = instanceCount;

	modelBase_->GetDxBase()->GetCommandList()->
		IASetVertexBuffers(0, 1, &vertexBufferView_);	// VBVを設定
	// マテリアルのCBufferの場所を設定
	modelBase_->GetDxBase()->GetCommandList()->
		SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	/*TextureManager::GetInstance()->GetSRVHandleGPU(modelData_.material.textureFilePath);*/

	SrvManager::GetInstance()->SetGraphicsRootDescriptorTable(
		2, modelData_.material.textureIndex);

	// 描画！（DrawCall/ドローコール）。
	modelBase_->GetDxBase()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), instanceCount_, 0, 0);

}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename, const std::string& mtlname)
{
	// 1. 中で必要となる変数の宣言
	MaterialData materialData; // 構築するMaterialData
	std::string line; // ファイルから読んだ1行を格納するもの
	bool isLoad = false; // 引数で指定されたマテリアルの行に来た場合読み込む

	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める

	// 3. 実際にファイルを読み、MaterialDataを構築していく
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "newmtl")
		{
			std::string materialName;
			s >> materialName;

			// 指定されたmtlNameと同じならisLaodをtrueにする
			if (materialName == mtlname)
			{
				isLoad = true;
			} else
			{
				isLoad = false;
			}
		}
		if (isLoad)
		{
			if (identifier == "map_Kd")
			{
				std::string textureFilename;
				s >> textureFilename;
				// 連結してファイルパスにする
				materialData.textureFilePath = directoryPath + "/" + textureFilename;
			} else if (identifier == "Kd")
			{
				s >> materialData.color.x >> materialData.color.y >> materialData.color.z;
				materialData.color.w = 1.0f;
			}
		}
	}

	// 4. MaterialDataを返す



	return materialData;
}

void Model::LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	// 1. 中で必要となる変数の宣言
	ModelData mesh; //メッシュデータ
	std::string materialFilename; // mtllibから取得したmtlファイル名
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals; // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line; // ファイルから読んだ1行を格納するもの

	// 2. ファイルを開く
	std::ifstream file(directoryPath + "/" + filename); // ファイルを開く
	assert(file.is_open()); // とりあえず開けなかったら止める

	// 3. 実際にファイルを読み、ModelDataを構築していく
	while (std::getline(file, line))
	{
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		////if (identifier == "o")
		////{
		////	// これまでのcurrentMeshを保存し、新しいcurrentMeshを初期化する
		////	if (!mesh.vertices.empty() || !mesh.material.textureFilePath.empty())
		////	{
		////		meshes.push_back(mesh);
		////	}
		////	mesh = ModelData(); // 新しいメッシュデータを初期化
		////} else 
		if (identifier == "v")
		{
			Vector4 position;
			s >> position.x >> position.y >> position.z;

			// ===== [テキストより独自で変換したポイント] =======
			// position.x → position.z
			//                                         by ChatGPT
			// ==================================================


			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt")
		{
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;

			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn")
		{
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;

			// ===== [テキストより独自で変換したポイント] =======
			// normal.x → normal.z
			//                                         by ChatGPT
			// ==================================================


			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f")
		{
			VertexData triangle[3];
			// 面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex)
			{
				std::string vertexDefintion;
				s >> vertexDefintion;
				// 頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefintion);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');	// 区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				// 要素へのIndexから、実際の要素の値を取得して頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];

				/*VertexData vertex = { position, texcoord, normal };
				modelData.vertices.push_back(vertex);*/
				triangle[faceVertex] = { position, texcoord, normal };
			}
			// 頂点を逆順で登録することで、周り順を逆にする
			mesh.vertices.push_back(triangle[2]);
			mesh.vertices.push_back(triangle[1]);
			mesh.vertices.push_back(triangle[0]);

		} else if (identifier == "mtllib")
		{
			// materialTemplateLibraryのファイルの名前を取得する

			s >> materialFilename;

		} else if (identifier == "usemtl")
		{
			// usemtlから使用するMaterial名を取得する
			std::string materialName;
			s >> materialName;

			// 基本的にobjファイルと同一階層にmtlファイルは存在させるので、ディレクトリ名とファイル名を渡す
			mesh.material = LoadMaterialTemplateFile(directoryPath, materialFilename, materialName);
		}

	}

	//// 最後に残った currentMesh を押し込む
	//if (!mesh.vertices.empty() || !mesh.material.textureFilePath.empty())
	//{
	//	meshes.push_back(mesh);
	//}

	//// 4. ModelDataを返す

	//if (!mesh.vertices.empty() || !mesh.material.textureFilePath.empty()) {
	//	meshes.push_back(mesh);
	//}

	materialTextureFilePath_ = mesh.material.textureFilePath;

	modelData_ = mesh;
}

void Model::SetTexture(const std::string& directoryFilePath)
{
	modelData_.material.textureFilePath = directoryFilePath;
	// テクスチャファイル読み込み
	TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilePath);
	// 読み込んだテクスチャの番号を取得
	modelData_.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilePath(modelData_.material.textureFilePath);
}

void Model::ResetTexture()
{
	modelData_.material.textureFilePath = materialTextureFilePath_;
	modelData_.material.textureIndex = materialTextureIndex_;
}

void Model::CreateVertexResource()
{
	// VertexResourceを作成する。
	vertexResource_ = modelBase_->GetDxBase()->
		CreateBufferResource(sizeof(VertexData) * modelData_.vertices.size());

	assert(vertexResource_ && "CreateBufferResource failed");

	// VertexBufferViewの設定
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// VertexResourceにデータを書き込むためのアドレスを取得してVertexDataに割り当てる
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	// 頂点データをコピーする
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());
}

void Model::CreateMaterialResource()
{
	// マテリアルリソースを作成する。
	materialResource_ = modelBase_->GetDxBase()->CreateBufferResource(sizeof(Material));
	// MaterialResourceにデータを書き込むためのアドレスを取得してMaterialDataに割り当てる
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	// 色の書き込み
	materialData_->color = modelData_.material.color;
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();
	materialData_->shininess = 100.0f;
}

void Model::CreateSphere()
{
	//isSphere_ = true;

	///// 球の作成

	//const int kSubdivision = 16;
	//int sphereSize = kSubdivision * kSubdivision * 4; // 1つの経度分割で4つの頂点が必要


	//vertexResource_ = 
	//	modelBase_->GetDxBase()->CreateBufferResource(sizeof(VertexData) * sphereSize);

	//
	//// リソースの先頭のアドレスから使う
	//vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//// 使用するリソースのサイズ
	//vertexBufferView_.SizeInBytes = sizeof(VertexData) * kSubdivision * kSubdivision * 4;
	//// 1頂点当たりのサイズ
	//vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//// 書き込むためのアドレスを取得
	//vertexResource_->Map(0, nullptr,
	//	reinterpret_cast<void**>(&vertexData_));

	//// 経度分割1つ分の角度 Φ
	//const float kLonEvery = pi * 2.0f / static_cast<float>(kSubdivision);
	//// 緯度分割1つ分の角度 Θ
	//const float kLatEvery = pi / static_cast<float>(kSubdivision);
	//// 緯度の方向に分割
	//for (int latIndex = 0; latIndex < kSubdivision; ++latIndex)
	//{
	//	float lat = -pi / 2.0f + kLatEvery * latIndex;
	//	// 経度の方向に分割しながら線を描く
	//	for (int lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
	//	{
	//		uint32_t start = (latIndex * kSubdivision + lonIndex) * 4;
	//		float lon = lonIndex * kLonEvery; // Φ
	//		// 頂点にデータを入力する。基準点a
	//		vertexData_[start].position =
	//		{
	//			std::cos(lat) * std::cos(lon),
	//			std::sin(lat),
	//			std::cos(lat) * std::sin(lon),
	//			1.0f,
	//		};
	//		vertexData_[start].texcoord =
	//		{
	//			static_cast<float>(lonIndex) / static_cast<float>(kSubdivision),
	//			1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision),
	//		};
	//		vertexData_[start].normal.x = vertexData_[start].position.x;
	//		vertexData_[start].normal.y = vertexData_[start].position.y;
	//		vertexData_[start].normal.z = vertexData_[start].position.z;

	//		vertexData_[start + 1].position =
	//		{
	//			std::cos(lat + kLatEvery) * std::cos(lon),
	//			std::sin(lat + kLatEvery),
	//			std::cos(lat + kLatEvery) * std::sin(lon),
	//			1.0f,
	//		};
	//		vertexData_[start + 1].texcoord =
	//		{
	//			static_cast<float>(lonIndex) / static_cast<float>(kSubdivision),
	//			1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision),
	//		};
	//		vertexData_[start + 1].normal.x = vertexData_[start + 1].position.x;
	//		vertexData_[start + 1].normal.y = vertexData_[start + 1].position.y;
	//		vertexData_[start + 1].normal.z = vertexData_[start + 1].position.z;

	//		vertexData_[start + 2].position =
	//		{
	//			std::cos(lat) * std::cos(lon + kLonEvery),
	//			std::sin(lat),
	//			std::cos(lat) * std::sin(lon + kLonEvery),
	//			1.0f,
	//		};
	//		vertexData_[start + 2].texcoord =
	//		{
	//			static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision),
	//			1.0f - static_cast<float>(latIndex) / static_cast<float>(kSubdivision),
	//		};
	//		vertexData_[start + 2].normal.x = vertexData_[start + 2].position.x;
	//		vertexData_[start + 2].normal.y = vertexData_[start + 2].position.y;
	//		vertexData_[start + 2].normal.z = vertexData_[start + 2].position.z;



	//		vertexData_[start + 3].position =
	//		{
	//			std::cos(lat + kLatEvery) * std::cos(lon + kLonEvery),
	//			std::sin(lat + kLatEvery),
	//			std::cos(lat + kLatEvery) * std::sin(lon + kLonEvery),
	//			1.0f,
	//		};
	//		vertexData_[start + 3].texcoord =
	//		{
	//			static_cast<float>(lonIndex + 1) / static_cast<float>(kSubdivision),
	//			1.0f - static_cast<float>(latIndex + 1) / static_cast<float>(kSubdivision),
	//		};
	//		vertexData_[start + 3].normal.x = vertexData_[start + 3].position.x;
	//		vertexData_[start + 3].normal.y = vertexData_[start + 3].position.y;
	//		vertexData_[start + 3].normal.z = vertexData_[start + 3].position.z;
	//	}
	//}

	//// Sphere用のindexResourceを作成する。
	//Microsoft::WRL::ComPtr<ID3D12Resource> indexResource = 
	//	modelBase_->GetDxBase()->CreateBufferResource(sizeof(uint32_t) * kSubdivision * kSubdivision * 6);
	//// IndexBufferViewを作成する
	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSphere{};
	//// リソースの先頭のアドレスから使う
	//indexBufferViewSphere.BufferLocation = indexResource->GetGPUVirtualAddress();
	//// 使用するリソースのサイズ
	//indexBufferViewSphere.SizeInBytes = sizeof(uint32_t) * kSubdivision * kSubdivision * 6;
	//// 1つのIndexのサイズ
	//indexBufferViewSphere.Format = DXGI_FORMAT_R32_UINT;
	//// IndexResourceにデータを書き込む
	//uint32_t* indexData = nullptr;
	//// 書き込むためのアドレスを取得
	//indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

	//// Indexは6つの三角形で1つの四角形を表すので、6つずつ書き込む
	//for (int latIndex = 0; latIndex < kSubdivision; ++latIndex)
	//{
	//	for (int lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
	//	{
	//		// Vertexの読み込み開始位置
	//		uint32_t vertexStart = (latIndex * kSubdivision + lonIndex) * 4;
	//		// Indexの書き込み開始位置
	//		uint32_t indexStart = (latIndex * kSubdivision + lonIndex) * 6;

	//		indexData[indexStart + 0] = vertexStart + 0;
	//		indexData[indexStart + 1] = vertexStart + 1;
	//		indexData[indexStart + 2] = vertexStart + 2;
	//		indexData[indexStart + 3] = vertexStart + 1;
	//		indexData[indexStart + 4] = vertexStart + 3;
	//		indexData[indexStart + 5] = vertexStart + 2;
	//	}
	//}



	//// マテリアル用のリソースを作る。
	//Microsoft::WRL::ComPtr<ID3D12Resource> materialResourceSphere =
	//	modelBase_->GetDxBase()->CreateBufferResource(sizeof(Material));
	//// マテリアルにデータを書き込む
	//Material* materialDataSphere = nullptr;
	//// 書き込むためのアドレスを取得
	//materialResourceSphere->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSphere));
	//// 色の書き込み
	//materialDataSphere->color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	//materialDataSphere->enableLighting = true;
	//materialDataSphere->uvTransform = MakeIdentity4x4();

}
