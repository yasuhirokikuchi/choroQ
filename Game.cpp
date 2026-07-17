#include "Game.h"
#include "SceneMgr.h"
#include "DxLib.h"
#include <stdlib.h>

#define HAIKEI_SIZE_W 640          // 背景の幅

#define CAR_X_DIFFERENCE 135    // 車のX座標の差
#define CAR_Y_DIFFERENCE 205    // 車のY座標の差

#define FINGER_X_DIFFERENCE 270    // 指のX座標の差
#define FINGER_Y_DIFFERENCE 140    // 指のY座標の差

#define BOMB_X_DIFFERENCE 50    // 爆弾のX座標の差
#define BOMB_Y_DIFFERENCE 60    // 爆弾のY座標の差

#define BAKUHATU_X_DIFFERENCE 300    // 爆発のX座標の差
#define BAKUHATU_Y_DIFFERENCE 300    // 爆発のY座標の差


// キーの状態
static int KeyState_Game[256] = { 0 };


// 車の座標＋画像ファイル用変数
static double Car_x, Car_x2;
static int  Car_y, Car_y2;
static int CarHandle_1, CarHandle_2, CarHandle_3;


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


    // 車の初期座標
    Car_x = 85;
    Car_y = 307;
    Car_x2 = Car_x + CAR_X_DIFFERENCE;
    Car_y2 = Car_y + CAR_Y_DIFFERENCE;


    // 指の初期座標
    Finger_x = -115;
    if (Car == 0) {
        Finger_y = 227;
    }
    else if (Car == 1) {
        Finger_y = 243;
    }
    else {
        Finger_y = 229;
    }
    Finger_x2 = Finger_x + FINGER_X_DIFFERENCE;
    Finger_y2 = Finger_y + FINGER_Y_DIFFERENCE;


    // 爆弾の初期座標
    Bomb_x = 690;
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
    Gugugu_x2 = Gugugu_x + 280;
    Gugugu_y2 = Gugugu_y + 120;


    // ぱっの文字の座標
    Pa_x = 100;
    Pa_y = 180;
    Pa_x2 = Pa_x + 250;
    Pa_y2 = Pa_y + 130;


    // どっかーんの文字の座標
    Dokkan_x = 120;
    Dokkan_y = 80;
    Dokkan_x2 = Dokkan_x + 300;
    Dokkan_y2 = Dokkan_y + 200;


    // 振動
    vibration_Haikei = 0;


    // 画像のロード
    CarHandle_1 = LoadGraph("images/seisaku/car.png");
    CarHandle_2 = LoadGraph("images/seisaku/car2.png");
    CarHandle_3 = LoadGraph("images/seisaku/car3.png");
    FingerHandle = LoadGraph("images/seisaku/finger.png");

    HaikeiHandle = LoadGraph("images/seisaku/haikei.png");
    tukueHandle = LoadGraph("images/seisaku/tukue.png");
    BombHandle = LoadGraph("images/seisaku/bomb.png");
    BakuhatuHandle = LoadGraph("images/seisaku/bakuhatu.png");

    GuguguHandle = LoadGraph("images/seisaku/gugugu.png");
    PaHandle = LoadGraph("images/seisaku/pa.png");
    DokkanHandle = LoadGraph("images/seisaku/dokkan.png");

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

    Car_x2 = Car_x + CAR_X_DIFFERENCE;
    Car_y2 = Car_y + CAR_Y_DIFFERENCE;

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
            Car_y = 307;
            Finger_y = 227;           // 車のサイズによって指の座標が変わる
        }
        if (Car_num == 1) {
            Car = 1;
            Car_y = 307;
            Finger_y = 243;
        }
        if (Car_num == 2) {
            Car = 2;
            Car_y = 307 + 10;
            Finger_y = 229;
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
        if (Car_x >= 5) {
            Car_x -= 0.4;
            Finger_x -= 0.4;
            if (Car == 0) {
                Car_y = 307 + 3;
                Finger_y = 227 + 4;
            }
            if (Car == 1) {
                Car_y = 307 + 3;
                Finger_y = 243 + 4;
            }
            if (Car == 2) {
                Car_y = 307 + 10 + 3;
                Finger_y = 229 + 4;
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
                    Car_y = 307 + 4;
                    Finger_y = 227 + 2;
                }
                if (Car == 1) {
                    Car_y = 307 + 4;
                    Finger_y = 243 + 2;
                }
                if (Car == 2) {
                    Car_y = 307 + 10 + 4;
                    Finger_y = 229 + 2;
                }

            }
            else {

                Gage = 398 + 4;

                if (Car == 0) {
                    Car_y = 307 - 4;
                    Finger_y = 227;
                }
                if (Car == 1) {
                    Car_y = 307 - 4;
                    Finger_y = 243;
                }
                if (Car == 2) {
                    Car_y = 307 - 4 + 10;
                    Finger_y = 229;
                }


            }
        }


        // キーを長押しした状態をそのままパワーに変換する
        if (Pull == TRUE) {
            Power = KeyState_Game[KEY_INPUT_SPACE];
        }

        // 限界まで引き続けたとき座標を相殺する
        if (Car_x <= 5) {
            Pull = FALSE;
            Car_x += 0.4;

            Over++;
        }
        if (Finger_x <= -195) {
            Finger_x += 0.4;
        }

        if (Over / 60 == 3) {

            Bakuhatu_x = 120 - 200;
            Dokkan_x = 120 - 200;

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

        // 必殺技で爆弾までの距離を調整
        if (Bomb_x - Car_x2 < 0 || Goal - Distance < -1) {
            End = TRUE;
            Out = TRUE;
        }

        // 爆弾を近づける
        if (((Goal - 9 <= Distance) && Speed > 0) && Out == FALSE) {
            Bomb_x -= (int)Speed;
        }

        // 走っている感じを出すための振動処理
        if (vibration_Car % 2 == 1) {
            if (Car == 0) {
                Car_y = 306;
            }
            if (Car == 1) {
                Car_y = 306;
            }
            if (Car == 2) {
                Car_y = 306 + 10;
            }
        }
        else {
            if (Car == 0) {
                Car_y = 308;
            }
            if (Car == 1) {
                Car_y = 308;
            }
            if (Car == 2) {
                Car_y = 308 + 10;
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
                Car_y = 304;
            }
            if (Car == 1) {
                Car_y = 304;
            }
            if (Car == 2) {
                Car_y = 304 + 10;
            }
            Finger_y = 225;
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
            // 爆弾までの距離によって結果表示
            if (labs(Goal - Distance) == 0) {
                DrawFormatString(255, 50, GetColor(0, 0, 0), "だいせいこう\n\nRキーでリスタート\nEscでメニューへ");
            }
            else if (labs(Goal - Distance) >= 1 && labs(Goal - Distance) < 4) {
                DrawFormatString(255, 50, GetColor(0, 0, 0), "せいこう\n\nRキーでリスタート\nEscでメニューへ");
            }
            else if (labs(Goal - Distance) >= 4 && labs(Goal - Distance) < 10) {
                DrawFormatString(255, 50, GetColor(0, 0, 0), "おしいけど\n\nRキーでリスタート\nEscでメニューへ");
            }
            else if (labs(Goal - Distance) >= 10) {
                DrawFormatString(255, 50, GetColor(0, 0, 0), "まだまだ\n\nRキーでリスタート\nEscでメニューへ");
            }
        }
        if (Out == TRUE) {

            DrawExtendGraph((int)Bakuhatu_x, vibration_Haikei + Bakuhatu_y, (int)Bakuhatu_x2, vibration_Haikei + Bakuhatu_y2, BakuhatuHandle, TRUE);
            DrawExtendGraph((int)Dokkan_x, vibration_Haikei + Dokkan_y, (int)Dokkan_x2, vibration_Haikei + Dokkan_y2, DokkanHandle, TRUE);

            DrawFormatString(255, 50, GetColor(0, 0, 0), "ばくはつ\n\nRキーでリスタート\nEscでメニューへ");
        }
        if (Out == TRUE && Genkai == TRUE) {
            DrawExtendGraph((int)Bakuhatu_x, vibration_Haikei + Bakuhatu_y, (int)Bakuhatu_x2, vibration_Haikei + Bakuhatu_y2, BakuhatuHandle, TRUE);
            DrawExtendGraph((int)Dokkan_x, vibration_Haikei + Dokkan_y, (int)Dokkan_x2, vibration_Haikei + Dokkan_y2, DokkanHandle, TRUE);

            DrawFormatString(255, 50, GetColor(0, 0, 0), "ひっぱりすぎてばくはつ\n\nRキーでリスタート\nEscでメニューへ");
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
        DrawFormatString(0, 0, GetColor(0, 0, 0), "speed:%d", (int)Speed);
        DrawFormatString(0, 15, GetColor(0, 0, 0), "power:%d", (int)Power);
        DrawFormatString(240, 50, GetColor(0, 0, 0), "爆弾まで:%d", (int)Goal - (int)Distance);


        // 車選択時
        if (Car_Select == TRUE) {
            DrawFormatString(200, 75, GetColor(0, 0, 0), "SPACEキーを押し込んで引っ張る");
            DrawFormatString(215, 100, GetColor(0, 0, 0), "←　→キーで車を選択");
        }

        // 限界まで引いた場合
        if (Over > 0) {
            DrawFormatString(420, 450, GetColor(0, 0, 0), "のこり:%d", 3 - (Over / 60));
        }

        // 引っ張り中の文字
        if (Gugugu == TRUE) {
            DrawExtendGraph((int)Gugugu_x, Gugugu_y, (int)Gugugu_x2, Gugugu_y2, GuguguHandle, TRUE);
            DrawBox(110, 20, 110 + 398, 40, GetColor(0, 0, 0), TRUE);
            DrawBox(110, 20, 110 + (int)Gage, 40, GetColor(238, 120, 0), TRUE);
        }

        // ぱっの文字
        if (Pa == TRUE) {
            DrawExtendGraph((int)Pa_x, Pa_y, (int)Pa_x2, (int)Pa_y2, PaHandle, TRUE);
        }

    }
}
