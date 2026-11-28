#include "pch.h"
#include "Game.h"
#include "SceneMgr.h"
#include <stdio.h>
#include <string.h>

#define HAIKEI_SIZE_W 640          // 背景の幅

#define CAR_DRAW_WIDTH 200         // 車の描画幅（高さは画像比率から算出）
#define CAR_START_X 20             // 車の初期X（画面左寄り）
#define CAR_PULL_LIMIT -60         // 引き切った位置（開始から80px）
#define CAR_Y0 307                 // タイヤが机に着くY
#define CAR_Y1 325
#define CAR_Y2 333

#define FINGER_X_DIFFERENCE 270    // 指画像の幅
#define FINGER_Y_DIFFERENCE 144    // 指画像の高さ
#define FINGER_X_ON_CAR -164       // 車の左端から指画像までのずれ（左にはみ出す）
#define FINGER_Y_CAR0 168          // 指先の黒枠が屋根のすぐ上に来るY
#define FINGER_Y_CAR1 185
#define FINGER_Y_CAR2 193
#define FINGER_PULL_LIMIT -224     // 引き切ったときの指X

#define BOMB_X_DIFFERENCE 50    // 爆弾のX座標の差
#define BOMB_Y_DIFFERENCE 60    // 爆弾のY座標の差
#define BOMB_START_X 690.0      // 爆弾の初期X（画面外）
#define BOMB_STOP_GAP 20.0      // 寸止め成功時の車〜爆弾の間隔
#define BOMB_APPEAR_REMAIN 6.0  // この残距離から画面内へ入り始める（小さいほど接近が速い）

#define BAKUHATU_X_DIFFERENCE 300    // 爆発のX座標の差
#define BAKUHATU_Y_DIFFERENCE 300    // 爆発のY座標の差

#define GUGUGU_W 420
#define GUGUGU_H 133
#define PA_W 260
#define PA_H 153
#define DOKKAN_W 480
#define DOKKAN_H 135


// キーの状態
static int KeyState_Game[256] = { 0 };


// 車の座標＋画像ファイル用変数
static double Car_x, Car_x2;
static int  Car_y, Car_y2;
static int CarHandle_1, CarHandle_2, CarHandle_3;
static int CarDrawW, CarDrawH; // アスペクト比を保った描画サイズ


// 車の種類
static int Car;
static int Car_num = 0;


// 指の座標＋画像ファイル用変数
static double Finger_x, Finger_x2;
static int Finger_y, Finger_y2, FingerHandle;


// 爆弾の座標＋画像ファイル用変数
static double Bomb_x, Bomb_x2;
static int  Bomb_y, Bomb_y2, BombHandle;


// 爆発エフェクトの座標＋画像ファイル用変数
static double Bakuhatu_x, Bakuhatu_x2;
static int  Bakuhatu_y, Bakuhatu_y2, BakuhatuHandle;


// ぐぐぐの文字の座標＋画像ファイル用変数
static double Gugugu_x, Gugugu_x2;
static int  Gugugu_y, Gugugu_y2, GuguguHandle;

// オーバーヒート
static int Over;

// ぱっの座標＋画像ファイル用変数
static double Pa_x, Pa_x2;
static int  Pa_y, Pa_y2, PaHandle;


// どっかーんの座標＋画像ファイル用変数
static double Dokkan_x, Dokkan_x2;
static int Dokkan_y, Dokkan_y2, DokkanHandle;


// 背景の座標(x軸)＋画像ファイル用変数
static int Haikei, HaikeiHandle, tukueHandle;


static int Power;             // 車のパワー
static double Speed;          // 車のスピード
static double Goal;           // 爆弾までの距離(基準点)
static double Distance;       // 移動距離


// 振動用変数
static int vibration;
static int vibration_Car;
static int vibration_Haikei;


// ゲージ
static double Gage;

// 画面上の文字（結果・数値・操作説明）
static int FontHud = -1;
static int FontResult = -1;


// 真偽のやつ
static bool Car_Select;
static bool Push;
static bool Pull;
static bool Gugugu;
static bool Genkai;
static bool Pa;
static bool Start;
static bool Bomb;
static bool Camera;
static bool End;
static bool Out;

// 現在の車種に合わせ、画像の縦横比を保った描画サイズを求める
static void UpdateCarDrawSize() {
    int handle = CarHandle_1;
    if (Car == 1) {
        handle = CarHandle_2;
    }
    else if (Car == 2) {
        handle = CarHandle_3;
    }

    int graphW = 0;
    int graphH = 0;
    GetGraphSize(handle, &graphW, &graphH);

    CarDrawW = CAR_DRAW_WIDTH;
    if (graphW > 0 && graphH > 0) {
        CarDrawH = CarDrawW * graphH / graphW;
    }
    else {
        CarDrawH = CarDrawW / 2;
    }
}

// 車の右下座標を、比率を保ったサイズで更新する
static void UpdateCarRect() {
    UpdateCarDrawSize();
    Car_x2 = Car_x + CarDrawW;
    Car_y2 = Car_y + CarDrawH;
}

// 残り距離（Goal - Distance）に比例して車〜爆弾の間隔を更新する
static void UpdateBombPosition() {
    double bombStopX = Car_x2 + BOMB_STOP_GAP;
    double remain = Goal - Distance;
    double farGap = BOMB_START_X - bombStopX;
    if (farGap < 1.0) {
        farGap = 1.0;
    }
    // 残距離 0 → 寸止め間隔 / 残距離 APPEAR → 画面右端付近
    double pixelsPerRemain = farGap / BOMB_APPEAR_REMAIN;

    if (Camera == TRUE) {
        Bomb_x = BOMB_START_X;
    }
    else if (remain > BOMB_APPEAR_REMAIN) {
        // まだ遠い：画面外。不足が大きいほどさらに右
        Bomb_x = BOMB_START_X + (remain - BOMB_APPEAR_REMAIN) * pixelsPerRemain;
    }
    else if (remain >= 0) {
        // 残り距離の差がそのまま見た目の差になる
        Bomb_x = bombStopX + remain * pixelsPerRemain;
    }
    else {
        // わずかに行き過ぎて停止した場合用（爆発せず寸止め結果へ）
        Bomb_x = bombStopX + remain * pixelsPerRemain;
        if (Bomb_x < Car_x2 + 2.0) {
            Bomb_x = Car_x2 + 2.0;
        }
    }
    Bomb_x2 = Bomb_x + BOMB_X_DIFFERENCE;
}

// 結果と数値用のフォントを用意する（Rキーのやり直しでは作り直さない）
static void EnsureGameFonts() {
    if (FontHud == -1) {
        FontHud = CreateFontToHandle("ＭＳ ゴシック", 20, 5, DX_FONTTYPE_NORMAL);
    }
    if (FontResult == -1) {
        FontResult = CreateFontToHandle("ＭＳ ゴシック", 34, 6, DX_FONTTYPE_NORMAL);
    }
}

static int TextWidth(const char* text, int font) {
    return GetDrawStringWidthToHandle(text, (int)strlen(text), font);
}

// 文字の後ろに板を敷き、壁の色に埋まらないようにする
static void DrawPlateText(int x, int y, const char* text, int font, unsigned int color) {
    if (font == -1) {
        DrawFormatString(x, y, color, "%s", text);
        return;
    }

    int textW = TextWidth(text, font);
    int textH = GetFontSizeToHandle(font);
    const int padX = 10;
    const int padY = 6;
    int x2 = x + textW + padX * 2;
    int y2 = y + textH + padY * 2;

    DrawBox(x + 3, y + 3, x2 + 3, y2 + 3, GetColor(168, 136, 96), TRUE);
    DrawBox(x, y, x2, y2, GetColor(255, 250, 236), TRUE);
    DrawBox(x, y, x2, y2, GetColor(72, 48, 24), FALSE);
    DrawStringToHandle(x + padX, y + padY, text, color, font);
}

static void DrawCenteredPlateText(int y, const char* text, int font, unsigned int color) {
    int textW = (font == -1) ? 0 : TextWidth(text, font);
    int x = (640 - (textW + 20)) / 2;
    if (x < 8) {
        x = 8;
    }
    DrawPlateText(x, y, text, font, color);
}

// 結果を、見出し・内容・操作に分けて中央に出す
static void DrawResultBoard(const char* title, unsigned int titleColor, const char* detail) {
    const char* hint = "Rキーでリスタート    Escでメニュー";
    if (FontResult == -1 || FontHud == -1) {
        DrawFormatString(160, 40, titleColor, "%s", title);
        DrawFormatString(160, 64, GetColor(0, 0, 0), "%s", detail);
        DrawFormatString(160, 88, GetColor(0, 0, 0), "%s", hint);
        return;
    }

    int titleW = TextWidth(title, FontResult);
    int detailW = TextWidth(detail, FontHud);
    int hintW = TextWidth(hint, FontHud);
    int contentW = titleW;
    if (detailW > contentW) {
        contentW = detailW;
    }
    if (hintW > contentW) {
        contentW = hintW;
    }

    int titleH = GetFontSizeToHandle(FontResult);
    int bodyH = GetFontSizeToHandle(FontHud);
    const int padX = 24;
    const int padY = 14;
    const int lineGap = 8;
    int w = contentW + padX * 2;
    int h = padY + titleH + lineGap + bodyH + lineGap + bodyH + padY;
    int x = (640 - w) / 2;
    int y = 14;

    DrawBox(x + 4, y + 4, x + w + 4, y + h + 4, GetColor(168, 136, 96), TRUE);
    DrawBox(x, y, x + w, y + h, GetColor(255, 250, 236), TRUE);
    DrawBox(x, y, x + w, y + h, GetColor(72, 48, 24), FALSE);
    DrawBox(x + 4, y + 4, x + w - 4, y + h - 4, GetColor(214, 176, 116), FALSE);

    DrawStringToHandle(x + (w - titleW) / 2, y + padY, title, titleColor, FontResult);
    int lineY = y + padY + titleH + lineGap;
    DrawStringToHandle(x + (w - detailW) / 2, lineY, detail, GetColor(48, 34, 20), FontHud);
    lineY += bodyH + lineGap;
    DrawStringToHandle(x + (w - hintW) / 2, lineY, hint, GetColor(96, 68, 36), FontHud);
}

// 初期化
void Game_Initialize() {

    // 色々初期化
    Power = 0;
    Speed = 0;
    Distance = 0;
    Gage = 0;
    Haikei = 0;
    Over = 0;

    KeyState_Game[KEY_INPUT_SPACE] = 0;
    EnsureGameFonts();

    Car_Select = TRUE;
    Push = TRUE;
    Pull = TRUE;
    Start = TRUE;
    Camera = TRUE;
    Bomb = TRUE;
    End = FALSE;
    Out = FALSE;
    Gugugu = FALSE;
    Pa = FALSE;
    Genkai = FALSE;

    // 車種を選んだ車に合わせる（未初期化参照を防ぐ）
    Car = Car_num;

    // ランダムなゴールの位置
    Goal = 100 + (GetRand(10) * 10);


    // 車と指の初期座標（タイヤは机、指先は屋根）
    Car_x = CAR_START_X;
    Finger_x = CAR_START_X + FINGER_X_ON_CAR;
    if (Car == 0) {
        Car_y = CAR_Y0;
        Finger_y = FINGER_Y_CAR0;
    }
    else if (Car == 1) {
        Car_y = CAR_Y1;
        Finger_y = FINGER_Y_CAR1;
    }
    else {
        Car_y = CAR_Y2;
        Finger_y = FINGER_Y_CAR2;
    }
    Finger_x2 = Finger_x + FINGER_X_DIFFERENCE;
    Finger_y2 = Finger_y + FINGER_Y_DIFFERENCE;


    // 爆弾の初期座標
    Bomb_x = BOMB_START_X;
    Bomb_y = 385;
    Bomb_x2 = Bomb_x + BOMB_X_DIFFERENCE;
    Bomb_y2 = Bomb_y + BOMB_Y_DIFFERENCE;


    // 爆発エフェクトの初期座標
    Bakuhatu_x = 120;
    Bakuhatu_y = 220;
    Bakuhatu_x2 = Bakuhatu_x + BAKUHATU_X_DIFFERENCE;
    Bakuhatu_y2 = Bakuhatu_y + BAKUHATU_Y_DIFFERENCE;


    // ぐぐぐの文字の初期座標
    Gugugu_x = 100;
    Gugugu_y = 180;
    Gugugu_x2 = Gugugu_x + GUGUGU_W;
    Gugugu_y2 = Gugugu_y + GUGUGU_H;


    // ぱっの文字の座標
    Pa_x = 100;
    Pa_y = 180;
    Pa_x2 = Pa_x + PA_W;
    Pa_y2 = Pa_y + PA_H;


    // どっかーんの文字の座標
    Dokkan_x = 120;
    Dokkan_y = 150;
    Dokkan_x2 = Dokkan_x + DOKKAN_W;
    Dokkan_y2 = Dokkan_y + DOKKAN_H;


    // 振動
    vibration_Haikei = 0;


    // 画像のロード
    CarHandle_1 = LoadGraph(IMAGE_DIR "car.png");
    CarHandle_2 = LoadGraph(IMAGE_DIR "car2.png");
    CarHandle_3 = LoadGraph(IMAGE_DIR "car3.png");
    FingerHandle = LoadGraph(IMAGE_DIR "finger.png");

    HaikeiHandle = LoadGraph(IMAGE_DIR "haikei.png");
    tukueHandle = LoadGraph(IMAGE_DIR "tukue.png");
    BombHandle = LoadGraph(IMAGE_DIR "bomb.png");
    BakuhatuHandle = LoadGraph(IMAGE_DIR "bakuhatu.png");

    GuguguHandle = LoadGraph(IMAGE_DIR "gugugu.png");
    PaHandle = LoadGraph(IMAGE_DIR "pa.png");
    DokkanHandle = LoadGraph(IMAGE_DIR "dokkan.png");

    UpdateCarRect();
}

// 終了処理
void Game_Finalize() {

    // 画像の解放
    DeleteGraph(CarHandle_1);
    DeleteGraph(CarHandle_2);
    DeleteGraph(CarHandle_3);
    DeleteGraph(FingerHandle);
    DeleteGraph(HaikeiHandle);
    DeleteGraph(tukueHandle);
    DeleteGraph(BombHandle);
    DeleteGraph(BakuhatuHandle);
    DeleteGraph(GuguguHandle);
    DeleteGraph(PaHandle);
    DeleteGraph(DokkanHandle);

    if (FontHud != -1) {
        DeleteFontToHandle(FontHud);
        FontHud = -1;
    }
    if (FontResult != -1) {
        DeleteFontToHandle(FontResult);
        FontResult = -1;
    }
}

// 更新
void Game_Update() {

    GetHitKeyStateAllEx(KeyState_Game);

    if (CheckHitKey(KEY_INPUT_ESCAPE) == 1) { // Escキーが押されていたら
        SceneMgr_ChangeScene(eScene_Menu); // シーンをメニューに変更
    }
    if (KeyState_Game[KEY_INPUT_R] == 1) { // Rキーが押されていたら
        Game_Initialize();                // ゲームを初めからやり直す
    }

    UpdateCarRect();

    Finger_x2 = Finger_x + FINGER_X_DIFFERENCE;
    Finger_y2 = Finger_y + FINGER_Y_DIFFERENCE;

    Bomb_x2 = Bomb_x + BOMB_X_DIFFERENCE;
    Bomb_y2 = Bomb_y + BOMB_Y_DIFFERENCE;

    Bakuhatu_x2 = Bakuhatu_x + BAKUHATU_X_DIFFERENCE;
    Bakuhatu_y2 = Bakuhatu_y + BAKUHATU_Y_DIFFERENCE;

    if (KeyState_Game[KEY_INPUT_SPACE] == 0) { // ゲーム開始してからスペースキーが押されていないとき

        // 車の種類選択

        if (Car_num > 2) {
            Car_num = 2;
        }
        if (Car_num < 0) {
            Car_num = 0;
        }


        if (KeyState_Game[KEY_INPUT_LEFT] == 1) {
            Car_num += 1;
        }
        if (KeyState_Game[KEY_INPUT_RIGHT] == 1) {
            Car_num -= 1;
        }


        if (Car_num == 0) {
            Car = 0;
            Car_y = CAR_Y0;
            Finger_y = FINGER_Y_CAR0;  // 車のサイズによって指の座標が変わる
        }
        if (Car_num == 1) {
            Car = 1;
            Car_y = CAR_Y1;
            Finger_y = FINGER_Y_CAR1;
        }
        if (Car_num == 2) {
            Car = 2;
            Car_y = CAR_Y2;
            Finger_y = FINGER_Y_CAR2;
        }
    }


    if (KeyState_Game[KEY_INPUT_SPACE] > 0 && Push == TRUE) { // ゲーム開始してからスペースキーを長押ししているとき

        Car_Select = FALSE;
        Gugugu = TRUE;


        // 長押ししているときの表示
        vibration++;
        if (vibration % 7 == 0) {
            Gugugu_y = 185;
        }
        else {
            Gugugu_y = 175;
        }

        // 車を引っ張る動き
        if (Car_x >= CAR_PULL_LIMIT) {
            Car_x -= 0.4;
            Finger_x -= 0.4;
            if (Car == 0) {
                Car_y = CAR_Y0 + 3;
                Finger_y = FINGER_Y_CAR0 + 4;
            }
            if (Car == 1) {
                Car_y = CAR_Y1 + 3;
                Finger_y = FINGER_Y_CAR1 + 4;
            }
            if (Car == 2) {
                Car_y = CAR_Y2 + 3;
                Finger_y = FINGER_Y_CAR2 + 4;
            }
        }
        else {
            Pull = FALSE;    // 限界まで引いたときそれ以上引かないようにする
        }

        // ゲージが限界まで行ったときの振動処理
        if (Gage <= 398) {
            Gage += 2;
        }
        else {
            vibration++;
            if (vibration % 9 == 0) {

                Gage = 398;


                // 車の種類ごとに座標を変える
                if (Car == 0) {
                    Car_y = CAR_Y0 + 4;
                    Finger_y = FINGER_Y_CAR0 + 2;
                }
                if (Car == 1) {
                    Car_y = CAR_Y1 + 4;
                    Finger_y = FINGER_Y_CAR1 + 2;
                }
                if (Car == 2) {
                    Car_y = CAR_Y2 + 4;
                    Finger_y = FINGER_Y_CAR2 + 2;
                }

            }
            else {

                Gage = 398 + 4;

                if (Car == 0) {
                    Car_y = CAR_Y0 - 4;
                    Finger_y = FINGER_Y_CAR0;
                }
                if (Car == 1) {
                    Car_y = CAR_Y1 - 4;
                    Finger_y = FINGER_Y_CAR1;
                }
                if (Car == 2) {
                    Car_y = CAR_Y2 - 4;
                    Finger_y = FINGER_Y_CAR2;
                }


            }
        }


        // キーを長押しした状態をそのままパワーに変換する
        if (Pull == TRUE) {
            Power = KeyState_Game[KEY_INPUT_SPACE];
        }

        // 限界まで引き続けたとき座標を相殺する
        if (Car_x <= CAR_PULL_LIMIT) {
            Pull = FALSE;
            Car_x += 0.4;

            Over++;
        }
        if (Finger_x <= FINGER_PULL_LIMIT) {
            Finger_x += 0.4;
        }

        if (Over / 60 == 3) {

            Bakuhatu_x = Car_x - 85;
            Dokkan_x = Car_x - 85;
            Dokkan_x2 = Dokkan_x + DOKKAN_W;

            Genkai = TRUE;
            End = TRUE;
            Out = TRUE;
        }
    }


    if (KeyState_Game[KEY_INPUT_SPACE] < 0 || Push == FALSE) { // スペースキーを長押しから離したとき
        Over = 0;
        Push = FALSE;
        Gugugu = FALSE;

        // スピードが0か爆弾に突撃したら終了
        if (Speed >= 0 && End == FALSE) {
            Haikei -= (int)Speed;
            vibration_Car = (int)Speed;
        }
        else {
            End = TRUE;
        }

        // 残り距離に応じて爆弾を近づける（パワー不足・ぴったりで間隔が変わる）
        if (Out == FALSE && Start == FALSE) {
            UpdateBombPosition();
        }

        // オーバーパワー：同じ進路で勢いは残ったまま目標を超えた＝爆弾へ突入して即衝突
        // （減速しきってから隙間を閉じるのではなく、スピードが残っている時点で爆発）
        if (Out == FALSE && Distance > Goal && Speed > 0) {
            Bomb_x = Car_x2;
            Bomb_x2 = Bomb_x + BOMB_X_DIFFERENCE;
            Bakuhatu_x = Car_x2 - BAKUHATU_X_DIFFERENCE * 0.5;
            Bakuhatu_x2 = Bakuhatu_x + BAKUHATU_X_DIFFERENCE;
            Dokkan_x = Car_x2 - DOKKAN_W / 2;
            Dokkan_x2 = Dokkan_x + DOKKAN_W;
            End = TRUE;
            Out = TRUE;
        }

        // 走っている感じを出すための振動処理
        if (vibration_Car % 2 == 1) {
            if (Car == 0) {
                Car_y = CAR_Y0 - 1;
            }
            if (Car == 1) {
                Car_y = CAR_Y1 - 1;
            }
            if (Car == 2) {
                Car_y = CAR_Y2 - 1;
            }
        }
        else {
            if (Car == 0) {
                Car_y = CAR_Y0 + 1;
            }
            if (Car == 1) {
                Car_y = CAR_Y1 + 1;
            }
            if (Car == 2) {
                Car_y = CAR_Y2 + 1;
            }
        }
        if (Haikei <= -639) {
            Haikei += 639;
        }

        // スタート判定
        if (Start == TRUE) {
            Speed = Power;
            Start = FALSE;
        }


        // 臨場感
        if (Camera == TRUE) {

            // 車を前に出し、加速する
            if (Car == 0) {
                Car_y = CAR_Y0 - 3;
            }
            if (Car == 1) {
                Car_y = CAR_Y1 - 3;
            }
            if (Car == 2) {
                Car_y = CAR_Y2 - 3;
            }
            Finger_x -= 8;

            // 徐々にカメラを追いつくように
            if (Car_x <= 190 && Camera == TRUE) {

                Pa = TRUE;
                Car_x += 2;

                if (Car_x <= 150) {
                    Car_x += 3;
                    if (Car_x <= 120) {
                        Car_x += 4;
                    }
                }
            }
            if (Car_x >= 190) {
                Camera = FALSE;
                Pa = FALSE;
            }
        }

        if (Finger_x <= -300) {
            Finger_x += 1;
        }


        if (Camera == FALSE) {
            if (Speed > 0) {
                Speed -= 0.1;
                Distance += 0.1;
                if (Speed > 20) {
                    Speed -= 0.5;
                    Distance += 0.5;
                    if (Speed > 50) {
                        Speed -= 1;
                        Distance += 1;
                    }
                }
            }
        }


    }
    if (Out == TRUE) {
        vibration++;
        if (vibration % 5 == 0) {
            vibration_Haikei = -2;
        }
        else {
            vibration_Haikei = 2;
        }
    }
}

// 描画
void Game_Draw() {

    if (End == TRUE) {
        DrawGraph(Haikei, vibration_Haikei, HaikeiHandle, TRUE);
        DrawGraph(Haikei + 639, vibration_Haikei, HaikeiHandle, TRUE);
        DrawGraph(Haikei, vibration_Haikei, tukueHandle, TRUE);
        DrawGraph(Haikei + 639, vibration_Haikei, tukueHandle, TRUE);

        if (Out == FALSE) {

            DrawExtendGraph((int)Bomb_x, Bomb_y, (int)Bomb_x2, Bomb_y2, BombHandle, TRUE);

            if (Car == 0) {
                DrawExtendGraph((int)Car_x, Car_y, (int)Car_x2, Car_y2, CarHandle_1, TRUE);
            }
            else if (Car == 1) {
                DrawExtendGraph((int)Car_x, Car_y, (int)Car_x2, Car_y2, CarHandle_2, TRUE);
            }
            else if (Car == 2) {
                DrawExtendGraph((int)Car_x, Car_y, (int)Car_x2, Car_y2, CarHandle_3, TRUE);
            }
            // 爆弾までの距離によって結果表示（見出し・差・操作を分ける）
            long gap = labs((long)(Goal - Distance));
            char detail[64];
            snprintf(detail, sizeof(detail), "差  %ld", gap);
            if (gap == 0) {
                DrawResultBoard("だいせいこう", GetColor(196, 82, 0), "ぴったり止まった");
            }
            else if (gap < 4) {
                DrawResultBoard("せいこう", GetColor(24, 122, 58), detail);
            }
            else if (gap < 10) {
                DrawResultBoard("おしいけど", GetColor(176, 96, 0), detail);
            }
            else {
                DrawResultBoard("まだまだ", GetColor(72, 64, 58), detail);
            }
        }
        else if (Genkai == TRUE) {
            DrawExtendGraph((int)Bakuhatu_x, vibration_Haikei + Bakuhatu_y, (int)Bakuhatu_x2, vibration_Haikei + Bakuhatu_y2, BakuhatuHandle, TRUE);
            DrawExtendGraph((int)Dokkan_x, vibration_Haikei + Dokkan_y, (int)Dokkan_x2, vibration_Haikei + Dokkan_y2, DokkanHandle, TRUE);

            DrawResultBoard("ひっぱりすぎ", GetColor(186, 28, 24), "限界まで引き続けた");
        }
        else {
            DrawExtendGraph((int)Bakuhatu_x, vibration_Haikei + Bakuhatu_y, (int)Bakuhatu_x2, vibration_Haikei + Bakuhatu_y2, BakuhatuHandle, TRUE);
            DrawExtendGraph((int)Dokkan_x, vibration_Haikei + Dokkan_y, (int)Dokkan_x2, vibration_Haikei + Dokkan_y2, DokkanHandle, TRUE);

            DrawResultBoard("ばくはつ", GetColor(186, 28, 24), "勢いが残ったまま当たった");
        }
    }
    if (End == FALSE) {
        DrawGraph(Haikei, 0, HaikeiHandle, TRUE);
        DrawGraph(Haikei + 639, 0, HaikeiHandle, TRUE);
        DrawGraph(Haikei, 0, tukueHandle, TRUE);
        DrawGraph(Haikei + 639, 0, tukueHandle, TRUE);
        if (Car == 0) {
            DrawExtendGraph((int)Car_x, Car_y, (int)Car_x2, Car_y2, CarHandle_1, TRUE);
        }
        else if (Car == 1) {
            DrawExtendGraph((int)Car_x, Car_y, (int)Car_x2, Car_y2, CarHandle_2, TRUE);
        }
        else if (Car == 2) {
            DrawExtendGraph((int)Car_x, Car_y, (int)Car_x2, Car_y2, CarHandle_3, TRUE);
        }

        DrawExtendGraph((int)Finger_x, Finger_y, (int)Finger_x2, Finger_y2, FingerHandle, TRUE);
        DrawExtendGraph((int)Bomb_x, Bomb_y, (int)Bomb_x2, Bomb_y2, BombHandle, TRUE);

        char status[64];
        snprintf(status, sizeof(status), "スピード  %d", (int)Speed);
        DrawPlateText(8, 8, status, FontHud, GetColor(40, 28, 18));
        int statusStep = (FontHud == -1) ? 22 : GetFontSizeToHandle(FontHud) + 16;
        snprintf(status, sizeof(status), "パワー  %d", (int)Power);
        DrawPlateText(8, 8 + statusStep, status, FontHud, GetColor(40, 28, 18));

        snprintf(status, sizeof(status), "爆弾まで  %d", (int)Goal - (int)Distance);
        int distW = (FontHud == -1) ? 160 : TextWidth(status, FontHud) + 20;
        DrawPlateText(640 - distW - 8, 8, status, FontHud, GetColor(40, 28, 18));


        // 車選択時
        if (Car_Select == TRUE) {
            DrawCenteredPlateText(96, "SPACEキー長押しで引っ張る", FontHud, GetColor(40, 28, 18));
            int hintStep = (FontHud == -1) ? 28 : GetFontSizeToHandle(FontHud) + 18;
            DrawCenteredPlateText(96 + hintStep, "←  →  キーで車を選ぶ", FontHud, GetColor(40, 28, 18));
        }

        // 限界まで引いた場合
        if (Over > 0) {
            snprintf(status, sizeof(status), "のこり  %d", 3 - (Over / 60));
            int remainW = (FontResult == -1) ? 120 : TextWidth(status, FontResult) + 20;
            DrawPlateText(640 - remainW - 8, 390, status, FontResult, GetColor(186, 28, 24));
        }

        // 引っ張り中の文字
        if (Gugugu == TRUE) {
            DrawExtendGraph((int)Gugugu_x, Gugugu_y, (int)Gugugu_x2, Gugugu_y2, GuguguHandle, TRUE);
            DrawBox(110, 92, 508, 112, GetColor(72, 48, 24), TRUE);
            int gageRight = 114 + (int)Gage * 390 / 402;
            if (gageRight > 504) {
                gageRight = 504;
            }
            DrawBox(114, 96, gageRight, 108, GetColor(238, 120, 0), TRUE);
        }

        // ぱっの文字
        if (Pa == TRUE) {
            DrawExtendGraph((int)Pa_x, Pa_y, (int)Pa_x2, (int)Pa_y2, PaHandle, TRUE);
        }

    }
}
