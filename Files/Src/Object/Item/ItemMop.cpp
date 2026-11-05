#include "../../Manager/ParamManager.h"
#include "../../Utility/AsoUtility.h"
#include "ItemMop.h"

ItemMop::ItemMop(void)
	: ItemBase(ITEM_PARAM, ParamManager::GetInstance().GetItem(PARAM_ID))
{
	startPos_ = AsoUtility::VECTOR_ZERO;
	endPos_ = AsoUtility::VECTOR_ZERO;
}

ItemMop::~ItemMop(void)
{
}

void ItemMop::Draw(void) const
{
	if (!IsActive())
	{
		return;
	}

	// 半透明で、ライトを切って描く設定にする
	SetUseLighting(FALSE);
	SetWriteZBuffer3D(FALSE);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, DRAW_ALPHA);

	// 仮の見た目。モデルやエフェクトができたら、ここを差し替える
	DrawCapsule3D(
		startPos_, endPos_, RADIUS,
		DRAW_DIV_NUM, DRAW_COLOR, DRAW_COLOR, false);

	// 設定を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	SetWriteZBuffer3D(TRUE);
	SetUseLighting(TRUE);
}

bool ItemMop::IsHitBody(
	const BodyCollision& body, const VECTOR& bodyPos) const
{
	// 攻撃が出ていないときは、当たらない
	if (!IsActive())
	{
		return false;
	}

	// 攻撃のカプセルと、相手のカプセルが重なっているか
	return HitCheck_Capsule_Capsule(
		startPos_, endPos_, RADIUS,
		body.GetCapsuleTop(bodyPos), body.GetCapsuleDown(bodyPos),
		body.GetRadius()) == TRUE;
}

void ItemMop::UpdateActive(float rate)
{
	// 持ち手の位置(目の少し前、少し下)
	VECTOR eye_ = GetCameraPosition();
	VECTOR front_ = GetCameraFrontVector();
	startPos_ = VAdd(eye_, VScale(front_, HAND_FRONT_DIST));
	startPos_.y += HAND_OFFSET_Y;

	// 見ている向きに、リーチの長さだけ、まっすぐ先端を伸ばす
	endPos_ = VAdd(startPos_, VScale(front_, GetReach()));
}