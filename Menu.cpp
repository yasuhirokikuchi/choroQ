#include "pch.h"
#include "Menu.h"
#include "SceneMgr.h"

static int KeyState_Menu[256] = { 0 };

static int MenuHandle;    // 画像ハンドル格納用変数

// 初期化
void Menu_Initialize() {
    for (int i = 0; i < 256; i++) {
        KeyState_Menu[i] = 0;
    }

    MenuHandle = LoadGraph(IMAGE_DIR "sundome.png");    // 画像のロード
}

// 終了処理
void Menu_Finalize() {
    DeleteGraph(MenuHandle);    // 画像の解放
}

// 更新
void Menu_Update() {
    GetHitKeyStateAllEx(KeyState_Menu);

    if (KeyState_Menu[KEY_INPUT_E] == 1) { // Eキーが押されていたら
        SceneMgr_ChangeScene(eScene_Game); // シーンをゲーム画面に変更
    }
    if (KeyState_Menu[KEY_INPUT_Z] == 1) { // Zキーが押されていたら
        DxLib_End();
    }
}

// 描画
void Menu_Draw() {
    DrawBox(0, 0, 680, 480, GetColor(255, 255, 255), TRUE);
    DrawExtendGraph(100, 20, 500, 400, MenuHandle, TRUE);
    DrawString(170, 340, "Eキーを押すとゲーム画面に進みます。", GetColor(0, 0, 0));
    DrawString(230, 360, "Zキーを押して終了。", GetColor(0, 0, 0));
}
