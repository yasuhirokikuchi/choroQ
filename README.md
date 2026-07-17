# choroQ（寸止め）

机の上でミニカーを引っ張り、爆弾の手前で止める DxLib 製ミニゲームです。

## 必要環境

- Visual Studio 2022（C++ デスクトップ開発ワークロード）
- [DxLib](https://dxlib.xsrv.jp/)（Visual Studio 用）

## DxLib の配置

1. DxLib を展開する（例: `C:\DxLib`）
2. 展開先に `DxLib.h` と `DxLib.lib` / `DxGuid.lib` があることを確認する
3. 別パスにする場合は、環境変数 `DXLIB_DIR` にそのフォルダを設定する

プロジェクトは既定で `C:\DxLib`、または `DXLIB_DIR` を参照します。

## ビルドと実行

1. `choroQ.sln` を Visual Studio で開く
2. 構成は `Debug | x64` または `Debug | Win32`（DxLib のビット数に合わせる）
3. ビルドして実行

ビルド後、`images/` は出力フォルダへ自動コピーされます。作業ディレクトリは出力フォルダです。

## 操作

| キー | 内容 |
|------|------|
| E | メニューからゲーム開始 |
| Z | 終了 |
| ← → | 車種選択（3種） |
| Space 長押し | 引っ張る（パワー蓄積） |
| Space 離す | 発射 |
| R | リスタート |
| Esc | メニューへ戻る |

## 画像の差し替え

現在 `images/seisaku/` にはプレースホルダ PNG が入っています。同名で差し替えれば見た目だけ復元できます。

必要なファイル:

- `sundome.png` … タイトル
- `car.png` / `car2.png` / `car3.png` … 車
- `finger.png` … 指
- `haikei.png` … 背景（幅約 639、横ループ）
- `tukue.png` … 机レイヤー
- `bomb.png` … 爆弾
- `bakuhatu.png` … 爆発
- `gugugu.png` / `pa.png` / `dokkan.png` … 演出文字

## ソース構成

- `Main.cpp` … エントリ / DxLib 初期化（ウィンドウモード）
- `SceneMgr` … メニュー ↔ ゲーム切替
- `Menu` … タイトル画面
- `Game` … 寸止め本編
