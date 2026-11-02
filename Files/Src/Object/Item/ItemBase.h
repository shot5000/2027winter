#pragma once
#include <DxLib.h>
#include "../Collision/BodyCollision.h"

class ItemBase
{

public:

	// アイテムでできること
	enum class KIND
	{
		ATTACK, // 攻撃だけできる
		CLEAN,  // 掃除だけできる
		BOTH,   // 攻撃も掃除もできる
	};

	// アイテムごとに決める値
	struct PARAM
	{
		const char* name_;		// 名前(スロットに表示する)
		KIND kind_;				// できること
		float activeTime_;		// 攻撃が出ている時間(秒)。攻撃できないアイテムは使わない
		float cooldownTime_;	// 次に攻撃できるまでの待ち時間(秒)。同上
		float cleanTime_;		// 汚れ1つを消すのにかかる長押しの時間(秒)。掃除できないアイテムは使わない
	};

	// 攻撃の経過の割合の、最小と最大
	static constexpr float RATE_MIN = 0.0f;
	static constexpr float RATE_MAX = 1.0f;

	// コンストラクタ(アイテムごとの値を受け取る)
	ItemBase(const PARAM& param);

	// デストラクタ
	virtual ~ItemBase(void);

	// 初期化
	virtual void Init(void);

	// 攻撃を出す(出せたら true。攻撃できないアイテム、出ている間、待ち時間中は出せない)
	bool Attack(void);

	// 更新(時間を進めて、出ている間は UpdateActive を呼ぶ)
	void Update(void);

	// 攻撃の描画(攻撃できるアイテムだけ、子クラスで作る)
	virtual void Draw(void) const;

	// 相手の体(カプセル)に、攻撃が当たっているか(攻撃できるアイテムだけ、子クラスで作る)
	virtual bool IsHitBody(
		const BodyCollision& body, const VECTOR& bodyPos) const;

	// 攻撃できるか、掃除できるか
	bool CanAttack(void) const;
	bool CanClean(void) const;

	// 名前
	const char* GetName(void) const;

	// 汚れ1つを消すのにかかる時間(秒)
	float GetCleanTime(void) const;

	// 攻撃が出ているか
	bool IsActive(void) const;

protected:

	// 攻撃を出した瞬間の処理(初期では何もしない)
	virtual void OnStart(void);

	// 攻撃が出ている間の処理(位置の更新など)
	// rate は経過の割合(0.0で出した瞬間、1.0で終わり)
	virtual void UpdateActive(float rate);

private:

	// アイテムごとの値
	PARAM param_;

	// 攻撃が出ているか
	bool isActive_;

	// 攻撃が出ている残り時間(秒)
	float activeTimer_;

	// 次に攻撃できるまでの残り時間(秒)
	float cooldownTimer_;

};