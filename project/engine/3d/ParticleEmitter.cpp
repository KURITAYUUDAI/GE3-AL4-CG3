#include "ParticleEmitter.h"
#include "ParticleManager.h"

void ParticleEmitter::Initialize(const std::string& name, const Transform& transform, 
	const uint32_t& count, const float& frequency)
{
	name_ = name;
	transform_ = transform;
	count_ = count;
	frequency_ = frequency;
	frequencyTime_ = 0.0f;
}

void ParticleEmitter::Update()
{
	frequencyTime_ += kDeltaTime;
	if (frequency_ <= frequencyTime_)
	{
		ParticleManager::GetInstance()->Emit(name_, transform_.translate, count_);
		frequencyTime_ -= frequency_;
	}
}

void ParticleEmitter::Emit()
{
	ParticleManager::GetInstance()->Emit(name_, transform_.translate, count_);
}

