#include "pch.h"
#include "Menu.h"
#include "SceneMgr.h"
#include <string.h>

static int KeyState_Menu[256] = { 0 };

static int MenuHandle;    // 画像ハンドル格納用変数
static int FontMenu = -1;

// 文字の後ろに板を敷く
static void DrawMenuPlate(int y, const char* text) {
    unsigned int color = GetColor(40, 28, 18);
    if (FontMenu == -1) {
        DrawString(170, y, text, color);
        return;
    }

    int textW = GetDrawStringWidthToHandle(text, (int)strlen(text), FontMenu);
    int textH = GetFontSizeToHandle(FontMenu);
    const int padX = 16;
    const int padY = 8;
    int x = (640 - (textW + padX * 2)) / 2;
    int x2 = x + textW + padX * 2;
    int y2 = y + textH + padY * 2;

    DrawBox(x + 3, y + 3, x2 + 3, y2 + 3, GetColor(168, 136, 96), TRUE);
    DrawBox(x, y, x2, y2, GetColor(255, 250, 236), TRUE);
    DrawBox(x, y, x2, y2, GetColor(72, 48, 24), FALSE);
    DrawStringToHandle(x + padX, y + padY, text, color, FontMenu);
}

// 初期化
void Menu_Initialize() {
    for (int i = 0; i < 256; i++) {
        KeyState_Menu[i] = 0;
    }

    MenuHandle = LoadGraph(IMAGE_DIR "sundome.png");    // 画像のロード
    if (FontMenu == -1) {
        FontMenu = CreateFontToHandle("ＭＳ ゴシック", 24, 5, DX_FONTTYPE_NORMAL);
    }
}

// 終了処理
void Menu_Finalize() {
    DeleteGraph(MenuHandle);    // 画像の解放
    if (FontMenu != -1) {
        DeleteFontToHandle(FontMenu);
        FontMenu = -1;
    }
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
    DrawBox(0, 0, 640, 480, GetColor(255, 248, 230), TRUE);
    DrawExtendGraph(110, 16, 530, 360, MenuHandle, TRUE);
    DrawMenuPlate(376, "Eキーでゲーム開始");
    DrawMenuPlate(428, "Zキーで終了");
}
