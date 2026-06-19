#include "UIManager.h"

//void UIManager::Initialize()
//{
//
//}
//
//void UIManager::Update(const GameUIViewModel& viewModel, const float& deltaTime)
//{
//    if (!isVisible_) 
//    {
//        return;
//    }
//
//    for (auto& ui : uiList_) 
//    {
//        /*if (!ui->GetIsVisible()) 
//        {
//            continue;
//        }*/
//
//        ui->Update(viewModel, deltaTime);
//    }
//}
//
//void UIManager::DrawLayer(const UILayer& layer)
//{
//    if (!isVisible_) {
//        return;
//    }
//
//    for (auto& ui : uiList_) 
//    {
//        if (!ui->GetIsVisible()) 
//        {
//            continue;
//        }
//
//        if (ui->GetLayer() != layer) 
//        {
//            continue;
//        }
//
//        ui->Draw();
//    }
//}
//
//void UIManager::AddUI(std::unique_ptr<UIBase> uiBase)
//{
//    uiList_.push_back(std::move(uiBase));
//}
//
