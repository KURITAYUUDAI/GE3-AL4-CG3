#pragma once
#include "UIUtility.h"
#include <algorithm>

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
    UIID GetUIID() const { return uiID_; }
    bool GetIsVisible() const { return isVisible_; }

    void SetUIID(UIID id) { uiID_ = id; }
    void SetIsVisible(bool visible) { isVisible_ = visible; }
    
protected:

    UIID uiID_ = kInvalidUIID;
    bool isVisible_ = true;
};

