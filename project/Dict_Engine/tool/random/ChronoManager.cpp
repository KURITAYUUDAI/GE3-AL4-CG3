#include "ChronoManager.h"

std::unique_ptr<ChronoManager> ChronoManager::instance_ = nullptr;

ChronoManager* ChronoManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = std::make_unique<ChronoManager>(ConstructorKey());
	}
	return instance_.get();
}

void ChronoManager::Finalize()
{
	instance_.reset();
}

void ChronoManager::Start()
{
	start_ = std::chrono::high_resolution_clock::now();
}

void ChronoManager::End()
{
	end_ = std::chrono::high_resolution_clock::now();
	duratetionSec_ += std::chrono::duration_cast<std::chrono::milliseconds>(end_ - start_).count();
}
