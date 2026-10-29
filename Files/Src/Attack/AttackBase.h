#pragma once
#include <DxLib.h>
#include "../Object/Collision/BodyCollision.h"

class AttackBase
{

public:

	// 攻撃ごとに決める時間
	struct PARAM
	{
		float activeTime_;		// 攻撃が出ている時間(秒)
		float cooldownTime_;	// 次に出せるまでの待ち時間(秒)
	};

	// 経過の割合の、最小と最大
	static constexpr float RATE_MIN = 0.0f;
	static constexpr float RATE_MAX = 1.0f;

	// コンストラクタ(攻撃ごとの時間を受け取る)
	AttackBase(const PARAM& param);

	// デストラクタ
	virtual ~AttackBase(void);

	// 初期化
	virtual void Init(void);

	// 攻撃を出す(出せたら true。出ている間や待ち時間中は出せない)
	bool Trigger(void);

	// 更新(時間を進めて、出ている間は UpdateActive を呼ぶ)
	void Update(void);

	// 描画(攻撃ごとに見た目が違うので、必ず子クラスで作る)
	virtual void Draw(void) const = 0;

	// 相手の体(カプセル)に当たっているか(当たり判定の形は、攻撃ごとに違うので、子クラスで作る)
	virtual bool IsHitBody(
		const BodyCollision& body, const VECTOR& bodyPos) const = 0;

	// 攻撃が出ているか
	bool IsActive(void) const;

protected:

	// 出した瞬間の処理(初期では何もしない)
	virtual void OnStart(void);

	// 出ている間の処理(位置の更新など)
	// rate は経過の割合(0.0で出した瞬間、1.0で終わり)
	virtual void UpdateActive(float rate) = 0;

private:

	// 攻撃ごとの時間
	PARAM param_;

	// 攻撃が出ているか
	bool isActive_;

	// 攻撃が出ている残り時間(秒)
	float activeTimer_;

	// 次に出せるまでの残り時間(秒)
	float cooldownTimer_;

};