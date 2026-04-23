#pragma once
#include "DirectXBase.h"
#include "object3d.h"
#include "myMath.h"
#include "Model.h"
#include <memory>

class Terrain
{
public:

	void Initialize();
	void Update();
	void Draw();
	void Finalize();

private:

	std::unique_ptr<Object3d> object3d_;

	Transform transform_;

	bool isDraw_ = true;
};

