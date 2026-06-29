#pragma once
#include <memory>
#include <unordered_map>
#include "PostEffectManager.h"
#include "Dissolve.h"

class FadeManager
{
public:

    // シングルトンインスタンスの取得
    static FadeManager* GetInstance();
    // 終了
    void Finalize();

    // コンストラクタに渡すための鍵
    class ConstructorKey
    {
    private:
        ConstructorKey() = default;
        friend class FadeManager;
    };

    // PassKeyを受け取るコンストラクタ
    explicit FadeManager(ConstructorKey){}

private:

    // unique_ptr の型定義に Deleter を入れることでdeleteが可能になる
    static std::unique_ptr<FadeManager> instance_;

    ~FadeManager() = default;
    FadeManager(FadeManager&) = delete;
    FadeManager& operator=(FadeManager&) = delete;

    friend struct std::default_delete<FadeManager>;

public:

    enum class FadeType
    {
        Dissolve,
    };

    enum class Status
    {
        None,		// フェードなし
        FadeIn,		// フェードイン中
        FadeOut,	// フェードアウト中
    };

    

    void Initialize();

    void Update();

    void Start(FadeType fadeType, Status status, float duration);

    void Stop(FadeType fadeType);

    // フェード終了判定
    bool IsFinished(FadeType fadeType);

private:

    struct FadeState
    {
        Status  status = Status::None;
        float   duration = 0.0f;
        float   timer = 0.0f;
        float   progress = 0.0f;
        bool    active = false;
    };

    PostEffectManager* postEffectManager_ = PostEffectManager::GetInstance();

    Dissolve* dissolve_ = nullptr;

    std::unordered_map<FadeType, FadeState> fadeStates_;
};

