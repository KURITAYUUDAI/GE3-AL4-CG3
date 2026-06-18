#include "GamePlayScene.h"
#include "SceneManager.h"
#include "LightManager.h"
#include "CameraManager.h"
#include "PrimitiveManager.h"
#include "PostEffectManager.h"
#include "GaussianBlur.h"
#include "BulletManager.h"
#include "SplineCurve.h"

void GamePlayScene::Initialize()
{
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();

	// Textureを読んで転送する
	textureManager_->LoadTexture("resources/uvChecker.png");
	textureManager_->LoadTexture("resources/monsterBall.png");
	textureManager_->LoadTexture("resources/rostock_laage_airport_4k.dds");

	int textureIndex = 0;
	const char* textureOptions[] = { "Checker", "monsterBall" };

	// シーン初期化始め
	/*camera_->SetRotate({ pi / 3.0f, pi, 0.0f });
	camera_->SetTranslate({ 0.0f, 9.0f, 5.0f });*/

	camera_->SetRotate({ 0.0f, 0.0f, 0.0f });
	camera_->SetTranslate({ 0.0f, 0.0f, -15.0f });

	/*debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	debugCamera_->SetRotate(camera_->GetRotate());
	debugCamera_->SetTranslate(camera_->GetTranslate());*/

	cameraManager_->Initialize();
	cameraManager_->AddCamera("Default", camera_.get());
	/*cameraManager_->AddCamera("Debug", debugCamera_.get());*/
	cameraManager_->SetActiveCamera("Default");

	defaultCameraController_ = std::make_unique<DefaultCameraController>();
	defaultCameraController_->Initialize();

	railCameraController_ = std::make_unique<RailCameraController>();
	railCameraController_->Initialize();

	cameraManager_->AddCameraController("Default", defaultCameraController_.get());
	cameraManager_->AddCameraController("Rail", railCameraController_.get());
	cameraManager_->SetActiveCameraController("Default");

	lightManager_->Initialize();
	lightManager_->SetDirectionalLightColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	lightManager_->SetDirectionalLightDirection({ 0.0f, -1.0f, 0.0f });
	lightManager_->SetDirectionalLightIntensity(1.0f);

	lightManager_->SetNumPointLights(1);
	lightManager_->SetPointLightColor(0, { 1.0f, 0.0f, 0.0f, 1.0f });
	lightManager_->SetPointLightPosition(0, { 0.0f, 1.0f, 0.0f });
	lightManager_->SetPointLightIntensity(0, 1.0f);
	lightManager_->SetPointLightRadius(0, 5.0f);
	lightManager_->SetPointLightDecay(0, 1.0f);

	lightManager_->SetNumSpotLights(1);
	lightManager_->SetSpotLightColor(0, { 0.0f, 1.0f, 0.0f, 1.0f });
	lightManager_->SetSpotLightPosition(0, { 2.0f, 1.25f, 0.0f });
	lightManager_->SetSpotLightIntensity(0, 4.0f);
	lightManager_->SetSpotLightDirection(0, { -1.0f, -1.0f, 0.0f });
	lightManager_->SetSpotLightDistance(0, 7.0f);
	lightManager_->SetSpotLightDecay(0, 1.0f);
	lightManager_->SetSpotLightCosAngle(0, std::cosf(pi / 3.0f));
	lightManager_->SetSpotLightCosFalloff(0, std::cosf(pi / 4.0f));

	for (size_t i = 0; i < 1; i++)
	{
		std::unique_ptr<Sprite> newSprite = std::make_unique<Sprite>();
		newSprite->Initialize("resources/uvChecker.png");
		newSprite->SetAnchorPoint(Vector2{ 0.5f, 0.5f });
		sprites_.push_back(std::move(newSprite));
	}

	sprites_[0]->SetTexture("resources/monsterBall.png");
	sprites_[0]->SetPosition(Vector2{ 100.0f, 100.0f });
	sprites_[0]->AdjustTextureSize();

	modelManager_->LoadModel("plane.obj");
	modelManager_->LoadModel("axis.obj");
	modelManager_->LoadModel("sphere.obj");
	modelManager_->LoadModel("multiMaterial.obj");

	for (size_t i = 0; i < 1; i++)
	{
		std::unique_ptr<Object3d> newObject3d = std::make_unique<Object3d>();
		newObject3d->Initialize();
		newObject3d->SetModel("plane.obj");
		object3ds_.push_back(std::move(newObject3d));
	}

	object3ds_[0]->SetModel("sphere.obj");

	/*particleManager_->CreateParticleGroup("circle", "resources/circle.png");
	particleManager_->SetModel("circle", "plane.obj");
	particleManager_->SetIsMoveAccelerationField("circle", true);*/
	particleManager_->CreateParticleGroup("slash", "resources/circle2.png");
	particleManager_->SetModel("slash", "plane.obj");
	particleManager_->SetIsMoveAccelerationField("slash", false);

	PrimitiveManager::GetInstance()->Initialize();
	PrimitiveManager::RingConfig ringConfig;
	ringConfig.segments = 32;
	ringConfig.innerRadius = 0.0f;
	ringConfig.outerRadius = 1.5f;
	ringConfig.innerColor = {0.0f, 0.0f, 1.0f, 1.0f};
	ringConfig.outerColor = {0.0f, 0.5f, 0.0f, 1.0f};
	ringConfig.uvScaleU = 2.0f;
	ringConfig.uvScaleV = 0.1f;
	ringConfig.startAngle = 1.0f / 5.0f * pi;
	ringConfig.endAngle = 9.0f / 5.0f * pi;
	ringConfig.radiusPoints = 
	{
		{ 0.0f, 0.5f, 0.5f },   // 開始：inner=0.2, outer=1.0
		{ 0.5f, 0.5f, 1.0f },   // 中間：inner=0.1, outer=0.6
		{ 1.0f, 0.5f, 0.5f },   // 終了：inner=0.2, outer=0.2
	};
	ringConfig.alphaFade.startFadeRange = 0.15f;
	ringConfig.alphaFade.endFadeRange = 0.15f;
	PrimitiveManager::GetInstance()->CreateRing("ring_primitive", ringConfig);
	
	particleManager_->CreateParticleGroup("ring", "resources/gradationLine.png");
	particleManager_->SetModel("ring", "ring_primitive");
	particleManager_->SetIsMoveAccelerationField("ring", false);
	particleManager_->SetIsBillboard("ring", false);

	PrimitiveManager::CylinderConfig cylinderConfig;
	cylinderConfig.segments = 32;
	cylinderConfig.stacks = 32;
	cylinderConfig.height = 3.0f;
	cylinderConfig.topRadiusX = 1.0f;
	cylinderConfig.topRadiusZ = 1.0f;
	cylinderConfig.bottomRadiusX = 1.0f;
	cylinderConfig.bottomRadiusZ = 1.0f;
	cylinderConfig.radiusPoints =
	{
		{ 0.0f, 1.0f, 1.0f, false },  // ここから折れ線
		{ 0.3f, 2.0f, 2.0f, true  },  // ここからなめらか
		{ 0.7f, 2.0f, 2.0f, false },  // ここから折れ線
		{ 1.0f, 1.0f, 1.0f },
	};
	cylinderConfig.topColor = { 0.0f, 0.0f, 1.0f, 1.0f };
	cylinderConfig.bottomColor = { 0.0f, 0.5f, 0.0f, 1.0f };
	cylinderConfig.uvScaleU = 2.0f;
	cylinderConfig.uvScaleV = 1.0f;
	cylinderConfig.startAngle = 0.0f;
	cylinderConfig.endAngle = 2.0f * pi;
	cylinderConfig.alphaFade.startFadeRange = 0.15f;
	cylinderConfig.alphaFade.endFadeRange = 0.15f;
	PrimitiveManager::GetInstance()->CreateCylinder("cylinder_primitive", cylinderConfig);

	particleManager_->CreateParticleGroup("cylinder", "resources/uvChecker.png");
	particleManager_->SetModel("cylinder", "cylinder_primitive");
	particleManager_->SetIsMoveAccelerationField("cylinder", false);
	particleManager_->SetIsBillboard("cylinder", false);

	AABB  aabb;
	aabb.max = { 1.0f, 1.0f, 1.0f };
	aabb.min = { -1.0f, -1.0f, -1.0f };
	particleManager_->CreateAccelerationField({ 5.0f, 0.0f, 0.0f }, aabb);


	/*for (size_t i = 0; i < 1; i++)
	{
		std::unique_ptr<ParticleEmitter> emitter = std::make_unique<ParticleEmitter>();
		Transform transform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
		emitter->Initialize("circle", transform, 3, 0.2f);
		emitters_.push_back(std::move(emitter));
	}*/

	skyBox_ = new SkyBox();
	skyBox_->Initialize();
	skyBox_->SetCamera(camera_.get());

	terrain_ = std::make_unique<Terrain>();
	terrain_->Initialize();

	player_ = std::make_unique<Player>();
	player_->Initialize();
	player_->SetEnvironmentTextureIndex(skyBox_->GetEnvironmentTextureIndex());
	player_->SetParent(cameraManager_->GetActiveCameraController()->GetWorldTransform());

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();
	enemy_->SetEnvironmentTextureIndex(skyBox_->GetEnvironmentTextureIndex());
	enemy_->SetParent(cameraManager_->GetActiveCameraController()->GetWorldTransform());

	BulletManager::GetInstance()->Initialize();

	slashEmitter = std::make_unique<ParticleEmitter>();
	slashEmitter->Initialize("slash", 
		{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }, 3, 0.2f);

	ringEmitter = std::make_unique<ParticleEmitter>();
	ringEmitter->Initialize("ring", 
		{ {1.0f, 2.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }, 1, 0.2f);

	cylinderEmitter = std::make_unique<ParticleEmitter>();
	cylinderEmitter->Initialize("cylinder",
		{ {1.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} }, 1, 0.2f);

	PostEffectManager::GetInstance()->Clear();
	//PostEffectManager::GetInstance()->Add("Outline");
	//PostEffectManager::GetInstance()->Add("GaussianBlur");
	PostEffectManager::GetInstance()->Add("Bloom");

	debugManager_->Initialize();

	// シーン初期化終わり

	soundManager_->SoundLoadFile("Resources/Alarm01.wav");
	soundManager_->SoundLoadFile("Resources/test.mp3");

	

}

void GamePlayScene::Finalize()
{
	enemy_->Finalize();
	player_->Finalize();
	
	debugManager_->Finalize();

	PostEffectManager::GetInstance()->Clear();

	/*for (auto it = emitters_.begin(); it != emitters_.end(); ++it)
	{
		std::unique_ptr<ParticleEmitter> emitter = std::move(*it);
		emitter.reset();

	}*/
	emitters_.clear();

	particleManager_->Reset();

	BulletManager::GetInstance()->Finalize();

	/*for (auto it = object3ds_.begin(); it != object3ds_.end(); ++it)
	{
		std::unique_ptr<Object3d> object3d = std::move(*it);
		object3d.reset();
	}*/
	object3ds_.clear();

	skyBox_->Finalize();
	delete skyBox_;

	/*for (auto it = sprites_.begin(); it != sprites_.end(); ++it)
	{
		std::unique_ptr<Sprite> sprite = std::move(*it);
		sprite.reset();
	}*/
	sprites_.clear();


	camera_->Finalize();
	camera_.reset();

	lightManager_->Finalize();
	cameraManager_->Finalize();
}

void GamePlayScene::Update(const float& deltaTime)
{
	if (inputManager_->TriggerKey(DIK_RETURN))
	{
		// シーン切り替え
		SceneManager::GetInstance()->SetSceneRequest("TITLE");
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
	ImGui::End();


	/*ImGui::Begin("GaussianBlur");
	auto* blur = PostEffectManager::GetInstance()->Get<GaussianBlur>("GaussianBlur");
	float sigma = blur->GetSigma();
	int kernelRadius = blur->GetKernelRadius();
	if (ImGui::SliderFloat("Sigma", &sigma, 0.1f, 20.0f))
	{
		blur->SetSigma(sigma);
	}
	if (ImGui::SliderInt("KernelRadius", &kernelRadius, 1, 64))
	{
		blur->SetKernelRadius(kernelRadius);
	}

	ImGui::End();*/

	//ImGui_ImplDX12_NewFrame();
	//ImGui_ImplWin32_NewFrame();
	//ImGui::NewFrame();

	//// ゲームの処理

	////// 開発用ImGuiの処理。実際に開発用のUIを出す場合はここをゲーム固有の処理に置き換えること。
	////ImGui::ShowDemoWindow();

	//ImGui::Begin("SpriteSetting");

	//ImGui::Checkbox("DrawSprite", &isDrawSprite);

	//if (ImGui::BeginTable("ItemsTable", 7, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
	//{
	//	ImGui::TableSetupScrollFreeze(1, 1);
	//	ImGui::TableSetupColumn("position", ImGuiTableColumnFlags_WidthFixed, 80.0f);
	//	ImGui::TableSetupColumn("rotation", ImGuiTableColumnFlags_WidthFixed, 80.0f);
	//	ImGui::TableSetupColumn("size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
	//	ImGui::TableSetupColumn("color", ImGuiTableColumnFlags_WidthFixed, 180.0f);
	//	ImGui::TableSetupColumn("uvTransform", ImGuiTableColumnFlags_WidthFixed, 160.0f);
	//	ImGui::TableSetupColumn("texture", ImGuiTableColumnFlags_WidthFixed, 80.0f);
	//	ImGui::TableSetupColumn("Ops", ImGuiTableColumnFlags_WidthFixed, 50.0f);
	//	ImGui::TableHeadersRow();

	//	int to_delete = -1;
	//	size_t i = 0;

	//	for (auto it = sprites.begin(); it != sprites.end(); ++it, ++i)
	//	{
	//		Sprite* sprite = *it;
	//		if (!sprite) continue;
	//	
	//		ImGui::TableNextRow();

	//		// 要素ごとにIDスタックを分ける
	//		ImGui::PushID((void*)sprite);
	//		
	//		ImGui::TableSetColumnIndex(0);
	//		Vector2 positionSprite = sprite->GetPosition();
	//		if (ImGui::DragFloat2("##positionSprite", &positionSprite.x, 1.0f, 0.0f, 1280.0f))
	//		{
	//			sprite->SetPosition(positionSprite);
	//		}

	//		ImGui::TableSetColumnIndex(1);
	//		float rotationSprite = sprite->GetRotation();
	//		if (ImGui::SliderAngle("##rotationSprite", &rotationSprite))
	//		{
	//			sprite->SetRotation(rotationSprite);
	//		}

	//		ImGui::TableSetColumnIndex(2);
	//		Vector2 sizeSprite = sprite->GetSize();
	//		if (ImGui::DragFloat2("##sizeSprite", &sizeSprite.x, 1.0f, 0.0f, 1280.0f))
	//		{
	//			sprite->SetSize(sizeSprite);
	//		}

	//		ImGui::TableSetColumnIndex(3);
	//		Vector4 colorSprite = sprite->GetColor();
	//		if (ImGui::ColorEdit4("##colorSprite", &colorSprite.x))
	//		{
	//			sprite->SetColor(colorSprite);
	//		}

	//		ImGui::TableSetColumnIndex(4);
	//		Transform uvTransformSprite = sprite->GetUVTransform();
	//		if (ImGui::DragFloat2("##UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f))
	//		{
	//			sprite->SetUVScale(uvTransformSprite.scale);
	//		}
	//		if (ImGui::DragFloat2("##UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f))
	//		{
	//			sprite->SetUVTranslate(uvTransformSprite.translate);
	//		}
	//		if (ImGui::SliderAngle("##UVRotate", &uvTransformSprite.rotate.z))
	//		{
	//			sprite->SetUVRotate(uvTransformSprite.rotate);
	//		}
	//		Vector2 textureLeftTopSprite = sprite->GetTextureLeftTop();
	//		if(ImGui::DragFloat2("##LeftTop", &textureLeftTopSprite.x))
	//		{
	//			sprite->SetTextureLeftTop(textureLeftTopSprite);
	//		}
	//		Vector2 textureSizeSprite = sprite->GetTextureSize();
	//		if (ImGui::DragFloat2("##Size", &textureSizeSprite.x))
	//		{
	//			sprite->SetTextureSize(textureSizeSprite);
	//		}

	//		ImGui::TableSetColumnIndex(5);
	//		if (ImGui::SmallButton("UV"))
	//		{
	//			sprite->SetTexture("resources/uvChecker.png");
	//		}
	//		if (ImGui::SmallButton("MB"))
	//		{
	//			sprite->SetTexture("resources/monsterBall.png");
	//		}
	//		bool isFlipXSprite = sprite->GetIsFlipX();
	//		if (ImGui::Checkbox("FlipX", &isFlipXSprite))
	//		{
	//			sprite->SetFlipX(isFlipXSprite);
	//		}
	//		bool isFlipYSprite = sprite->GetIsFlipY();
	//		if (ImGui::Checkbox("FlipY", &isFlipYSprite))
	//		{
	//			sprite->SetFlipY(isFlipYSprite);
	//		}


	//		ImGui::TableSetColumnIndex(6);
	//		if (ImGui::SmallButton("Delete##del")) 
	//		{
	//			to_delete = int(i);
	//		}

	//		ImGui::PopID();
	//	}

	//	ImGui::EndTable();

	//	if (to_delete >= 0 && to_delete < (int) sprites.size()) 
	//	{
	//		delete sprites[to_delete];
	//		sprites.erase(sprites.begin() + to_delete);
	//	}

	//	if (ImGui::SmallButton("Add##del"))
	//	{
	//		Sprite* sprite = new Sprite();
	//		sprite->Initialize(spriteBase, "resources/uvChecker.png");
	//		sprites.push_back(sprite);
	//	}
	//}

	//ImGui::End();
	ImGui::Begin("LightSetting");

	int32_t enableLighting = object3ds_[0]->GetEnableLighting();
	if (ImGui::Checkbox("EnableLighting", (bool*)&enableLighting))
	{
		object3ds_[0]->SetEnableLighting(enableLighting);
	}

	Vector3 cameraPos = cameraManager_->GetCameraWorldPosition();
	ImGui::InputFloat3("CameraData", &cameraPos.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

	ImGui::End();

	ImGui::Begin("CheckFPS");

	// FPS表示
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	ImGui::End();

	//if (ImGui::BeginTable("ItemsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
	//{
	//	ImGui::TableSetupScrollFreeze(1, 1);
	//	ImGui::TableSetupColumn("transform", ImGuiTableColumnFlags_WidthFixed, 180.0f);
	//	/*ImGui::TableSetupColumn("color", ImGuiTableColumnFlags_WidthFixed, 180.0f);*/
	//	ImGui::TableSetupColumn("model", ImGuiTableColumnFlags_WidthFixed, 80.0f);
	//	ImGui::TableSetupColumn("Ops", ImGuiTableColumnFlags_WidthFixed, 50.0f);
	//	ImGui::TableHeadersRow();

	//	int to_delete = -1;
	//	size_t i = 0;

	//	for (auto it = object3ds.begin(); it != object3ds.end(); ++it, ++i)
	//	{
	//		Object3d* object3d = *it;
	//		if (!object3d) continue;

	//		ImGui::TableNextRow();

	//		// 要素ごとにIDスタックを分ける
	//		ImGui::PushID((void*)object3d);

	//		ImGui::TableSetColumnIndex(0);
	//		Vector3 scaleModel = object3d->GetScale();
	//		if (ImGui::DragFloat3("##scaleModel", &scaleModel.x, 0.01f, 0.0f, 10.0f))
	//		{
	//			object3d->SetScale(scaleModel);
	//		}
	//		Vector3 rotateModel = object3d->GetRotate();
	//		if (ImGui::DragFloat3("##rotateModel", &rotateModel.x, (1.0f / 180.0f) * pi, -2.0f * pi, 2.0f * pi))
	//		{
	//			object3d->SetRotate(rotateModel);
	//		}
	//		Vector3 translateModel = object3d->GetTranslate();
	//		if (ImGui::DragFloat3("##positionModel", &translateModel.x, 0.01f, 0.0f, 1280.0f))
	//		{
	//			object3d->SetTranslate(translateModel);
	//		}

	//		/*ImGui::TableSetColumnIndex(1);
	//		Vector4 colorModel = object3d->GetColor();
	//		if (ImGui::ColorEdit4("##colorModel", &colorModel.x))
	//		{
	//			sprite->SetColor(colorModel);
	//		}*/

	//		ImGui::TableSetColumnIndex(1);
	//		if (ImGui::SmallButton("plane"))
	//		{
	//			object3d->SetModel("plane.obj");
	//		}
	//		if (ImGui::SmallButton("axis"))
	//		{
	//			object3d->SetModel("axis.obj");
	//		}


	//		ImGui::TableSetColumnIndex(2);
	//		if (ImGui::SmallButton("Delete##del"))
	//		{
	//			to_delete = int(i);
	//		}

	//		ImGui::PopID();
	//	}

	//	ImGui::EndTable();

	//	if (to_delete >= 0 && to_delete < (int)object3ds.size())
	//	{
	//		delete object3ds[to_delete];
	//		object3ds.erase(object3ds.begin() + to_delete);
	//	}

	//	if (ImGui::SmallButton("Add##del"))
	//	{
	//		Object3d* newObject3d = new Object3d();
	//		newObject3d->Initialize(object3dBase);
	//		newObject3d->SetModel("plane.obj");
	//		object3ds.push_back(newObject3d);
	//	}
	//}

	//ImGui::End();




	//ImGui::Begin("Window");

	//ImGui::Checkbox("DebugCamera", &isDebugCamera);

	//ImGui::Combo("Texture", &textureIndex, textureOptions, IM_ARRAYSIZE(textureOptions));




	//for (size_t i = 0; i < modelData.size(); ++i)
	//{
	//	std::string label = "##Color" + std::to_string(i);
	//	if (ImGui::ColorEdit4(label.c_str(), materialColor[i].data()))
	//	{
	//		// materialColorを上書きする
	//		materialData[i]->color = Vector4(materialColor[i][0], materialColor[i][1], materialColor[i][2], materialColor[i][3]);
	//		// （＝GPU 側で使われるマテリアル色を更新）
	//	}
	//}

	//ImGui::DragFloat3("DirectionalLightDirection", &directionLightDirection.x, 0.01f);


	//ImGui::DragFloat("DirectionLightIntensity", &directionalLightData->intensity, 0.01f);

	//if (ImGui::ColorEdit4("DirectionalLightColor", directionalLightColor))
	//{
	//	// directionalLightColorを上書きする
	//	directionalLightData->color = Vector4(directionalLightColor[0], directionalLightColor[1], directionalLightColor[2], directionalLightColor[3]);
	//	// （＝GPU 側で使われるマテリアル色を更新）
	//}

	//Transform cameraTransform = 
	//	Transform{ camera->GetScale(), camera->GetRotate(), camera->GetTranslate() };
	//if (ImGui::DragFloat3("CameraScale", &cameraTransform.scale.x, 0.01f))
	//{
	//	camera->SetScale(cameraTransform.scale);
	//}
	//if (ImGui::DragFloat3("CameraRotate", &cameraTransform.rotate.x, 1.0f / 180.0f * pi))
	//{
	//	camera->SetRotate(cameraTransform.rotate);
	//}
	//if (ImGui::DragFloat3("CameraTranslate", &cameraTransform.translate.x, 0.01f))
	//{
	//	camera->SetTranslate(cameraTransform.translate);
	//}

	//ImGui::Text("MouseX: %.2f, MouseY: %.2f", mousePosition.x, mousePosition.y);

	//ImGui::Text("Press 0 to play the sound");

	//ImGui::End();

#endif



	/*

	directionalLightData->direction = Vector3{ directionLightDirection.x, directionLightDirection.y, directionLightDirection.z };

	*/


	/*worldMatrixSphere = MakeAffineMatrix(transformSphere.scale, transformSphere.rotate, transformSphere.translate);
	wvpMatrixSphere = Multiply(worldMatrixSphere, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixDataSphere->World = worldMatrixSphere;
	transformationMatrixDataSphere->WVP = wvpMatrixSphere;*/

	lightManager_->Update();

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
		//emitter->Update(deltaTime);
	}

	particleManager_->Update(deltaTime);

	skyBox_->Update();

	terrain_->Update();

	player_->Update(deltaTime);

	enemy_->Update(deltaTime);

	BulletManager::GetInstance()->Update(deltaTime);

	if (inputManager_->TriggerKey(DIK_0))
	{
		slashEmitter->EmitSlash();
	}

	if (inputManager_->TriggerKey(DIK_1))
	{
		ringEmitter->Emit();
	}

	if (inputManager_->TriggerKey(DIK_2))
	{
		cylinderEmitter->Emit();
	}


	collisionManager_->Clear();
	collisionManager_->AddCollider(player_->GetCollider());
	collisionManager_->AddCollider(enemy_->GetCollider());
	for (auto& bullet : BulletManager::GetInstance()->GetBullets())
	{
		collisionManager_->AddCollider(bullet->GetCollider());
	}
	collisionManager_->CheckAllCollisions();

	//// ImGuiの内部コマンドを生成する
	//ImGui::Render();

	if (enemy_->GetIsDead())
	{
		SceneManager::GetInstance()->SetSceneRequest("TITLE");
	}

	if (player_->GetIsDead())
	{
		SceneManager::GetInstance()->SetSceneRequest("TITLE");
	}
}

void GamePlayScene::Draw()
{

	skyBox_->Draw();

	if (isDrawObject3d_)
	{
		/*for (size_t i = 0; i < object3ds_.size(); i++)
		{
			object3ds_[i]->Draw();
		}*/

		terrain_->Draw();
		player_->Draw();
		enemy_->Draw();
		BulletManager::GetInstance()->Draw();
	}

	
	


	particleManager_->Draw();

	/*for (size_t i = 0; i < particle3ds.size(); i++)
	{
		particle3ds[i]->Draw();
	}*/

	//// SphereのIndexBufferViewを設定
	//commandList->IASetIndexBuffer(&indexBufferView);
	// 
	//// Sphereの描画
	//dxBase->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSphere);	// VBVを設定
	//// SphereのIndexBufferViewを設定
	//dxBase->GetCommandList()->IASetIndexBuffer(&indexBufferViewSphere);
	//// SphereのマテリアルのCBufferの場所を設定
	//dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSphere->GetGPUVirtualAddress());
	//// SphereのTransformationMatrixCBufferの場所を設定
	//dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationResourceSphere->GetGPUVirtualAddress());
	//// SRV用のDescriptorTableの先頭を設定。2はrootParameter[2]である。
	//dxBase->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandles[0]);	// SphereはCheckerを使う
	//// 平行光源用のCBufferをバインド（rootParameter[3] = b1）
	//dxBase->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLightResource->GetGPUVirtualAddress());
	//// 描画！（DrawCall/ドローコール）。
	//dxBase->GetCommandList()->DrawIndexedInstanced(kSubdivision * kSubdivision * 6, 1, 0, 0, 0);


	if (isDrawSprite_)
	{
		spriteManager_->DrawingCommon();

		for (size_t i = 0; i < sprites_.size(); i++)
		{
			sprites_[i]->Draw();
		}
	}

	cameraManager_->DrawDebugUI();

	debugManager_->DrawAll(cameraManager_->GetMainCamera()->GetViewProjectionMatrix());


	if (inputManager_->TriggerKey(DIK_0))
	{
		soundManager_->SoundPlayWave("Resources/Alarm01.wav");
	}
	if (inputManager_->TriggerKey(DIK_1))
	{
		soundManager_->SoundPlayWave("Resources/test.mp3");

	}
}
