#pragma once
#include "DirectXBase.h"

class ModelBase
{
public:

	void Initialize(DirectXBase* dxBase);

public:

	DirectXBase* GetDxBase() const { return dxBase_; }

private:

	DirectXBase* dxBase_;

};

