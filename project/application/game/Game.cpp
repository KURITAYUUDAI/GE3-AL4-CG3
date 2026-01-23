#include "Game.h"

void Game::Initialize()
{
	Dict_Framework::Initialize();

	// Textureを読んで転送する
	textureManager_->LoadTexture("resources/uvChecker.png");
	textureManager_->LoadTexture("resources/monsterBall.png");



	int textureIndex = 0;
	const char* textureOptions[] = { "Checker", "monsterBall" };

	// シーン初期化始め
	/*camera_->SetRotate({ pi / 3.0f, pi, 0.0f });
	camera_->SetTranslate({ 0.0f, 9.0f, 5.0f });*/

	camera_->SetRotate({ 0.0f, pi, 0.0f });
	camera_->SetTranslate({ 0.0f, 0.0f, 15.0f });
	object3dBase_->SetDefaultCamera(camera_.get());
	particleManager_->SetDefaultCamera(camera_.get());

	

	for (size_t i = 0; i < 1; i++)
	{
		std::unique_ptr<Sprite> newSprite = std::make_unique<Sprite>();
		newSprite->Initialize(spriteBase_.get(), "resources/uvChecker.png");
		newSprite->SetAnchorPoint(Vector2{ 0.5f, 0.5f });
		sprites_.push_back(std::move(newSprite));
	}

	sprites_[0]->SetTexture("resources/monsterBall.png");
	sprites_[0]->SetPosition(Vector2{ 100.0f, 100.0f });
	sprites_[0]->AdjustTextureSize();



	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");
	ModelManager::GetInstance()->LoadModel("sphere.obj");

	for (size_t i = 0; i < 2; i++)
	{
		std::unique_ptr<Object3d> newObject3d = std::make_unique<Object3d>();
		newObject3d->Initialize(object3dBase_.get());
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


	// クラッシュ用
	/*uint32_t* p = nullptr;
	*p = 100;*/
}

void Game::Finalize()
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

	Dict_Framework::Finalize();
}

void Game::Update()
{
	// 規定クラスの往診処理
	Dict_Framework::Update();

	imguiManager_->Begin();

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

	ImGui::Begin("CameraSetting");
	Vector3 cameraRotate = camera_->GetRotate();
	Vector3 cameraTranslate = camera_->GetTranslate();
	if(ImGui::DragFloat3("CameraRotate", &cameraRotate.x, (1.0f / 180.0f) * pi))
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
	if(ImGui::Checkbox("EnableLighting", (bool*)&enableLighting))
	{
		object3ds_[0]->SetEnableLighting(enableLighting);
	}
	if (ImGui::ColorEdit4("LightColor", &directionLightColor.x))
	{
		object3ds_[0]->SetLightColor(directionLightColor);
	}
	if (ImGui::DragFloat3("LightDirection", &directionLightDirection.x, 0.01f))
	{
		object3ds_[0]->SetLightDirection(directionLightDirection);
	}
	if (ImGui::DragFloat("LightIntensity", &directionLightIntensity, 0.01f))
	{
		object3ds_[0]->SetLightIntensity(directionLightIntensity);
	}

	Vector3 cameraPos = object3ds_[0]->GetCameraWorldPosition();
	ImGui::InputFloat3("CameraData", &cameraPos.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

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

	mousePosition_.x = static_cast<float>(inputManager_->MousePoint(winAPI_->GetHwnd()).x);
	mousePosition_.y = static_cast<float>(inputManager_->MousePoint(winAPI_->GetHwnd()).y);

	camera_->Update();
	if (isDebugCamera_)
	{
		debugCamera_.Update(inputManager_, camera_->GetTransform());
		camera_->SetViewMatrix(debugCamera_.GetViewMatrix());
	}
	else
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

	imguiManager_->End();
}

void Game::Draw()
{
	// 描画の処理
	dxBase_->PreDraw();

	srvManager_->PreDraw();

	if (isDrawObject3d_)
	{
		object3dBase_->DrawingCommon();

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
		spriteBase_->DrawingCommon();

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


	imguiManager_->Draw();


	dxBase_->PostDraw();
}

