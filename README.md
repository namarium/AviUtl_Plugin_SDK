# AviUtl Plugin SDK
Single-file AviUtl Plugin SDK for C++.  

This library is a rewrite of "AviUtl Plugin SDK for ver0.99m" provided by "Kenkun(ＫＥＮくん)".  
(Original http://spring-fragrance.mints.ne.jp/aviutl/)  

## How to use
Just include `AviUtl.h` in your ".vcxproj".  
However, Plug-ins other than .aul format (.auf / .aui / .auo / .auc) require an interface.  
In that case, define the interface by referring to `sample.def` and `sample.h`.  

## License
Released under the MIT license. (Compliant with AviUtlPluginSDK)

----

## 概要
AviUtl の Plugin を作成するための SDK です。  
`ＫＥＮくん` 様が配布されている `AviUtl Plugin SDK` を元に、
ヘッダーオンリーで使用できるよう、独自に書き直したものです。  

つまり、このリポジトリで配布されるSDKは、  
オリジナルの `AviUtl Plugin SDK` とは無関係のサードパーティとなります。

## ファイルの説明
```
AviUtl.h
sample.def
sample.h
```
- AviUtl.h  
    プラグインSDK本体です。  
    オリジナルの「AviUtl Plugin SDK」とは、宣言に互換性はありません。  
    プロジェクトから、このファイルを参照するだけで使用できます。

- sample.def  
    定義ファイルのサンプルです。

- sample.h  
    インターフェース宣言のサンプルです。

## 動作環境
Visual Studio 2015 以上の環境を想定しています。

### 文字セット
常に「Unicode 文字セット」を使用します。  
AviUtlは、「マルチバイト文字セット」をサポートしていません。(自前で確認した際、文字化けしたため)  

### 呼び出し規約
常に stdcall を使用します。  
`sample.h` と同様に、インターフェースを `__stdcall` で宣言しておけば、  
プロジェクトの設定を気にする必要はありません。

### アーキテクチャ
x86 構成（Win32）でビルドしてください。  
この制限は、AviUtl側が 64bit 対応されない限り変更されることはありません。

### LAA オプション (LARGEADDRESSAWARE)
有効化しておくことを推奨します。  
AviUtl version 1.10 以降では、常に LAA が有効になっています。  
それ以前のバージョンでも、LAA はシステム設定から使用可能です。  
つまり、AviUtl側の設定に応じて使い分けられるので、常に有効化しておいても問題無いという話です。

## ライセンス
オリジナルの AviUtlPluginSDK に準拠して、`MIT ライセンスで配布します`。  

# リンク
- AviUtl
    - http://spring-fragrance.mints.ne.jp/aviutl/
