#pragma once
#include "UIBase.h"
#include "UIUtility.h"
#include <list>
#include <memory>

template<class ViewModelType>
class UIManager
{
public:
    void Initialize()
    {}

    void Update(const ViewModelType& viewModel, const float& deltaTime)
    {
        if (!isVisible_)
        {
            return;
        }

        for (auto& ui : uiList_)
        {
            if (!ui->GetIsVisible())
            {
                continue;
            }

            ui->Update(viewModel, deltaTime);
        }
    }

    void DrawLayer(const UILayer& layer)
    {
        if (!isVisible_)
        {
            return;
        }

        for (auto& ui : uiList_)
        {
            if (!ui->GetIsVisible())
            {
                continue;
            }

            if (ui->GetLayer() != layer)
            {
                continue;
            }

            ui->Draw();
        }
    }

    void AddUI(std::unique_ptr<UIBase<ViewModelType>> uiBase)
    {
        if (!uiBase)
        {
            return;
        }

        uiList_.push_back(std::move(uiBase));
    }

    void SetIsVisible(bool visible) { isVisible_ = visible; }

    bool GetIsVisible() const { return isVisible_; }

private:
    bool isVisible_ = true;
    std::list<std::unique_ptr<UIBase<ViewModelType>>> uiList_;
};

