#include "TitleScene.h"
#include "SceneManager.h"

void TitleScene::Initialize()
{


	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	// Textureを読んで転送する
	textureManager_->LoadTexture("resources/uvChecker.png");
	textureManager_->LoadTexture("resources/monsterBall.png");
	textureManager_->LoadTexture("resources/title.png");



	int textureIndex = 0;
	const char* textureOptions[] = { "Checker", "monsterBall" };

	// シーン初期化始め
	/*camera_->SetRotate({ pi / 3.0f, pi, 0.0f });
	camera_->SetTranslate({ 0.0f, 9.0f, 5.0f });*/

	camera_->SetRotate({ 0.0f, pi, 0.0f });
	camera_->SetTranslate({ 0.0f, 0.0f, 15.0f });
	object3dManager_->SetDefaultCamera(camera_.get());
	particleManager_->SetDefaultCamera(camera_.get());



	for (size_t i = 0; i < 1; i++)
	{
		std::unique_ptr<Sprite> newSprite = std::make_unique<Sprite>();
		newSprite->Initialize("resources/uvChecker.png");
		newSprite->SetAnchorPoint(Vector2{ 0.5f, 0.5f });
		sprites_.push_back(std::move(newSprite));
	}

	sprites_[0]->SetTexture("resources/title.png");
	sprites_[0]->SetPosition(Vector2{ 100.0f, 100.0f });
	sprites_[0]->AdjustTextureSize();



	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	ModelManager::GetInstance()->LoadModel("sphere.obj");

	for (size_t i = 0; i < 1; i++)
	{
		std::unique_ptr<Object3d> newObject3d = std::make_unique<Object3d>();
		newObject3d->Initialize();
		newObject3d->SetModel("plane.obj");
		object3ds_.push_back(std::move(newObject3d));
	}

	object3ds_[0]
		->SetModel("sphere.obj");


	particleManager_->SetModel("plane.obj");
	particleManager_->CreateParticleGroup("circle", "resources/circle.png");


	AABB  aabb;
	aabb.max = { 1.0f, 1.0f, 1.0f };
	aabb.min = { -1.0f, -1.0f, -1.0f };
	particleManager_->CreateAccelerationField({ 5.0f, 0.0f, 0.0f }, aabb);


	for (size_t i = 0; i < 1; i++)
	{
		std::unique_ptr<ParticleEmitter> emitter = std::make_unique<ParticleEmitter>();
		Transform transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		emitter->Initialize("circle", transform, 3, 0.2f);
		emitters_.push_back(std::move(emitter));
	}

	// シーン初期化終わり

	SoundManager::GetInstance()->SoundLoadFile("Resources/Alarm01.wav");
	SoundManager::GetInstance()->SoundLoadFile("Resources/test.mp3");

}

void TitleScene::Finalize()
{
	for (auto it = emitters_.begin(); it != emitters_.end(); ++it)
	{
		std::unique_ptr<ParticleEmitter> emitter = std::move(*it);
		emitter.reset();

	}
	emitters_.clear();

	for (auto it = object3ds_.begin(); it != object3ds_.end(); ++it)
	{
		std::unique_ptr<Object3d> object3d = std::move(*it);
		object3d.reset();
	}
	object3ds_.clear();

	for (auto it = sprites_.begin(); it != sprites_.end(); ++it)
	{
		std::unique_ptr<Sprite> sprite = std::move(*it);
		sprite.reset();
	}
	sprites_.clear();


	camera_->Finalize();
	camera_.reset();
}

void TitleScene::Update()
{
	if (inputManager_->TriggerKey(DIK_RETURN))
	{
		// シーン切り替え
		SceneManager::GetInstance()->SetSceneRequest("GAMEPLAY");
		return;
	}

#ifdef USE_IMGUI

	// デモウィンドウ表示
	ImGui::ShowDemoWindow();

	ImGui::Begin("Setting");
	ImGui::SetWindowSize("Sprite Setting", { 500.0f, 100.0f });
	Vector2 spritePos = sprites_[0]->GetPosition();
	if (ImGui::DragFloat2("pos", &spritePos.x, 1.0f, 0.0f, 0.0f, "%05.1f"))
	{
		sprites_[0]->SetPosition(spritePos);
	}
	ImGui::Checkbox("DrawSprite", &isDrawSprite_);
	ImGui::Checkbox("DrawObject3d", &isDrawObject3d_);
	ImGui::Checkbox("DebugCamera", &isDebugCamera_);
	ImGui::End();


	ImGui::Begin("CameraSetting");
	Vector3 cameraRotate = camera_->GetRotate();
	Vector3 cameraTranslate = camera_->GetTranslate();
	if (ImGui::DragFloat3("CameraRotate", &cameraRotate.x, (1.0f / 180.0f) * pi))
	{
		camera_->SetRotate(cameraRotate);
	}
	if (ImGui::DragFloat3("CameraTranslate", &cameraTranslate.x, 0.1f))
	{
		camera_->SetTranslate(cameraTranslate);
	}

	ImGui::End();

	ImGui::Begin("LightSetting");

	int32_t enableLighting = object3ds_[0]->GetEnableLighting();
	Vector4 directionLightColor = object3ds_[0]->GetLightColor();
	Vector3 directionLightDirection = object3ds_[0]->GetLightDirection();
	float directionLightIntensity = object3ds_[0]->GetLightIntensity();
	if (ImGui::Checkbox("EnableLighting", (bool*)&enableLighting))
	{
		object3ds_[0]->SetEnableLighting(enableLighting);
	}
	if (ImGui::ColorEdit4("LightColor", &directionLightColor.x))
	{
		object3ds_[0]->SetLightColor(directionLightColor);
	}
	if (ImGui::DragFloat3("LightDirection", &directionLightDirection.x, 0.01f))
	{
		object3ds_[0]->SetLightDirection(Normalize(directionLightDirection));
	}
	if (ImGui::DragFloat("LightIntensity", &directionLightIntensity, 0.01f))
	{
		object3ds_[0]->SetLightIntensity(directionLightIntensity);
	}

	Vector3 cameraPos = object3ds_[0]->GetCameraWorldPosition();
	ImGui::InputFloat3("CameraData", &cameraPos.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

	ImGui::End();

#endif



	/*

	directionalLightData->direction = Vector3{ directionLightDirection.x, directionLightDirection.y, directionLightDirection.z };

	*/


	/*worldMatrixSphere = MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
	wvpMatrixSphere = Multiply(worldMatrixSphere, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixDataSphere->World = worldMatrixSphere;
	transformationMatrixDataSphere->WVP = wvpMatrixSphere;*/

	mousePosition_.x = static_cast<float>(inputManager_->MousePoint().x);
	mousePosition_.y = static_cast<float>(inputManager_->MousePoint().y);

	camera_->Update();
	if (isDebugCamera_)
	{
		debugCamera_.Update(inputManager_, camera_->GetTransform());
		camera_->SetViewMatrix(debugCamera_.GetViewMatrix());
	} else
	{
		camera_->TransformView();
	}
	camera_->Transformation();




	for (auto it = sprites_.begin(); it != sprites_.end(); ++it)
	{
		Sprite* sprite = it->get();
		sprite->Update();
	}

	for (auto it = object3ds_.begin(); it != object3ds_.end(); ++it)
	{
		Object3d* object3d = it->get();
		object3d->Update();
	}

	for (auto it = emitters_.begin(); it != emitters_.end(); ++it)
	{
		ParticleEmitter* emitter = it->get();
		emitter->Update();
	}

	particleManager_->Update();

	//// ImGuiの内部コマンドを生成する
	//ImGui::Render();


}

void TitleScene::Draw()
{

	if (isDrawObject3d_)
	{
		object3dManager_->DrawingCommon();

		for (size_t i = 0; i < object3ds_.size(); i++)
		{
			object3ds_[i]->Draw();
		}
	}


	/*particleManager_->Draw();*/

	/*for (size_t i = 0; i < particle3ds.size(); i++)
	{
		particle3ds[i]->Draw();
	}*/


	if (isDrawSprite_)
	{
		spriteManager_->DrawingCommon();

		for (size_t i = 0; i < sprites_.size(); i++)
		{
			sprites_[i]->Draw();
		}
	}



	if (inputManager_->TriggerKey(DIK_0))
	{
		SoundManager::GetInstance()->SoundPlayWave("Resources/Alarm01.wav");
	}
	if (inputManager_->TriggerKey(DIK_1))
	{
		SoundManager::GetInstance()->SoundPlayWave("Resources/test.mp3");

	}
}