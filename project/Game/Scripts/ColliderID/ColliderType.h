#pragma once

enum TypeID {
	kPlayer,	// プレイヤー
	kYoyo,		// ヨーヨー
	kEnemy,		// エネミー
	kObstacle,	// 障害物
	kHP,		// 回復アイテム
	kPower,		// 攻撃アップアイテム
	kSpeed,		// スピードアップアイテム
	kNone		// 判定を取らない
};