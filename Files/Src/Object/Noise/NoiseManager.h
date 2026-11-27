#pragma once
#include <vector>
#include <DxLib.h>

// ゲーム中に出た音を覚えておく(敵はここから、聞こえた音を調べる)
class NoiseManager
{

public:

	// 音1つ分の情報
	struct NOISE
	{
		VECTOR pos_;	// 音が出た場所
		float radius_;	// 音が聞こえる範囲
		int id_;		// 音の番号(新しい音ほど大きい)
		float age_;		// 音が出てからの時間(秒)
	};

	// 音の番号がまだ無いことを表す値
	static constexpr int NO_ID = 0;

	// 音を覚えておく時間(秒。確認用の表示にも使う)
	static constexpr float LIFE_TIME = 1.0f;

	// 音の大きさ(聞こえる範囲)
	static constexpr float DOOR_RADIUS = 800.0f;

	// 走る音の大きさと、出す間隔(秒)
	static constexpr float RUN_RADIUS = 1000.0f;
	static constexpr float RUN_INTERVAL = 0.5f;

	// 攻撃と掃除の音の基準の大きさ(アイテムの音量をかけて使う)
	static constexpr float ATTACK_BASE_RADIUS = 1000.0f;
	static constexpr float CLEAN_BASE_RADIUS = 600.0f;

	// 範囲の描画：床からの高さ、色、輪の分割数
	static constexpr float DRAW_OFFSET_Y = 30.0f;
	static constexpr unsigned int DRAW_COLOR = 0xffdd33;
	static constexpr int DRAW_DIV_NUM = 48;

	// 円の角度(全体)
	static constexpr float CIRCLE_ANGLE = 360.0f * DX_PI_F / 180.0f;

	// 音を出す
	static void Emit(const VECTOR& pos, float radius);

	// 更新(古い音を消す)
	static void Update(float deltaTime);

	// 全部消す(ゲームの開始と終了で呼ぶ)
	static void Reset(void);

	// 今ある音の一覧
	static const std::vector<NOISE>& GetNoises(void);

	// 音の範囲の描画(確認用)
	static void DrawDebug(void);

private:

	// 今ある音
	static std::vector<NOISE> noises_;

	// 次に出す音の番号
	static int nextId_;

};