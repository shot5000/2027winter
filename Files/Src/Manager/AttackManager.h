#pragma once
#include <memory>
#include <vector>
#include "../Attack/AttackBase.h"

class AttackManager
{

public:

	// 攻撃の種類(増やすときは、ここに足して、Init の登録も同じ順に足す)
	enum class TYPE
	{
		MOP,
		MAX,
	};

	// コンストラクタ
	AttackManager(void);

	// デストラクタ
	~AttackManager(void);

	// 初期化
	void Init(void);

	// 更新(左クリックで、選んでいる攻撃を出す)
	void Update(void);

	// 描画
	void Draw(void) const;

	// 解放
	void Release(void);

	// 使う攻撃を切り替える
	void SetType(TYPE type);

	// どれかの攻撃が、相手の体(カプセル)に当たっているか
	bool IsHitBody(const BodyCollision& body, const VECTOR& bodyPos) const;

private:

	// 攻撃の配列(TYPE の並びと同じ順)
	std::vector<std::unique_ptr<AttackBase>> attacks_;

	// 今選んでいる攻撃
	TYPE type_;

	// 前フレームで、左クリックを押していたか
	bool isPrevClick_;

};