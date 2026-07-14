#include "TitleScene.h"
#include "SceneManager.h"
#include "LightManager.h"
#include "CameraManager.h"

#include "freetype/FreeTypeManager.h"
#include "FontManager.h"
#include "TextManager.h"

void TitleScene::Initialize()
{
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	// Textureを読んで転送する
	textureManager_->LoadTexture("uvChecker.png");
	textureManager_->LoadTexture("monsterBall.png");
	textureManager_->LoadTexture("title.png");

	int textureIndex = 0;
	const char* textureOptions[] = { "Checker", "monsterBall" };

	// シーン初期化始め
	/*camera_->SetRotate({ pi / 3.0f, pi, 0.0f });
	camera_->SetTranslate({ 0.0f, 9.0f, 5.0f });*/
	camera_->SetRotate({ 0.0f, pi, 0.0f });
	camera_->SetTranslate({ 0.0f, 0.0f, 15.0f });


	/*object3dManager_->SetDefaultCamera(camera_.get());
	particleManager_->SetDefaultCamera(camera_.get());*/

	//debugCamera_ = std::make_unique<DebugCamera>();
	//debugCamera_->Initialize();
	//debugCamera_->SetRotate(camera_->GetRotate());
	//debugCamera_->SetTranslate(camera_->GetTranslate());

	cameraManager_->Initialize();
	cameraManager_->AddCamera("Default", camera_.get());
	/*cameraManager_->AddCamera("Debug", debugCamera_.get());*/
	cameraManager_->SetActiveCamera("Default");

	defaultCameraController_ = std::make_unique<DefaultCameraController>();
	defaultCameraController_->Initialize();

	cameraManager_->AddCameraController("Default", defaultCameraController_.get());
	cameraManager_->SetActiveCameraController("Default");

	lightManager_->Initialize();
	lightManager_->SetDirectionalLightColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	lightManager_->SetDirectionalLightDirection({ 0.0f, -1.0f, 0.0f });
	lightManager_->SetDirectionalLightIntensity(1.0f);

	for (size_t i = 0; i < 1; i++)
	{
		std::unique_ptr<Sprite> newSprite = std::make_unique<Sprite>();
		newSprite->Initialize("uvChecker.png");
		newSprite->SetAnchorPoint(Vector2{ 0.5f, 0.5f });
		sprites_.push_back(std::move(newSprite));
	}

	sprites_[0]->SetTexture("title.png");
	sprites_[0]->SetPosition(Vector2{ 100.0f, 100.0f });
	sprites_[0]->AdjustTextureSize();



	modelManager_->LoadModel("", "plane.obj");
	modelManager_->LoadModel("", "axis.obj");
	modelManager_->LoadModel("", "sphere.obj");

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
	particleManager_->CreateParticleGroup("circle", "circle.png");


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

	enterSprite_ = std::make_unique<Sprite>();
	enterSprite_->Initialize("ENTER.png");
	enterSprite_->SetPosition({ 20.0f, 540.0f });


	ModelManager::GetInstance()->LoadModel("", "plane.glTF");

	glTFObject_ = std::make_unique<Object3d>();
	glTFObject_->Initialize();
	glTFObject_->SetModel("plane.glTF");
	glTFObject_->SetEnableLighting(false);

	// シーン初期化終わり

	SoundManager::GetInstance()->SoundLoadFile("", "Alarm01.wav");
	SoundManager::GetInstance()->SoundLoadFile("", "test.mp3");

	FreeTypeManager::GetInstance()->Initialize();
	TextManager::GetInstance()->Initialize();

	titleText_ = std::make_unique<Text>();
	titleText_->InitializeRichText(
		"fonts/x8y12pxTheStrongGamer.ttf",
		32,
		U"TITLE <b><color=#ff4040>SCENE</color></b>\n"
		U"<i><color=#40a0ff>ITALIC</color></i> <u><color=#40ff80>UNDER</color></u> "
		U"<b><i><u><color=#ffd040cc>BOTH</color></u></i></b>");
	titleText_->SetPosition({ 120.0f, 320.0f });

}

void TitleScene::Finalize()
{
	for (auto it = emitters_.begin(); it != emitters_.end(); ++it)
	{
		std::unique_ptr<ParticleEmitter> emitter = std::move(*it);
		emitter.reset();

	}
	emitters_.clear();
	particleManager_->Reset();

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

	camera_->Finalize();
	camera_.reset();

	lightManager_->Finalize();
	cameraManager_->Finalize();

	FreeTypeManager::GetInstance()->Finalize();
	FontManager::GetInstance()->Finalize();
	TextManager::GetInstance()->Finalize();
	//PostEffectManager::GetInstance()->Clear();
}

void TitleScene::Update(const float& deltaTime)
{
	if (inputManager_->TriggerKey(DIK_RETURN))
	{
		// シーン切り替え
		SceneManager::GetInstance()->SetSceneRequest("GAMEPLAY");
		return;
	}

	WorldTransform::AdvanceFrame();

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
	Vector4 directionLightColor = LightManager::GetInstance()->GetDirectionalLightColor();
	Vector3 directionLightDirection = LightManager::GetInstance()->GetDirectionalLightDirection();
	float directionLightIntensity = LightManager::GetInstance()->GetDirectionalLightIntensity();
	if (ImGui::Checkbox("EnableLighting", (bool*)&enableLighting))
	{
		object3ds_[0]->SetEnableLighting(enableLighting);
	}
	if (ImGui::ColorEdit4("LightColor", &directionLightColor.x))
	{
		LightManager::GetInstance()->SetDirectionalLightColor(directionLightColor);
	}
	if (ImGui::DragFloat3("LightDirection", &directionLightDirection.x, 0.01f))
	{
		LightManager::GetInstance()->SetDirectionalLightDirection(Normalize(directionLightDirection));
	}
	if (ImGui::DragFloat("LightIntensity", &directionLightIntensity, 0.01f))
	{
		LightManager::GetInstance()->SetDirectionalLightIntensity(directionLightIntensity);
	}

	Vector3 cameraPos = CameraManager::GetInstance()->GetCameraWorldPosition();
	ImGui::InputFloat3("CameraData", &cameraPos.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

	ImGui::End();

	ImGui::Begin("glTFObject Setting");

	Vector3 glTFTanslate = glTFObject_->GetTranslate();
	if (ImGui::DragFloat3("translate", &glTFTanslate.x, 0.1f))
	{
		glTFObject_->SetTranslate(glTFTanslate);
	}

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

	if (isDebugCamera_)
	{
		/*cameraManager_->SetActiveCamera("Debug");*/
	} 
	else
	{
		cameraManager_->SetActiveCamera("Default");
	}

	cameraManager_->Update(deltaTime);



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
		emitter->Update(deltaTime);
	}

	glTFObject_->Update();

	enterSprite_->Update();

	particleManager_->Update(deltaTime);

	//// ImGuiの内部コマンドを生成する
	//ImGui::Render();

	titleText_->Update();
}

void TitleScene::FinishFadeIn()
{
	
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

	glTFObject_->Draw();

	/*particleManager_->Draw();*/

	/*for (size_t i = 0; i < particle3ds.size(); i++)
	{
		particle3ds[i]->Draw();
	}*/


	if (isDrawSprite_)
	{
		for (size_t i = 0; i < sprites_.size(); i++)
		{
			sprites_[i]->Draw();
		}
	}

	enterSprite_->Draw();

	if (inputManager_->TriggerKey(DIK_0))
	{
		SoundManager::GetInstance()->SoundPlayWave("Alarm01.wav");
	}
	if (inputManager_->TriggerKey(DIK_1))
	{
		SoundManager::GetInstance()->SoundPlayWave("test.mp3");

	}

	titleText_->Draw();

	cameraManager_->DrawDebugUI();

#ifdef _DEBUG
	debugManager_->DrawAll(cameraManager_->GetMainCamera()->GetViewProjectionMatrix());
#endif

}
