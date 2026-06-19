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

    UIID AddUI(std::unique_ptr<UIBase<ViewModelType>> uiBase)
    {
        if (!uiBase)
        {
            return kInvalidUIID;
        }

        UIID id{ nextUIID_.value++ };
        uiBase->SetUIID(id);

        uiList_.push_back(std::move(uiBase));

        return id;
    }

    void RemoveUI(UIID id)
    {
        if (id == kInvalidUIID)
        {
            return;
        }

        uiList_.remove_if(
            [id](const std::unique_ptr<UIBase<ViewModelType>>& ui)
            {
                return ui && ui->GetUIID() == id;
            }
        );
    }

    void Clear()
    {
        uiList_.clear();
    }

    void SetIsVisible(bool visible) { isVisible_ = visible; }

    bool GetIsVisible() const { return isVisible_; }

private:
    bool isVisible_ = true;
    std::list<std::unique_ptr<UIBase<ViewModelType>>> uiList_;

    UIID nextUIID_ = 1;
};

