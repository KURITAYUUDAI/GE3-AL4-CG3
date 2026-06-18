#pragma once
#include <list>
#include "Collider.h"
#include <memory>

class CollisionManager
{
public:

	// シングルトンインスタンスの取得
	static CollisionManager* GetInstance();
	// 終了
	void Finalize();

	// コンストラクタに渡すための鍵
	class ConstructorKey
	{
	private:
		ConstructorKey() = default;
		friend class CollisionManager;
	};

	// PassKeyを受け取るコンストラクタ
	explicit CollisionManager(ConstructorKey){}

private:

	// unique_ptr の型定義に Deleter を入れることでdeleteが可能になる
	static std::unique_ptr<CollisionManager> instance_;

	~CollisionManager() = default;
	CollisionManager(CollisionManager&) = delete;
	CollisionManager& operator=(CollisionManager&) = delete;

	friend struct std::default_delete<CollisionManager>;


public:
	void AddCollider(Collider* collider);
	void Clear();
	void CheckAllCollisions();
	bool CanCheckCollision(Collider* a, Collider* b);

private:
	std::list<Collider*> colliders_;
};

