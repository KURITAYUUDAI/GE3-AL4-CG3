#pragma once
#include "UIUtility.h"

template<class ViewModelType>
class UIBase
{
public:

    virtual ~UIBase() = default;

    virtual void Initialize() = 0;
    virtual void Update(const ViewModelType& viewModel, const float& deltaTime) = 0;
    virtual void Draw() = 0;

public: // 外部入出力

    virtual UILayer GetLayer() const { return UILayer::Screen; }
    bool GetIsVisible() const { return isVisible_; }

    void SetIsVisible(bool visible) { isVisible_ = visible; }


protected:

    bool isVisible_ = true;
};

