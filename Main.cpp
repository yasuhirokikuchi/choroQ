#include "pch.h"
#include "SceneMgr.h"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // 文字列をUTF-8として扱う（/utf-8 でコンパイルしたリテラルに合わせる）
    SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);

    // 640x480 のまま画面全体へ拡大する（ドットは最近傍でくっきり、比率は維持）
    SetGraphMode(640, 480, 32);
    SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_BORDERLESS_WINDOW);
    SetFullScreenScalingMode(DX_FSSCALINGMODE_NEAREST, TRUE);
    ChangeWindowMode(FALSE);

    if (DxLib_Init() == -1) {
        return -1;
    }
    SetDrawScreen(DX_SCREEN_BACK);

    SceneMgr_Initialize();

    // 画面反転 & メッセージ処理 & 画面クリア
    while (ScreenFlip() == 0 && ProcessMessage() == 0 && ClearDrawScreen() == 0) {
        SceneMgr_Update();  // 更新
        SceneMgr_Draw();    // 描画
    }

    SceneMgr_Finalize();

    DxLib_End(); // DXライブラリの終了処理

    return 0;
}
