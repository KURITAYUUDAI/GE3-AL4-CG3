#pragma once
#include "UIUtility.h"

class UIBase
{
public:

    virtual ~UIBase() = default;

    virtual void Initialize() = 0;
    virtual void Update(const GameUIViewModel& viewModel, const float& deltaTime) = 0;
    virtual void Draw() = 0;

public: // 外部入出力

    virtual UILayer GetLayer() const { return UILayer::Screen; }
    bool GetIsVisible() const { return isVisible_; }

    void SetIsVisible(bool visible) { isVisible_ = visible; }


protected:

    bool isVisible_ = true;
};

