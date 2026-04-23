#include "Terrain.h"
#include "ModelManager.h"
#include "ImGuiManager.h"

void Terrain::Initialize()
{
	ModelManager::GetInstance()->LoadModel("terrain.obj");

	object3d_ = std::make_unique<Object3d>();
	object3d_->Initialize();
	object3d_->SetModel("terrain.obj");

	transform_.scale = { 1.0f, 1.0f, 1.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.translate = { 0.0f, 0.0f, 0.0f };
}

void Terrain::Update()
{
	object3d_->SetTransform(transform_);
	object3d_->Update();

	ImGui::Begin("TerrainSetting");
	bool isDraw = isDraw_;
	if (ImGui::Checkbox("DrawTerrain", &isDraw))
	{
		isDraw_ = isDraw;
	}
	ImGui::End();
}

void Terrain::Draw()
{
	if (isDraw_)
	{
		object3d_->Draw();
	}
}

void Terrain::Finalize()
{

}
