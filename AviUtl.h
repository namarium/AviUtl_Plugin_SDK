/// Copyright (c) 2020 namarium
/// Licensed under the MIT License.
/// https://github.com/namarium/AviUtl_Plugin_SDK/blob/master/LICENSE

/// 
/// AviUtl Plugin SDK  for ver0.99m　の仕様をもとに作成しています。
/// 

#pragma once
static_assert(UNICODE, "Error: UNICODE hasn't been defined yet.");

#include <array>        // std::array
#include <type_traits>  // std::is_pod, std::underlying_type

/// <summary>
/// enum class の演算子定義マクロ
/// </summary>
/// <param name="name">enum class の定義名</param>
#define AU_DECLARE_ENUMCLASS_OPERATOR(name) \
	constexpr name operator ~ (name op) {return static_cast<name>(~static_cast<std::underlying_type<name>::type>(op));} \
	constexpr name operator & (name l, name r) {using ty = std::underlying_type<name>::type; return static_cast<name>(static_cast<ty>(l) & static_cast<ty>(r));} \
	name& operator&=(name& l, name r) {using ty = std::underlying_type<name>::type; l = static_cast<name>(static_cast<ty>(l) & static_cast<ty>(r)); return l;} \
	constexpr name operator ^ (name l, name r) {using ty = std::underlying_type<name>::type; return static_cast<name>(static_cast<ty>(l) ^ static_cast<ty>(r));} \
	name& operator^=(name& l, name r) {using ty = std::underlying_type<name>::type; l = static_cast<name>(static_cast<ty>(l) ^ static_cast<ty>(r)); return l;} \
	constexpr name operator | (name l, name r) {using ty = std::underlying_type<name>::type; return static_cast<name>(static_cast<ty>(l) | static_cast<ty>(r));} \
	name& operator|=(name& l, name r) {using ty = std::underlying_type<name>::type; l = static_cast<name>(static_cast<ty>(l) | static_cast<ty>(r)); return l;}

/// <summary>
/// 定数文字列 の定義マクロ
/// </summary>
/// <param name="name">取得関数の定義名</param>
/// <param name="str">定数文字列</param>
#define AU_DECLARE_CONSTANT_STRING(name, str) \
	enum {name##ASize=sizeof(str), name##Size=sizeof(L##str)}; \
	inline static const char* name##A = str; \
	inline static const wchar_t* name = L##str;


/// <summary>
/// AviUtl Plugin SDK
/// </summary>
namespace AviUtl
{
	/// <summary>
	/// 定数定義
	/// </summary>
	namespace Constants
	{
		/// <summary>
		/// ウィンドウクラス名
		/// </summary>
		namespace WindowClassName
		{
			/// <summary>
			/// ウィンドウ
			/// </summary>
			AU_DECLARE_CONSTANT_STRING(Window, "AviUtl");

			/// <summary>
			/// ボタン
			/// </summary>
			AU_DECLARE_CONSTANT_STRING(Button, "AviUtlButton");

			/// <summary>
			/// 拡張編集
			/// </summary>
			AU_DECLARE_CONSTANT_STRING(ExtendedEditorWindow, "ExtendedFilterClass");
		}

		/// <summary>
		/// ハッシュ情報
		/// </summary>
		namespace FileHash
		{
			/// <summary>
			/// aviutl.exe のバージョン番号とハッシュ(SHA1)のペア
			/// </summary>
			std::pair<const char*, const char*> AviUtl[] =
			{
				/// <summary>
				/// version0.99k2
				/// </summary>
				std::make_pair("0.99k2", "52c241710a1cdbc7efdbdc5f50807eddae049c87"),

				/// <summary>
				/// version0.99l
				/// </summary>
				std::make_pair("0.99l", "fc9c953aa0f8d618df3d25ec5df761849f92c987"),

				/// <summary>
				/// version0.99m
				/// </summary>
				std::make_pair("0.99m", "f3530f8da99f0cb49787663c3e28ea39d3efb1a3"),

				/// <summary>
				/// version1.00
				/// </summary>
				std::make_pair("1.00", "11994746b8470e58c3745a66c01dbaeb2f48e20c"),

				/// <summary>
				/// version1.10
				/// </summary>
				std::make_pair("1.10", "473c0479b828a1ff663473223eb9fc9f59edf856")
			};
		}
	}

#ifndef _WIN64 // x86環境のみ利用可能
static_assert(sizeof(void*) == 4, "Error: Architecture not targeted for build.");

	/// <summary>
	/// Filter プラグイン
	/// </summary>
	namespace Filter
	{
		// 前方宣言
		union Pixel_YC;
		union Pixel;
		struct FilterProcInfo;
		struct FrameStatus;
		struct FileInfo;
		struct SystemInfo;
		struct CallbackFunctionSet;
		struct FilterPluginTable;

		/// <summary>
		/// YC共用体（YCbCr色空間）
		/// <para>画素データは範囲外に出ていることがあります</para>
		/// <para>また、範囲内に収めなくてもかまいません</para>
		/// </summary>
		/// <remarks>
		/// <para>データ範囲外となる場合は、最小値/最大値にクランプされた値として認識されるようです.</para>
		/// </remarks>
		union Pixel_YC final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 6 };

			struct {
				/// <summary>
				/// 輝度
				/// <para>データ範囲：0 ～ 4096</para>
				/// </summary>
				short Y;

				/// <summary>
				/// 色差(青)
				/// <para>値の範囲：-2048 ～ 2048</para>
				/// </summary>
				short Cb;

				/// <summary>
				/// 色差(赤)
				/// <para>値の範囲：-2048 ～ 2048</para>
				/// </summary>
				short Cr;
			};

			/// <summary>
			/// YCbCr配列
			/// </summary>
			std::array<short, 3> YCbCr;
		};

		/// <summary>
		/// Pixel共用体（RGBカラーモデル）
		/// </summary>
		union Pixel final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 3 };

			struct {
				/// <summary>
				/// 青色
				/// </summary>
				unsigned char B;

				/// <summary>
				/// 緑色
				/// </summary>
				unsigned char G;

				/// <summary>
				/// 赤色
				/// </summary>
				unsigned char R;
			};

			/// <summary>
			/// BGR配列
			/// </summary>
			std::array<unsigned char, 3> BGR;
		};

		/// <summary>
		/// フィルタPROC用構造体
		/// <para>※ インターレース解除フィルタ時は、pYC_Edit に初期画像データが入っていません。</para>
		/// <para>※ インターレース解除フィルタ時は、pYC_Edit, pYC_Temp , Width, Height を変更できません。</para>
		/// </summary>
		struct FilterProcInfo final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 104 };

			/// <summary>
			/// フラグ情報定数
			/// </summary>
			enum class InfoFlag : int {
				/// <summary>
				/// フィールドオーダーを標準と逆に扱う ( 標準はボトム->トップになっています )
				/// </summary>
				InvertFieldOrder = 0x00010000,

				/// <summary>
				/// 解除方法を反転する ( インターレース解除フィルタのみ )
				/// </summary>
				InvertInterlace = 0x00020000,
			};

			/// <summary>
			/// フラグ情報
			/// </summary>
			InfoFlag Flag;

			/// <summary>
			/// 画像データへのポインタ ( pYC_EditとpYC_Tempは入れ替えられます )
			/// </summary>
			Pixel_YC* pYC_Edit;

			/// <summary>
			/// テンポラリ領域へのポインタ
			/// </summary>
			Pixel_YC* pYC_Temp;

			struct {
				/// <summary>
				/// 現在の画像の幅 ( 画像サイズは変更出来ます )
				/// </summary>
				int Width;

				/// <summary>
				/// 現在の画像の高さ ( 画像サイズは変更出来ます )
				/// </summary>
				int Height;
			};

			struct {
				/// <summary>
				/// 画像領域の幅
				/// </summary>
				int Width_Max;

				/// <summary>
				/// 画像領域の高さ
				/// </summary>
				int Height_Max;
			};

			/// <summary>
			/// 現在のフレーム番号( 番号は0から )
			/// </summary>
			int Frame;

			/// <summary>
			/// 総フレーム数
			/// </summary>
			int Frame_Total;

			struct {
				/// <summary>
				/// 元の画像の幅
				/// </summary>
				int Width_Original;

				/// <summary>
				/// 元の画像の高さ
				/// </summary>
				int Height_Original;
			};

			/// <summary>
			/// オーディオデータへのポインタ ( オーディオフィルタの時のみ )
			/// <para>オーディオ形式はPCM16bitです ( 1サンプルは mono = 2byte , stereo = 4byte )</para>
			/// </summary>
			short* pAudio;

			/// <summary>
			/// オーディオサンプルの総数
			/// </summary>
			int Audio_Total;

			/// <summary>
			/// オーディオチャンネル数
			/// </summary>
			int Audio_Channel;

			/// <summary>
			/// （現在は使用されていません）
			/// </summary>
			Pixel* pPixel;

			/// <summary>
			/// エディットハンドル
			/// </summary>
			void* Edit_Handle;

			/// <summary>
			/// 画像領域の画素のバイトサイズ
			/// </summary>
			int YC_Size;

			/// <summary>
			/// 画像領域の幅のバイトサイズ
			/// </summary>
			int Line_Size;

			/// <summary>
			/// 予約領域
			/// </summary>
			std::array<int, 8> Reserved;
		};

		/// <summary>
		/// フレームステータス構造体
		/// </summary>
		struct FrameStatus final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 64 };

			/// <summary>
			/// インターレース情報定数
			/// </summary>
			enum class InterlaceType : int {
				/// <summary>
				/// 標準
				/// </summary>
				Normal,

				/// <summary>
				/// 反転
				/// </summary>
				Reverse,

				/// <summary>
				/// 奇数
				/// </summary>
				Odd,

				/// <summary>
				/// 偶数
				/// </summary>
				Even,

				/// <summary>
				/// 二重化
				/// </summary>
				Mix,

				/// <summary>
				/// 自動
				/// </summary>
				Auto,
			};

			/// <summary>
			/// 編集フラグ定数
			/// </summary>
			enum class EditFlag : int {
				/// <summary>
				/// キーフレーム
				/// </summary>
				KeyFrame = 1,

				/// <summary>
				/// マークフレーム
				/// </summary>
				MarkFrame = 2,

				/// <summary>
				/// 優先間引きフレーム
				/// </summary>
				DelFrame = 4,

				/// <summary>
				/// コピーフレーム
				/// </summary>
				NullFrame = 8,
			};

			/// <summary>
			/// 実際の映像データ番号
			/// </summary>
			int Video;

			/// <summary>
			/// 実際の音声データ番号
			/// </summary>
			int Audio;

			/// <summary>
			/// フレームのインターレース
			/// </summary>
			InterlaceType Interlace;

			/// <summary>
			/// （現在は使用されていません）
			/// </summary>
			int Index_24FPS;

			/// <summary>
			/// フレームのプロファイル環境の番号
			/// </summary>
			int Config;

			/// <summary>
			/// フレームの圧縮設定の番号
			/// </summary>
			int VCM;

			/// <summary>
			/// 編集フラグ
			/// </summary>
			EditFlag Edit_Flag;

			/// <summary>
			/// 予約領域
			/// </summary>
			std::array<int, 9> Reserved;
		};

		/// <summary>
		/// ファイル情報構造体
		/// </summary>
		struct FileInfo final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 64 };

			/// <summary>
			/// ファイル情報フラグ定数
			/// </summary>
			enum class FileInfoFlag : int {
				/// <summary>
				/// 映像が存在する
				/// </summary>
				Video = 1,

				/// <summary>
				/// 音声が存在する
				/// </summary>
				Audio = 2,
			};

			/// <summary>
			/// フラグ情報
			/// </summary>
			FileInfoFlag Flag;

			/// <summary>
			/// ファイル名
			/// <para>( avi_file_open()ではNULLになります )</para>
			/// </summary>
			char* pName;

			struct {
				/// <summary>
				/// 元の幅
				/// </summary>
				int Width;

				/// <summary>
				/// 元の高さ
				/// </summary>
				int Height;
			};

			/// <summary>
			/// フレームレート
			/// </summary>
			int Video_Rate;

			/// <summary>
			/// フレームレート
			/// </summary>
			int Video_Scale;

			/// <summary>
			/// 音声サンプリングレート
			/// </summary>
			int Audio_Rate;

			/// <summary>
			/// 音声チャンネル数
			/// </summary>
			int Audio_Channel;

			/// <summary>
			/// 総フレーム数
			/// </summary>
			int Frame_Total;

			/// <summary>
			/// ビデオ展開形式
			/// </summary>
			unsigned int Video_DecodeFormat;

			/// <summary>
			/// ビデオ展開形式のビット数
			/// </summary>
			int Video_DecodeBit;

			/// <summary>
			/// 音声の総サンプル数
			/// <para> ( avi_file_open()の時のみ設定されます )</para>
			/// </summary>
			int Audio_Total;

			/// <summary>
			/// 予約領域
			/// </summary>
			std::array<int, 4> Reserved;
		};

		/// <summary>
		/// システムインフォメーション構造体
		/// </summary>
		struct SystemInfo final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 76 };

			/// <summary>
			/// システムフラグ定数
			/// </summary>
			enum class SystemInfoFlag : int {
				/// <summary>
				/// 編集中
				/// </summary>
				Edit = 1,

				/// <summary>
				/// VFAPI動作時
				/// </summary>
				VFAPI = 2,

				/// <summary>
				/// SSE使用
				/// </summary>
				UseSSE = 4,

				/// <summary>
				/// SSE2使用
				/// </summary>
				UseSSE2 = 8,
			};

			/// <summary>
			/// システムフラグ
			/// </summary>
			SystemInfoFlag Flag;

			/// <summary>
			/// バージョン情報
			/// </summary>
			char* Info;

			/// <summary>
			/// 登録されてるフィルタの数
			/// </summary>
			int Frame_Total;

			struct {
				/// <summary>
				/// 編集出来る最小画像の幅
				/// </summary>
				int Width_Min;

				/// <summary>
				/// 編集出来る最小画像の高さ
				/// </summary>
				int Height_Min;
			};

			struct {
				/// <summary>
				/// 編集出来る最大画像の幅
				/// </summary>
				int Width_Max;

				/// <summary>
				/// 編集出来る最大画像の高さ
				/// </summary>
				int Height_Max;
			};

			/// <summary>
			/// 編集出来る最大フレーム数
			/// </summary>
			int Frame_Max;

			/// <summary>
			/// 編集ファイル名 (ファイル名が決まっていない時は何も入っていません)
			/// </summary>
			char* Edit_Name;

			/// <summary>
			/// プロジェクトファイル名 (ファイル名が決まっていない時は何も入っていません)
			/// </summary>
			char* pProject_Name;

			/// <summary>
			/// 出力ファイル名 (ファイル名が決まっていない時は何も入っていません)
			/// </summary>
			char* pOutput_Name;

			struct {
				/// <summary>
				/// 編集用画像領域の幅
				/// </summary>
				int Width_VRAM;

				/// <summary>
				/// 編集用画像領域の高さ
				/// </summary>
				int Height_VRAM;
			};

			/// <summary>
			/// 編集用画像領域の画素のバイト数
			/// </summary>
			int YC_VRAM_Size;

			/// <summary>
			/// 編集用画像領域の幅のバイト数
			/// </summary>
			int Line_VRAM_Size;

			/// <summary>
			/// フィルタ設定ウィンドウで使用しているフォントのハンドル
			/// </summary>
			void* hFont;

			/// <summary>
			/// ビルド番号 (新しいバージョンになるほど大きな値になります)
			/// </summary>
			int Build;

			/// <summary>
			/// 予約領域
			/// </summary>
			std::array<int, 2> Reserved;
		};

		/// <summary>
		/// マルチスレッド関数用の定義
		/// </summary>
		/// <param name="threadId">スレッド番号 ( 0 ～ thread_num-1 )</param>
		/// <param name="threadNum">スレッド数 ( 1 ～ )</param>
		/// <param name="pParam1">汎用パラメータ</param>
		/// <param name="pParam2">汎用パラメータ</param>
		using MultiThread_Func = void(*)(int threadId, int threadNum, void* pParam1, void* pParam2);

		/// <summary>
		/// AVI入力ファイルハンドル
		/// </summary>
		using AviFileHandle = void*;

		/// <summary>
		/// 外部関数構造体
		/// </summary>
		struct CallbackFunctionSet final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 320 };

			/// <summary>
			/// AVIファイルの読み込みフラグ定数
			/// </summary>
			enum class AviFileOpenFlag : int {
				/// <summary>
				/// ビデオのみ読み込むようにします
				/// </summary>
				Video_Only = 16,

				/// <summary>
				/// オーディオのみ読み込むようにします
				/// </summary>
				Audio_Only = 32,

				/// <summary>
				/// YUY2 で展開するようにします
				/// </summary>
				Only_YUY2 = 0x10000,

				/// <summary>
				/// RGB24で展開するようにします
				/// </summary>
				Only_RGB24 = 0x20000,

				/// <summary>
				/// RGB32で展開するようにします
				/// </summary>
				Only_RGB32 = 0x40000,
			};

			/// <summary>
			/// フィルターの種類
			/// </summary>
			enum class GetAviFileFilterType : int {
				/// <summary>
				/// ビデオ
				/// </summary>
				Video,

				/// <summary>
				/// オーディオ
				/// </summary>
				Audio,
			};

			/// <summary>
			/// ステータスの種類
			/// </summary>
			enum class FrameStatusType : int {
				/// <summary>
				/// 編集フラグ
				/// </summary>
				EditFlag,

				/// <summary>
				/// インターレース
				/// </summary>
				Interlace,
			};

			/// <summary>
			/// メニューアイテム追加フラグ定数
			/// </summary>
			enum class AddMenuItemFlag : int {
				/// <summary>
				/// 標準のショートカットキーをSHIFT+キーにする
				/// </summary>
				Key_Shift = 1,

				/// <summary>
				/// 標準のショートカットキーをCTRL+キーにする
				/// </summary>
				Key_Ctrl = 2,

				/// <summary>
				/// 標準のショートカットキーをALT+キーにする
				/// </summary>
				Key_Alt = 4,
			};

			/// <summary>
			/// 編集データオープンのフラグ定数
			/// </summary>
			enum class EditOpenFlag : int {
				/// <summary>
				/// 追加読み込みをします
				/// </summary>
				Add = 2,

				/// <summary>
				/// 音声読み込みをします
				/// </summary>
				Audio = 16,

				/// <summary>
				/// プロジェクトファイルを開きます
				/// </summary>
				Project = 512,

				/// <summary>
				/// 読み込みダイアログを表示します
				/// </summary>
				Dialog = 65536,
			};

			/// <summary>
			/// 編集データ出力のフラグ定数
			/// </summary>
			enum class EditOutputFlag : int {
				/// <summary>
				/// 出力ダイアログを表示しません
				/// </summary>
				No_Dialog = 2,

				/// <summary>
				/// WAV出力をします
				/// </summary>
				Wav = 4,
			};

			/// <summary>
			/// 指定したフレームのAVIファイル上でのオフセット分移動した、フレームの画像データのポインタを取得します
			/// <para>データはフィルタ前のものです</para>
			/// <para>※出来るだけget_ycp_source_cache()の方を使用するようにしてください</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="offset">フレームからのオフセット</param>
			/// <returns>
			/// 画像データへのポインタ (NULLなら失敗)
			/// <para>画像データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効</para>
			/// </returns>
			using GetYC_Offset_Func = void(*)(void* pEdit, int frameNum, int offset); GetYC_Offset_Func GetYC_Offset;

			/// <summary>
			/// 指定したフレームの画像データのポインタを取得します
			/// <para>データはフィルタ前のものです</para>
			/// <para>※出来るだけget_ycp_source_cache()の方を使用するようにしてください</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 画像データへのポインタ (NULLなら失敗)
			/// <para>画像データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効</para>
			/// </returns>
			using GetYC_Func = void(*)(void* pEdit, int frameNum); GetYC_Func GetYC;

			/// <summary>
			/// 指定したフレームのDIB形式(RGB24bit)の画像データのポインタを取得します
			/// <para>データはフィルタ前のものです</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// DIB形式データへのポインタ (NULLなら失敗)
			/// <para>画像データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効</para>
			/// </returns>
			using GetPixel_Func = void*(*)(void* pEdit, int frameNum); GetPixel_Func GetPixel;

			/// <summary>
			/// 指定したフレームのオーディオデータを読み込みます
			/// <para>データはフィルタ前のものです</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="pBuffer">格納するバッファ (NULLならサンプル数の取得のみ)</param>
			/// <returns>
			/// 読み込んだサンプル数
			/// </returns>
			using GetAudio_Func = int(*)(void* pEdit, int frameNum, void* pBuffer); GetAudio_Func GetAudio;

			/// <summary>
			/// 現在編集中か調べます
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <returns>
			/// 1 であれば、編集中
			/// </returns>
			using IsEditing_Func = int(*)(void* pEdit); IsEditing_Func IsEditing;

			/// <summary>
			/// 現在保存中か調べます
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <returns>
			/// 1 であれば、保存中
			/// </returns>
			using IsSaving_Func = int(*)(void* pEdit); IsSaving_Func IsSaving;

			/// <summary>
			/// 現在の表示フレームを取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <returns>
			/// 現在のフレーム番号
			/// </returns>
			using GetFrame_Func = int(*)(void* pEdit); GetFrame_Func GetFrame;

			/// <summary>
			/// 総フレーム数を取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <returns>
			/// 現在の総フレーム数
			/// </returns>
			using GetFrame_Total_Func = int(*)(void* pEdit); GetFrame_Total_Func GetFrameTotal;

			/// <summary>
			/// フィルタ前のフレームのサイズを取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="pWidth">画像幅の格納ポインタ</param>
			/// <param name="pHeight">画像高さの格納ポインタ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetFrameSize_Func = int(*)(void* pEdit, int* pWidth, int* pHeight); GetFrameSize_Func GetFrameSize;

			/// <summary>
			/// 現在の表示フレームを変更します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 設定されたフレーム番号
			/// </returns>
			using SetFrame_Func = int(*)(void* pEdit, int frameNum); SetFrame_Func SetFrame;

			/// <summary>
			/// 総フレーム数を変更します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 設定されたフレーム番号
			/// </returns>
			using SetFrameTotal_Func = int(*)(void* pEdit, int frameNum); SetFrameTotal_Func SetFrameTotal;

			/// <summary>
			/// フレームを他のフレームにコピーします
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="dest">コピー先フレーム番号</param>
			/// <param name="src">コピー元フレーム番号</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using CopyFrame_Func = int(*)(void* pEdit, int dest, int src); CopyFrame_Func CopyFrame;

			/// <summary>
			/// フレームの映像だけを他のフレームにコピーします
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="dest">コピー先フレーム番号</param>
			/// <param name="src">コピー元フレーム番号</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using CopyVideo_Func = int(*)(void* pEdit, int dest, int src); CopyVideo_Func CopyVideo;

			/// <summary>
			/// フレームの音声だけを他のフレームにコピーします
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="dest">コピー先フレーム番号</param>
			/// <param name="src">コピー元フレーム番号</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using CopyAudio_Func = int(*)(void* pEdit, int dest, int src); CopyAudio_Func CopyAudio;

			/// <summary>
			/// クリップボードにDIB形式(RGB24bit)の画像をコピーします
			/// </summary>
			/// <param name="hWnd">ウィンドウハンドル</param>
			/// <param name="pPixel">DIB形式データへのポインタ</param>
			/// <param name="width">画像の幅</param>
			/// <param name="height">画像の高さ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using CopyClip_Func = int(*)(void* hWnd, void* pPixel, int width, int height); CopyClip_Func CopyClip;

			/// <summary>
			/// クリップボードから画像を張りつけます
			/// </summary>
			/// <param name="hWnd">ウィンドウハンドル</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using PasteClip_Func = int(*)(void* hWnd, void* pEdit, int frameNum); PasteClip_Func PasteClip;

			/// <summary>
			/// フレームのステータスを取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="pFrameStatus">フレームステータスへのポインタ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetFrameStatus_Func = int(*)(void* pEdit, int frameNum, FrameStatus* pFrameStatus); GetFrameStatus_Func GetFrameStatus;

			/// <summary>
			/// フレームのステータスを変更します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="pFrameStatus">フレームステータスへのポインタ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using SetFrameStatus_Func = int(*)(void* pEdit, int frameNum, FrameStatus* pFrameStatus); SetFrameStatus_Func SetFrameStatus;

			/// <summary>
			/// 実際に保存されるフレームか調べます
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 1 なら保存されます
			/// </returns>
			using IsSaveFrame_Func = int(*)(void* pEdit, int frameNum); IsSaveFrame_Func IsSaveFrame;

			/// <summary>
			/// キーフレームかどうか調べます
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 1 ならキーフレーム
			/// </returns>
			using IsKeyFrame_Func = int(*)(void* pEdit, int frameNum); IsKeyFrame_Func IsKeyFrame;

			/// <summary>
			/// 再圧縮が必要か調べます
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 1 なら再圧縮が必要
			/// </returns>
			using IsRecompress_Func = int(*)(void* pEdit, int frameNum); IsRecompress_Func IsRecompress;

			/// <summary>
			/// 設定ウィンドウのトラックバーとチェックボックスを再描画します
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using FilterWindowUpdate_Func = int(*)(void* pFilter); FilterWindowUpdate_Func FilterWindowUpdate;

			/// <summary>
			/// 設定ウィンドウが表示されているか調べます
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <returns>
			/// 1 なら表示されている
			/// </returns>
			using IsFilterWindowDisp_Func = int(*)(void* pFilter); IsFilterWindowDisp_Func IsFilterWindowDisp;

			/// <summary>
			/// 編集ファイルの情報を取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="pFileInfo">ファイルインフォメーション構造体へのポインタ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetFileInfo_Func = int(*)(void* pEdit, FileInfo* pFileInfo); GetFileInfo_Func GetFileInfo;

			/// <summary>
			/// 現在のプロファイルの名前を取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="profileNum">プロファイル環境の番号</param>
			/// <returns>
			/// プロファイルの名前へのポインタ (NULLなら失敗)
			/// </returns>
			using GetConfigName_Func = char*(*)(void* pEdit, int profileNum); GetConfigName_Func GetConfigName;

			/// <summary>
			/// フィルタが有効になっているか調べます
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <returns>
			/// 1 ならフィルタ有効
			/// </returns>
			using IsFilterActive_Func = int(*)(void* pFilter); IsFilterActive_Func IsFilterActive;

			/// <summary>
			/// 指定したフレームのDIB形式(RGB24bit)の画像データを読み込みます
			/// <para>データはフィルタ後のものです</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="pPixel">DIB形式データを格納するポインタ (NULLなら画像サイズだけを返します)</param>
			/// <param name="pWidth">画像幅の格納ポインタ (NULLならDIB形式データだけを返します)</param>
			/// <param name="pHeight">画像高さの格納ポインタ (NULLならDIB形式データだけを返します)</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetPixelFiltered_Func = int(*)(void* pEdit, int frameNum, void* pPixel, int* pWidth, int* pHeight); GetPixelFiltered_Func GetPixelFiltered;

			/// <summary>
			/// 指定したフレームのオーディオデータを読み込みます
			/// <para>データはフィルタ後のものです</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="pBuffer">格納するバッファ (NULLならサンプル数の取得のみ)</param>
			/// <returns>
			/// 読み込んだサンプル数
			/// </returns>
			using GetAudioFiltered_Func = int(*)(void* pEdit, int frameNum, void* pBuffer); GetAudioFiltered_Func GetAudioFiltered;

			/// <summary>
			/// 選択開始終了フレームを取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="pStartFrame">選択開始フレーム</param>
			/// <param name="pEndFrame">選択終了フレーム</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetSelectFrame_Func = int(*)(void* pEdit, int* pStartFrame, int* pEndFrame); GetSelectFrame_Func GetSelectFrame;

			/// <summary>
			/// 選択開始終了フレームを設定します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="pStartFrame">選択開始フレーム</param>
			/// <param name="pEndFrame">選択終了フレーム</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using SetSelectFrame_Func = int(*)(void* pEdit, int startFrame, int endFrame); SetSelectFrame_Func SetSelectFrame;

			/// <summary>
			/// PixelからPixel_YCに変換します
			/// </summary>
			/// <param name="pYC">Pixel_YC構造体へのポインタ</param>
			/// <param name="pPixel">Pixel構造体へのポインタ</param>
			/// <param name="structCount">構造体の数</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using RGB2YC_Func = int(*)(Pixel_YC* pYC, Pixel* pPixel, int structCount); RGB2YC_Func RGB2YC;

			/// <summary>
			/// Pixel_YCからPixelに変換します
			/// </summary>
			/// <param name="pPixel">Pixel構造体へのポインタ</param>
			/// <param name="pYC">Pixel_YC構造体へのポインタ</param>
			/// <param name="structCount">構造体の数</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using YC2RGB_Func = int(*)(Pixel* pPixel, Pixel_YC* pYC, int structCount); YC2RGB_Func YC2RGB;

			/// <summary>
			/// ファイルダイアログを使って読み込むファイル名を取得します
			/// </summary>
			/// <param name="name">ファイル名を格納するポインタ</param>
			/// <param name="filterName">ファイルフィルタ</param>
			/// <param name="defaultName">デフォルトのファイル名</param>
			/// <returns>
			/// 1 なら成功
			/// それ以外は、キャンセル
			/// </returns>
			using DlgSetLoadName_Func = int(*)(char* name, char* filterName, char* defaultName); DlgSetLoadName_Func DlgSetLoadName;

			/// <summary>
			/// ファイルダイアログを使って書き込むファイル名を取得します
			/// </summary>
			/// <param name="name">ファイル名を格納するポインタ</param>
			/// <param name="filterName">ファイルフィルタ</param>
			/// <param name="defaultName">デフォルトのファイル名</param>
			/// <returns>
			/// 1 なら成功
			/// それ以外は、キャンセル
			/// </returns>
			using DlgSetSaveName_Func = int(*)(char* name, char* filterName, char* defaultName); DlgSetSaveName_Func DlgSetSaveName;

			/// <summary>
			/// INIファイルから数値を読み込む
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="key">アクセス用のキーの名前</param>
			/// <param name="defaultNum">デフォルトの数値</param>
			/// <returns>
			/// 読み込んだ数値
			/// </returns>
			using IniLoadInt_Func = int(*)(void* pFilter, char* key, int defaultNum); IniLoadInt_Func IniLoadInt;

			/// <summary>
			/// INIファイルに数値を書き込む
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="key">アクセス用のキーの名前</param>
			/// <param name="destNum">書き込む数値</param>
			/// <returns>
			/// 書き込んだ数値
			/// </returns>
			using IniSaveInt_Func = int(*)(void* pFilter, char* key, int destNum); IniSaveInt_Func IniSaveInt;

			/// <summary>
			/// INIファイルから文字列を読み込む
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="key">アクセス用のキーの名前</param>
			/// <param name="pBufferStr">文字列を読み込むバッファ</param>
			/// <param name="defaultStr">デフォルトの文字列</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using IniLoadStr_Func = int(*)(void* pFilter, char* key, char* pBufferStr, char* defaultStr); IniLoadStr_Func IniLoadStr;

			/// <summary>
			/// INIファイルに文字列を書き込む
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="key">アクセス用のキーの名前</param>
			/// <param name="str">書き込む文字列</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using IniSaveStr_Func = int(*)(void* pFilter, char* key, char* str); IniSaveStr_Func IniSaveStr;

			/// <summary>
			/// 指定したファイルIDのファイルの情報を取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="pFileInfo">ファイルインフォメーション構造体へのポインタ</param>
			/// <param name="srcFileId">ファイルID</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetSourceFileInfo_Func = int(*)(void* pEdit, FileInfo* pFileInfo, int srcFileId); GetSourceFileInfo_Func GetSourceFileInfo;

			/// <summary>
			/// 指定したフレームのソースのファイルIDとフレーム番号を取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="pSrcFileId">ファイルIDを格納するポインタ</param>
			/// <param name="pSrcVideoNumber">フレーム番号を格納するポインタ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetSourceVideoNumber_Func = int(*)(void* pEdit, int frameNum, int* pSrcFileId, int* pSrcVideoNumber); GetSourceVideoNumber_Func GetSourceVideoNumber;

			/// <summary>
			/// システムの情報を取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル (NULLならpSystemInfoの編集中のフラグとすべてのファイル名が無効になります)</param>
			/// <param name="pSystemInfo">システムインフォメーション構造体へのポインタ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetSystemInfo_Func = int(*)(void* pEdit, SystemInfo* pSystemInfo); GetSystemInfo_Func GetSystemInfo;

			/// <summary>
			/// 指定のフィルタIDのフィルタ構造体へのポインタを取得します
			/// </summary>
			/// <param name="filterId">フィルタID (0～登録されてるフィルタの数-1までの値)</param>
			/// <returns>
			/// フィルタ構造体へのポインタ (NULLなら失敗)
			/// </returns>
			using GetFilterPtr_Func = void*(*)(int filterId); GetFilterPtr_Func GetFilterPtr;

			/// <summary>
			/// 指定したフレームの画像データのポインタを取得します
			/// <para>データは自分のフィルタの直前までフィルタしたものです</para>
			/// <para>※出来るだけget_ycp_filtering_cache_ex()の方を使用するようにしてください</para>
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="reserved">予約領域　(NULLを指定してください)</param>
			/// <returns>
			/// 画像データへのポインタ (NULLなら失敗)
			/// <para>画像データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効</para>
			/// </returns>
			using GetYcpFiltering_Func = void*(*)(void* pFilter, void* pEdit, int frameNum, void* reserved); GetYcpFiltering_Func GetYcpFiltering;

			/// <summary>
			/// 指定したフレームのオーディオデータを読み込みます
			/// <para>データは自分のフィルタの直前までフィルタしたものです</para>
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="reserved">格納するバッファ (NULLならサンプル数の取得のみ)</param>
			/// <returns>
			/// 読み込んだサンプル数
			/// </returns>
			using GetAudioFiltering_Func = int(*)(void* pFilter, void* pEdit, int frameNum, void* pBuffer); GetAudioFiltering_Func GetAudioFiltering;

			/// <summary>
			/// GetYcpFilteringCacheEX()のキャッシュの設定をします
			/// <para>設定値が変わった時のみキャッシュ領域を再確保します</para>
			/// <para>キャッシュ領域はフィルタがアクティブの時のみ確保されます</para>
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="width">キャッシュ領域の幅</param>
			/// <param name="height">キャッシュ領域の高さ</param>
			/// <param name="cacheFrameSize">キャッシュするフレーム数</param>
			/// <param name="flag">NULLを指定してください</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using SetYcpFilteringCacheSize_Func = int(*)(void* pFilter, int width, int height, int cacheFrameSize, int flag); SetYcpFilteringCacheSize_Func SetYcpFilteringCacheSize;

			/// <summary>
			/// 指定したフレームの画像データのキャッシュポインタを取得します
			/// <para>SetYcpFilteringCacheSize_Func()の設定にしたがってキャッシュされます</para>
			/// <para>データは自分のフィルタの直前までフィルタしたものです</para>
			/// <para>※出来るだけGetYcpFilteringCache_Func()の方を使用するようにしてください</para>
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 画像データへのキャッシュポインタ (NULLなら失敗)
			/// <para>画像データポインタの内容はキャッシュから破棄されるまで有効</para>
			/// </returns>
			using GetYcpFilteringCache_Func = void*(*)(void* pFilter, void* pEdit, int frameNum); GetYcpFilteringCache_Func GetYcpFilteringCache;

			/// <summary>
			/// 指定したフレームの画像データのポインタを取得します
			/// <para>データはフィルタ前のものです</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="frameOffset">元のAVI上でのフレームのオフセット</param>
			/// <returns>
			/// 画像データへのポインタ (NULLなら失敗)
			/// <para>画像データポインタの内容はキャッシュから破棄されるまで有効</para>
			/// </returns>
			using GetYcpSourceCache_Func = void*(*)(void* pEdit, int frameNum, int frameOffset); GetYcpSourceCache_Func GetYcpSourceCache;

			/// <summary>
			/// 表示されているフレームの画像データのポインタを取得します
			/// <para>データはフィルタ後のものです</para>
			/// <para>表示フィルタのみ使用可能です。</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="format">画像フォーマット( NULL = RGB24bit / 'Y''U''Y''2' = YUY2 )</param>
			/// <returns>
			/// 画像データへのポインタ (NULLなら失敗)
			/// <para>画像データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効</para>
			/// </returns>
			using GetDispPixelPtr_Func = void*(*)(void* pEdit, int format); GetDispPixelPtr_Func GetDispPixelPtr;

			/// <summary>
			/// 指定したフレームの画像データを読み込みます
			/// <para>データはフィルタ前のものです</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="pPixel">DIB形式データを格納するポインタ</param>
			/// <param name="format">画像フォーマット( NULL = RGB24bit / 'Y''U''Y''2' = YUY2 )</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetPixelSource_Func = int(*)(void* pEdit, int frameNum, void* pPixel, int format); GetPixelSource_Func GetPixelSource;

			/// <summary>
			/// 指定したフレームの画像データを読み込みます
			/// <para>データはフィルタ後のものです</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="pPixel">DIB形式データを格納するポインタ (NULLなら画像サイズだけを返します)</param>
			/// <param name="pWidth">画像の幅 (NULLならDIB形式データだけを返します)</param>
			/// <param name="pHeight">画像の高さ (NULLならDIB形式データだけを返します)</param>
			/// <param name="format">画像フォーマット( NULL = RGB24bit / 'Y''U''Y''2' = YUY2 )</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetPixelFilteredEX_Func = int(*)(void* pEdit, int frameNum, void* pPixel, int* pWidth, int* pHeight, int format); GetPixelFilteredEX_Func GetPixelFilteredEX;

			/// <summary>
			/// 指定したフレームの画像データのキャッシュポインタを取得します
			/// <para>set_ycp_filtering_cache_size()の設定にしたがってキャッシュされます</para>
			/// <para>データは自分のフィルタの直前までフィルタしたものです</para>
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="pWidth">取得した画像の幅 (NULLなら無視されます)</param>
			/// <param name="pHeight">取得した画像の高さ (NULLなら無視されます)</param>
			/// <returns>
			/// 画像データへのキャッシュポインタ (NULLなら失敗)
			/// <para>画像データポインタの内容はキャッシュから破棄されるまで有効</para>
			/// </returns>
			using GetYcpFilteringCacheEX_Func = Pixel_YC*(*)(void* pFilter, void* pEdit, int frameNum, int* pWidth, int* pHeight); GetYcpFilteringCacheEX_Func GetYcpFilteringCacheEX;

			/// <summary>
			/// 指定した関数をシステムの設定値に応じたスレッド数で呼び出します
			/// <para>呼び出された関数内からWin32APIや外部関数(RGB2YC,YC2RGBは除く)を使用しないでください</para>
			/// </summary>
			/// <param name="pFunc">マルチスレッドで呼び出す関数</param>
			/// <param name="pParam1">呼び出す関数に渡す汎用パラメータ</param>
			/// <param name="pParam2">呼び出す関数に渡す汎用パラメータ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using ExecMultiThread_Func = int(*)(MultiThread_Func pFunc, void* pParam1, void* pParam2); ExecMultiThread_Func ExecMultiThread;

			/// <summary>
			/// 空のフレーム画像データ領域を作成します
			/// <para>YcpEditと同様に外部関数で使用できますが</para>
			/// <para>FilterProcInfoのYcpEdit,YcpTempと入れ替えることは出来ません</para>
			/// </summary>
			/// <returns>
			/// 作成したフレーム画像データへのポインタ (NULLなら失敗)
			/// </returns>
			using CreateYC_Func = Pixel_YC*(*)(); CreateYC_Func CreateYC;

			/// <summary>
			/// CreateYCで作成した領域を削除します
			/// </summary>
			/// <param name="pYC">フレーム画像データへのポインタ</param>
			using DeleteYC_Func = void(*)(Pixel_YC* pYC); DeleteYC_Func DeleteYC;

			/// <summary>
			/// フレーム画像データにBMPファイルから画像を読み込みます
			/// </summary>
			/// <param name="pYC">画像を読み込むフレーム画像へのポインタ (NULLなら描画をせずにサイズを返します)</param>
			/// <param name="pFileName">読み込むBMPファイル名</param>
			/// <param name="pWidth">読み込んだ画像の幅 (NULLを指定できます)</param>
			/// <param name="pHeight">読み込んだ画像の高さ (NULLを指定できます)</param>
			/// <param name="flag">NULLを指定してください</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using LoadImage_Func = int(*)(Pixel_YC* pYC, char* pFileName, int* pWidth, int* pHeight, int flag); LoadImage_Func LoadImageFile;

			/// <summary>
			/// フレーム画像データをリサイズします
			/// <para>元画像の任意の画像領域をリサイズすることも出来ます</para>
			/// </summary>
			/// <param name="pYC">リサイズ後のフレーム画像を格納するポインタ</param>
			/// <param name="width">リサイズ解像度の幅</param>
			/// <param name="height">リサイズ解像度の高さ</param>
			/// <param name="pYcSrc">元画像のフレーム画像へのポインタ(NULLならpYCと同じ)</param>
			/// <param name="srcX">元画像のリサイズ対象領域の左上のX座標</param>
			/// <param name="srcY">元画像のリサイズ対象領域の左上のY座標</param>
			/// <param name="srcWidth">元画像のリサイズ対象領域の幅</param>
			/// <param name="srcHeight">元画像のリサイズ対象領域の高さ</param>
			using ResizeYC_Func = void(*)(Pixel_YC* pYC, int width, int height, Pixel_YC* pYcSrc, int srcX, int srcY, int srcWidth, int srcHeight); ResizeYC_Func ResizeYC;

			/// <summary>
			/// フレーム画像データの任意の領域をコピーします
			/// <para>描画の際は最大画像サイズの領域に収まるようにクリッピングをします</para>
			/// <para>コピー元とコピー先の領域は重ならないようにしてください</para>
			/// </summary>
			/// <param name="pYC">コピー先のフレーム画像へのポインタ</param>
			/// <param name="x">コピー先の左上のX座標</param>
			/// <param name="y">コピー先の左上のY座標</param>
			/// <param name="pYcSrc">コピー元のフレーム画像へのポインタ</param>
			/// <param name="srcX">コピー元の左上のX座標</param>
			/// <param name="srcY">コピー元の左上のY座標</param>
			/// <param name="srcWidth">幅</param>
			/// <param name="srcHeight">コピー元の高さ</param>
			/// <param name="transparent">コピー元の透明度 (0～4096)</param>
			using CopyYC_Func = void(*)(Pixel_YC* pYC, int x, int y, Pixel_YC* pYcSrc, int srcX, int srcY, int srcWidth, int srcHeight, int transparent); CopyYC_Func CopyYC;

			/// <summary>
			/// フレーム画像データにテキストを描画します
			/// <para>描画の際は最大画像サイズの領域に収まるようにクリッピングをします</para>
			/// </summary>
			/// <param name="pYC">描画するフレーム画像データへのポインタ (NULLなら描画をせずにサイズを返します)</param>
			/// <param name="x">描画する左上のX座標</param>
			/// <param name="y">描画する左上のY座標</param>
			/// <param name="pText">描画するテキストの内容</param>
			/// <param name="r">描画色R (0～255)</param>
			/// <param name="g">描画色G (0～255)</param>
			/// <param name="b">描画色B (0～255)</param>
			/// <param name="a">透明度 (0～4096)</param>
			/// <param name="hFont">描画で使用するフォント (NULLならデフォルトのフォント)</param>
			/// <param name="pWidth">描画したテキスト領域の幅 (NULLを指定できます)</param>
			/// <param name="pHeight">描画したテキスト領域の高さ (NULLを指定できます)</param>
			using DrawText_Func = void(*)(Pixel_YC* pYC, int x, int y, char* pText, int r, int g, int b, int a, void* hFont, int* pWidth, int* pHeight); DrawText_Func DrawTextYC;

			/// <summary>
			/// AVIファイルをオープンしてavi_file_read_video(),avi_file_read_audio()でデータを読み込む為のハンドルを取得します。
			/// <para>※編集中のファイルとフォーマット(fpsやサンプリングレート等)が異なる場合があるので注意してください。</para>
			/// </summary>
			/// <param name="pFileName">読み込むAVIファイル名 (入力プラグインで読み込めるファイルも指定できます)</param>
			/// <param name="pFileInfo">ファイルインフォメーション構造体へのポインタ (読み込んだファイルの情報が入ります)</param>
			/// <param name="flag">読み込みフラグ</param>
			/// <returns>
			/// AVIファイルハンドル (NULLなら失敗)
			/// </returns>
			using AviFileOpen_Func = AviFileHandle(*)(char* pFileName, FileInfo* pFileInfo, AviFileOpenFlag flag); AviFileOpen_Func AviFileOpen;

			/// <summary>
			/// AVIファイルをクローズします
			/// </summary>
			/// <param name="aviHandle">AVIファイルハンドル</param>
			using AviFileClose_Func = void(*)(AviFileHandle aviHandle); AviFileClose_Func AviFileClose;

			/// <summary>
			/// フレーム画像データにAVIファイルから画像データを読み込みます
			/// </summary>
			/// <param name="aviHandle">AVIファイルハンドル</param>
			/// <param name="pYC">画像データを読み込むフレーム画像へのポインタ</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using AviFileReadVideo_Func = int(*)(AviFileHandle aviHandle, Pixel_YC* pYC, int frameNum); AviFileReadVideo_Func AviFileReadVideo;

			/// <summary>
			/// AVIファイルから音声データを読み込みます
			/// </summary>
			/// <param name="aviHandle">AVIファイルハンドル</param>
			/// <param name="pBuffer">音声を読み込むバッファへのポインタ</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 読み込んだサンプル数
			/// </returns>
			using AviFileReadAudio_Func = int(*)(AviFileHandle aviHandle, void* pBuffer, int frameNum); AviFileReadAudio_Func AviFileReadAudio;

			/// <summary>
			/// AVIファイルから読み込んだDIB形式の画像データのポインタを取得します
			/// <para>取得できる画像データのフォーマットは、AviFileOpen() で取得した FileInfo のビデオ展開形式になります。</para>
			/// </summary>
			/// <param name="aviHandle">AVIファイルハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// DIB形式データへのポインタ (NULLなら失敗)
			/// <para>画像データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効</para>
			/// </returns>
			using AviFileGetVideoPixelPtr_Func = void*(*)(AviFileHandle aviHandle, int frameNum); AviFileGetVideoPixelPtr_Func AviFileGetVideoPixelPtr;

			/// <summary>
			/// AviFileOpen() で読み込めるファイルのファイルフィルタを取得します
			/// </summary>
			/// <param name="type">ファイルの種類</param>
			/// <returns>
			/// ファイルフィルタへのポインタ
			/// </returns>
			using GetAviFileFilter_Func = char*(*)(GetAviFileFilterType type); GetAviFileFilter_Func GetAviFileFilter;

			/// <summary>
			/// AVIファイルから音声データを読み込みます
			/// </summary>
			/// <param name="aviHandle">AVIファイルハンドル</param>
			/// <param name="start">読み込み開始サンプル番号</param>
			/// <param name="length">読み込むサンプル数</param>
			/// <param name="pBuffer">データを読み込むバッファへのポインタ</param>
			/// <returns>
			/// 読み込んだサンプル数
			/// </returns>
			using AviFileReadAudioSample_Func = int(*)(AviFileHandle aviHandle, int start, int length, void* pBuffer); AviFileReadAudioSample_Func AviFileReadAudioSample;

			/// <summary>
			/// AviFileReadAudioSample()で読み込む音声のサンプリングレート等を変更します
			/// </summary>
			/// <param name="aviHandle">AVIファイルハンドル</param>
			/// <param name="audioRate">音声サンプリングレート</param>
			/// <param name="audioChannel">音声チャンネル数</param>
			/// <returns>
			/// 変更したサンプリングレートでの総サンプル数
			/// </returns>
			using AviFileSetAudioSampleRate_Func = int(*)(AviFileHandle aviHandle, int audioRate, int audioChannel); AviFileSetAudioSampleRate_Func AviFileSetAudioSampleRate;

			/// <summary>
			/// フレームのステータスが格納されているバッファへのポインタを取得します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="audioRate">音声サンプリングレート</param>
			/// <returns>
			/// バッファへのポインタ
			/// <para>バッファへのポインタの内容は編集ファイルがクローズされるまで有効</para>
			/// </returns>
			using GetFrameStatusTable_Func = unsigned char*(*)(void* pEdit, FrameStatusType type); GetFrameStatusTable_Func GetFrameStatusTable;

			/// <summary>
			/// 現在の編集状況をアンドゥバッファに設定します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using SetUndo_Func = int(*)(void* pEdit); SetUndo_Func SetUndo;

			/// <summary>
			/// メインウィンドウの設定メニュー項目を追加します
			/// <para>メニューが選択された時にhwndで指定したウィンドウに、WM_FILTER_COMMANDのメッセージを送ります</para>
			/// <para>※必ずfunc_init()かWM_FILTER_INITから呼び出すようにしてください。</para>
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="name">メニューの名前</param>
			/// <param name="hWnd">WM_FILTER_COMMANDを送るウィンドウハンドル</param>
			/// <param name="">WM_FILTER_COMMANDのWPARAM</param>
			/// <param name="defKey">標準のショートカットキーの仮想キーコード (NULLなら無し)</param>
			/// <param name="flag">フラグ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using AddMenuItem_Func = int(*)(void* pFilter, char* name, void* hWnd, int id, int defKey, AddMenuItemFlag flag); AddMenuItem_Func AddMenuItem;

			/// <summary>
			/// 編集ファイルを開きます
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="pFileName">ファイル名</param>
			/// <param name="flag">フラグ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using EditOpen_Func = int(*)(void* pEdit, char* pFileName, EditOpenFlag flag); EditOpen_Func EditOpen;

			/// <summary>
			/// 編集ファイルを閉じます
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using EditClose_Func = int(*)(void* pEdit); EditClose_Func EditClose;

			/// <summary>
			/// 編集データをAVI出力します
			/// <para>WAV出力やプラグイン出力も出来ます</para>
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="pFileName">出力ファイル名</param>
			/// <param name="flag">フラグ</param>
			/// <param name="type">出力プラグインの名前 (NULLならAVI/WAV出力)</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using EditOutput_Func = int(*)(void* pEdit, char* pFileName, EditOutputFlag flag, char* type); EditOutput_Func EditOutput;

			/// <summary>
			/// プロファイルを設定します
			/// </summary>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="profileNum">プロファイル環境の番号</param>
			/// <param name="pName">プロファイルの名前</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using SetConfig_Func = int(*)(void* pEdit, int profileNum, char* pName); SetConfig_Func SetConfig;

			/// <summary>
			/// 予約領域
			/// </summary>
			std::array<int, 7> Reserved;
		};

		/// <summary>
		/// フィルタプラグイン構造体
		/// </summary>
		struct FilterPluginTable final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 140 };

			/// <summary>
			/// 編集データ出力のフラグ定数
			/// </summary>
			enum class FilterFlag : int {
				/// <summary>
				/// フィルタをアクティブにします
				/// </summary>
				Active = 1,

				/// <summary>
				/// フィルタを常にアクティブにします
				/// </summary>
				AlwaysActive = 4,

				/// <summary>
				/// 設定をポップアップメニューにします
				/// </summary>
				ConfigPopup = 8,

				/// <summary>
				/// 設定をチェックボックスメニューにします
				/// </summary>
				ConfigCheck = 16,

				/// <summary>
				/// 設定をラジオボタンメニューにします
				/// </summary>
				ConfigRadio = 32,

				/// <summary>
				/// 拡張データを保存出来るようにします
				/// </summary>
				ExData = 1024,

				/// <summary>
				/// フィルタのプライオリティを常に最上位にします
				/// </summary>
				PriorityHighest = 2048,

				/// <summary>
				/// フィルタのプライオリティを常に最下位にします
				/// </summary>
				PriorityLowest = 4096,

				/// <summary>
				/// サイズ変更可能なウィンドウを作ります
				/// </summary>
				WindowThickFrame = 8192,

				/// <summary>
				/// 設定ウィンドウのサイズを指定出来るようにします
				/// </summary>
				WindowSize = 16384,

				/// <summary>
				/// 表示フィルタにします
				/// </summary>
				DispFilter = 32768,

				/// <summary>
				/// 再描画をplugin側で処理するようにします
				/// </summary>
				ReDraw = 0x20000,

				/// <summary>
				/// フィルタの拡張情報を設定できるようにします
				/// </summary>
				ExInformation = 0x40000,

				/// <summary>
				/// （FILTER_FLAG_EX_INFORMATION を使うようにして下さい）
				/// </summary>
				Information = 0x80000,

				/// <summary>
				/// 設定ウィンドウを表示しないようにします
				/// </summary>
				NoConfig = 0x100000,

				/// <summary>
				/// オーディオフィルタにします
				/// </summary>
				AudioFilter = 0x200000,

				/// <summary>
				/// チェックボックスをラジオボタンにします
				/// </summary>
				RadioButton = 0x400000,

				/// <summary>
				/// 水平スクロールバーを持つウィンドウを作ります
				/// </summary>
				WindowHScroll = 0x800000,

				/// <summary>
				/// 垂直スクロールバーを持つウィンドウを作ります
				/// </summary>
				WindowVSscroll = 0x1000000,

				/// <summary>
				/// インターレース解除フィルタにします
				/// </summary>
				InterlaceFilter = 0x4000000,

				/// <summary>
				/// FilterProc()の画像の初期データを作成しないようにします
				/// </summary>
				NoInitData = 0x8000000,

				/// <summary>
				/// インポートメニューを作ります
				/// </summary>
				Import = 0x10000000,

				/// <summary>
				/// エクスポートメニューを作ります
				/// </summary>
				ExportMenu = 0x20000000,

				/// <summary>
				/// WndProc()に、WM_FILTER_MAIN_???のメッセージを送るようにします
				/// </summary>
				MainMessage = 0x40000000,
			};

			/// <summary>
			/// フィルタ用ウィンドウメッセージ
			/// </summary>
			enum class WindowMessage : int {
				/// <summary>
				/// 各フィルタ設定や編集内容が変更された直後に送られます
				/// </summary>
				Update = 1124,

				/// <summary>
				/// 編集ファイルがオープンされた直後に送られます
				/// </summary>
				FileOpen = 1125,

				/// <summary>
				/// 編集ファイルがクローズされる直前に送られます
				/// </summary>
				FileClose = 1126,

				/// <summary>
				/// 開始直後に送られます
				/// </summary>
				Init = 1127,

				/// <summary>
				/// 終了直前に送られます
				/// </summary>
				Exit = 1128,

				/// <summary>
				/// セーブが開始される直前に送られます
				/// </summary>
				SaveStart = 1129,

				/// <summary>
				/// セーブが終了された直後に送られます
				/// </summary>
				SaveEnd = 1130,

				/// <summary>
				/// インポートが選択された直後に送られます
				/// </summary>
				Import = 1131,

				/// <summary>
				/// エクスポートが選択された直後に送られます
				/// </summary>
				ExportMenu = 1132,

				/// <summary>
				/// フィルタの有効/無効が変更された直後に送られます
				/// </summary>
				ChangeActive = 1133,

				/// <summary>
				/// フィルタウィンドウの表示/非表示が変更された直後に送られます
				/// </summary>
				ChangeWindow = 1134,

				/// <summary>
				/// 自分のフィルタの設定が変更された直後に送られます
				/// </summary>
				ChangeParam = 1135,

				/// <summary>
				/// 編集中/非編集中が変更された直後に送られます
				/// </summary>
				ChangeEdit = 1136,

				/// <summary>
				/// 
				/// </summary>
				Command = 1137,

				/// <summary>
				/// 編集ファイルの更新(追加や音声読み込み等)があった直後に送られます
				/// </summary>
				FileUpdate = 1138,

				/// <summary>
				/// メインウィンドウでマウスの左ボタンが押された時に送られます
				/// <para>lparamには編集画像上での座標が入ります(編集中以外は0になります)</para>
				/// </summary>
				MainMouseDown = 1144,

				/// <summary>
				/// メインウィンドウでマウスが移動した時に送られます
				/// <para>lparamには編集画像上での座標が入ります(編集中以外は0になります)</para>
				/// </summary>
				MainMouseUp = 1145,

				/// <summary>
				/// メインウィンドウでマウスの左ボタンが離された時に送られます
				/// <para>lparamには編集画像上での座標が入ります(編集中以外は0になります)</para>
				/// </summary>
				MainMouseMove = 1146,

				/// <summary>
				/// メインウィンドウでキーが押された時に送られます
				/// <para>wparamには仮想キーコードが入ります</para>
				/// </summary>
				MainKeyDown = 1147,

				/// <summary>
				/// メインウィンドウでキーが離された時に送られます
				/// <para>wparamには仮想キーコードが入ります</para>
				/// </summary>
				MainKeyUp = 1148,

				/// <summary>
				/// メインウィンドウの位置やサイズが変更された時に送られます
				/// <para>lparamにはメインウィンドウのウィンドウハンドルが入ります</para>
				/// </summary>
				MainMoveSize = 1149,

				/// <summary>
				/// メインウィンドウでマウスの左ボタンがダブルクリックされた時に送られます
				/// <para>lparamには編集画像上での座標が入ります(編集中以外は0になります)</para>
				/// </summary>
				MainMouseDblclk = 1150,

				/// <summary>
				/// メインウィンドウでマウスの右ボタンが押された時に送られます
				/// <para>lparamには編集画像上での座標が入ります(編集中以外は0になります)</para>
				/// </summary>
				MainMouseRDown = 1151,

				/// <summary>
				/// メインウィンドウでマウスの右ボタンが離された時に送られます
				/// <para>lparamには編集画像上での座標が入ります(編集中以外は0になります)</para>
				/// </summary>
				MainMouseRUp = 1152,

				/// <summary>
				/// メインウィンドウでマウスのホイールが回転した時に送られます
				/// <para>wparamの上位ワードにホイールの回転量が入ります</para>
				/// <para>lparamには編集画像上での座標が入ります(編集中以外は0になります)</para>
				/// </summary>
				MainMouseWheel = 1153,

				/// <summary>
				/// メインウィンドウでコンテキストメニューが表示される時に送られます
				/// <para>lparamにはスクリーン座標が入ります</para>
				/// <para>メニューを表示した時は戻り値をTRUEにしてください(再描画はされません)</para>
				/// <para>戻り値をTRUEにすると編集内容が更新されたとして全体が再描画されます</para>
				/// </summary>
				MainContextMenu = 1154,

				/// <summary>
				/// チェックボックスのボタンを押したときに WM_COMMAND( WPARAM = FilterButton + n )のウィンドウメッセージが送られます
				/// </summary>
				FilterButton = 12004,
			};

			/// <summary>
			/// 設定が変更されたときの状態フラグ定数
			/// </summary>
			enum class FilterUpdateStatusType : int {
				/// <summary>
				/// 全項目が変更された
				/// </summary>
				All = 0,

				/// <summary>
				/// Track + n：n番目のトラックバーが変更された
				/// </summary>
				Track = 0x10000,

				/// <summary>
				/// Check + n：n番目のチェックボックスが変更された
				/// </summary>
				Check = 0x20000,
			};

			/// <summary>
			/// フィルタ用ウィンドウのサイズ設定フラグ定数
			/// </summary>
			enum class FilterWindowSize : int {
				/// <summary>
				/// ORして設定するとクライアント領域でのサイズ指定になります。
				/// </summary>
				Client = 0x10000000,

				/// <summary>
				/// ORして設定すると標準のサイズからの追加分の指定になります。
				/// </summary>
				Add = 0x30000000,
			};

			/// <summary>
			/// フィルタのフラグ
			/// </summary>
			FilterFlag Flag;

			struct {
				/// <summary>
				/// 設定ウインドウの幅 (FILTER_FLAG_WINDOW_SIZEが立っている時に有効)
				/// <para>設定値に FilterWindowSize::Client をORして設定するとクライアント領域でのサイズ指定になります。</para>
				/// <para>設定値に FilterWindowSize::Add をORして設定すると標準のサイズからの追加分の指定になります。</para>
				/// </summary>
				int Width;

				/// <summary>
				/// 設定ウインドウの高さ (FILTER_FLAG_WINDOW_SIZEが立っている時に有効)
				/// <para>設定値に FilterWindowSize::Client をORして設定するとクライアント領域でのサイズ指定になります。</para>
				/// <para>設定値に FilterWindowSize::Add をORして設定すると標準のサイズからの追加分の指定になります。</para>
				/// </summary>
				int Height;
			};

			/// <summary>
			/// フィルタの名前
			/// </summary>
			char* pName;

			/// <summary>
			/// トラックバーの数
			/// </summary>
			int Trackbar_Num;

			/// <summary>
			/// トラックバーの名前郡へのポインタ(トラックバー数が0ならNULLでよい)
			/// </summary>
			char** pTrackbar_NameList;

			/// <summary>
			/// トラックバーの初期値郡へのポインタ(トラックバー数が0ならNULLでよい)
			/// </summary>
			int* pTrackbar_Default;

			struct {
				/// <summary>
				/// トラックバーの数値の下限上限 (NULLなら全て0～256)
				/// </summary>
				int* pTrackbar_UpperLimit;

				/// <summary>
				/// トラックバーの数値の下限上限 (NULLなら全て0～256)
				/// </summary>
				int* pTrackbar_LowerLimit;
			};

			/// <summary>
			/// チェックボックスの数
			/// </summary>
			int Checkbox_Num;

			/// <summary>
			/// チェックボックスの名前郡へのポインタ(チェックボックス数が0ならNULLでよい)
			/// </summary>
			char** pCheckbox_NameList;

			/// <summary>
			/// チェックボックスの初期値郡へのポインタ(チェックボックス数が0ならNULLでよい)
			/// <para>初期値がマイナス値の場合はボタンになります。ボタンを押したときにWM_COMMAND( WPARAM = MID::FilterButton + n )のウィンドウメッセージが送られます</para>
			/// </summary>
			int* pCheckbox_Default;

			/// <summary>
			/// フィルタ処理関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="pFilterProcInfo">フィルタ処理用の構造体アドレス</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using FilterProc_Func = int(*)(FilterPluginTable* pFilter, FilterProcInfo* pFilterProcInfo); FilterProc_Func FilterProc;

			/// <summary>
			/// 開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using FilterInit_Func = int(*)(FilterPluginTable* pFilter); FilterInit_Func FilterInit;

			/// <summary>
			/// 終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using FilterExit_Func = int(*)(FilterPluginTable* pFilter); FilterExit_Func FilterExit;

			/// <summary>
			/// 自分の設定が変更されたときに呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="status">フィルタ処理用の構造体アドレス</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using FilterUpdate_Func = int(*)(FilterPluginTable* pFilter, FilterUpdateStatusType status); FilterUpdate_Func FilterUpdate;

			/// <summary>
			/// 設定ウィンドウにウィンドウメッセージが来た時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// <para>VFAPI動作時には呼ばれません</para>
			/// <para>戻り値を 1 にすると編集内容が更新されたとして全体が再描画されます</para>
			/// </summary>
			/// <param name="hWnd">ウィンドウハンドル</param>
			/// <param name="message">ウィンドウメッセージ</param>
			/// <param name="wParam">追加パラメータ</param>
			/// <param name="lParam">追加パラメータ</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <returns>
			/// 1 を返すと全体が再描画される
			/// </returns>
			using WndProc_Func = int(*)(void* hWnd, unsigned int message, unsigned int wParam, unsigned int lParam, void* pEdit, FilterPluginTable* pFilter); WndProc_Func WndProc;

			/// <summary>
			/// トラックバーの設定値郡へのポインタ (AviUtl側で設定されます)
			/// </summary>
			int* pTrackbar_List;

			/// <summary>
			/// チェックボックスの設定値郡へのポインタ (AviUtl側で設定されます)
			/// </summary>
			int* pCheckbox_List;

			/// <summary>
			/// 拡張データ領域へのポインタ (FILTER_FLAG_EX_DATAが立っている時に有効)
			/// </summary>
			void* pExData;

			/// <summary>
			/// 拡張データサイズ (FILTER_FLAG_EX_DATAが立っている時に有効)
			/// </summary>
			int ExDataSize;

			/// <summary>
			/// フィルタ情報へのポインタ (FILTER_FLAG_EX_INFORMATIONが立っている時に有効)
			/// <para>SDK付属のサンプルには、以下のような文字列が設定されていた。</para>
			/// <para>"サンプルフィルタ version 0.06 by ＫＥＮくん"</para>
			/// </summary>
			char* pInfoText;

			/// <summary>
			/// セーブが開始される直前に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="startFrame">セーブする先頭フレーム</param>
			/// <param name="endFrame">セーブする最終フレーム</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using SaveStart_Func = int(*)(FilterPluginTable* pFilter, unsigned int startFrame, unsigned int endFrame, void* pEdit); SaveStart_Func SaveStart;

			/// <summary>
			/// セーブが終了した直前に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using SaveEnd_Func = int(*)(FilterPluginTable* pFilter, void* pEdit); SaveEnd_Func SaveEnd;

			/// <summary>
			/// 外部関数テーブルへのポインタ (AviUtl側で設定されます)
			/// </summary>
			CallbackFunctionSet* pCallbackFunctionSet;

			/// <summary>
			/// ウィンドウハンドル (AviUtl側で設定されます)
			/// </summary>
			void* hWnd;

			/// <summary>
			/// DLLのインスタンスハンドル (AviUtl側で設定されます)
			/// </summary>
			void* hDllInstance;

			/// <summary>
			/// 拡張データの初期値データ領域へのポインタ (NULLなら初期化されません)
			/// </summary>
			void* pExDataDef;

			/// <summary>
			/// インターレース解除フィルタで保存するフレームを決める時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="saveCount">セーブする範囲の先頭からのフレーム番号</param>
			/// <param name="frameNum">編集フレーム番号</param>
			/// <param name="fps">フレームレートの変更の設定値 (30,24,20,15,10)</param>
			/// <param name="editFlag">編集フラグ</param>
			/// <param name="interlace">フレームのインターレース</param>
			/// <returns>
			/// 1 を返すと保存フレーム、1以外 を返すと間引きフレームになります。
			/// </returns>
			using IsSaveframe_Func = int(*)(FilterPluginTable* pFilter, void* pEdit, int saveCount, int frameNum, int fps, int editFlag, int interlace); IsSaveframe_Func IsSaveframe;

			/// <summary>
			/// プロジェクトファイルからデータを読み込んだ時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// <para>プロジェクトファイルに保存したデータが無い場合は呼ばれません</para>
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="pData">プロジェクトから読み込んだデータへのポインタ</param>
			/// <param name="size">プロジェクトから読み込んだデータのバイト数</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using ProjectLoad_Func = int(*)(FilterPluginTable* pFilter, void* pEdit, void* pData, int size); ProjectLoad_Func ProjectLoad;

			/// <summary>
			/// プロジェクトファイルをセーブしている時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// <para>プロジェクトファイルにフィルタのデータを保存します</para>
			/// <para>※AviUtlからは始めに保存サイズ取得の為にdataがNULLで呼び出され、続けて実際のデータを取得する為に呼び出されます。</para>
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="pData">プロジェクトに書き込むデータを格納するバッファへのポインタ (NULLの場合はデータのバイト数のみ返す)</param>
			/// <param name="pSize">プロジェクトに書き込むデータのバイト数を返すポインタ</param>
			/// <returns>
			/// 1 なら保存するデータがある
			/// </returns>
			using ProjectSave_Func = int(*)(FilterPluginTable* pFilter, void* pEdit, void* pData, int* pSize); ProjectSave_Func ProjectSave;

			/// <summary>
			/// メインウィンドウのタイトルバーを表示する時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// <para>タイトルバーの文字列を変更できます (未編集時、出力時は呼ばれません)</para>
			/// </summary>
			/// <param name="pFilter">フィルタテーブル構造体のアドレス</param>
			/// <param name="pEdit">エディットハンドル</param>
			/// <param name="frame">編集フレーム番号</param>
			/// <param name="pTitle">表示するタイトルバーの文字列</param>
			/// <param name="maxTitle">titleのバッファサイズ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using ModifyTitle_Func = int(*)(FilterPluginTable* pFilter, void* pEdit, int frame, char* pTitle, int maxTitle); ModifyTitle_Func ModifyTitle;

			/// <summary>
			/// PluginsディレクトリのサブディレクトリにDLLがある時のみ、サブディレクトリ名が入ります。
			/// </summary>
			char* pDllPath;

			/// <summary>
			/// 予約領域
			/// <para>NULLにしてください。</para>
			/// </summary>
			std::array<int, 2> Reserved;
		};

		// operators
		AU_DECLARE_ENUMCLASS_OPERATOR(FilterProcInfo::InfoFlag)
		AU_DECLARE_ENUMCLASS_OPERATOR(FrameStatus::InterlaceType)
		AU_DECLARE_ENUMCLASS_OPERATOR(FrameStatus::EditFlag)
		AU_DECLARE_ENUMCLASS_OPERATOR(FileInfo::FileInfoFlag)
		AU_DECLARE_ENUMCLASS_OPERATOR(SystemInfo::SystemInfoFlag)
		AU_DECLARE_ENUMCLASS_OPERATOR(CallbackFunctionSet::AviFileOpenFlag)
		AU_DECLARE_ENUMCLASS_OPERATOR(CallbackFunctionSet::GetAviFileFilterType)
		AU_DECLARE_ENUMCLASS_OPERATOR(CallbackFunctionSet::FrameStatusType)
		AU_DECLARE_ENUMCLASS_OPERATOR(CallbackFunctionSet::AddMenuItemFlag)
		AU_DECLARE_ENUMCLASS_OPERATOR(CallbackFunctionSet::EditOpenFlag)
		AU_DECLARE_ENUMCLASS_OPERATOR(CallbackFunctionSet::EditOutputFlag)
		AU_DECLARE_ENUMCLASS_OPERATOR(FilterPluginTable::FilterFlag)
		AU_DECLARE_ENUMCLASS_OPERATOR(FilterPluginTable::WindowMessage)
		AU_DECLARE_ENUMCLASS_OPERATOR(FilterPluginTable::FilterUpdateStatusType)
		AU_DECLARE_ENUMCLASS_OPERATOR(FilterPluginTable::FilterWindowSize)

		// POD check
		static_assert(std::is_pod<Pixel_YC>::value, "Error: Pixel_YC is non-POD.");
		static_assert(std::is_pod<Pixel>::value, "Error: Pixel is non-POD.");
		static_assert(std::is_pod<FilterProcInfo>::value, "Error: FilterProcInfo is non-POD.");
		static_assert(std::is_pod<FrameStatus>::value, "Error: FrameStatus is non-POD.");
		static_assert(std::is_pod<FileInfo>::value, "Error: FileInfo is non-POD.");
		static_assert(std::is_pod<SystemInfo>::value, "Error: SystemInfo is non-POD.");
		static_assert(std::is_pod<CallbackFunctionSet>::value, "Error: CallbackFunctionSet is non-POD.");
		static_assert(std::is_pod<FilterPluginTable>::value, "Error: FilterPluginTable is non-POD.");

		// Size check
		static_assert(sizeof(Pixel_YC) == Pixel_YC::Size, "Error: Pixel_YC size does not fit.");
		static_assert(sizeof(Pixel) == Pixel::Size, "Error: Pixel size does not fit.");
		static_assert(sizeof(FilterProcInfo) == FilterProcInfo::Size, "Error: FilterProcInfo size does not fit.");
		static_assert(sizeof(FrameStatus) == FrameStatus::Size, "Error: FrameStatus size does not fit.");
		static_assert(sizeof(FileInfo) == FileInfo::Size, "Error: FileInfo size does not fit.");
		static_assert(sizeof(SystemInfo) == SystemInfo::Size, "Error: SystemInfo size does not fit.");
		static_assert(sizeof(CallbackFunctionSet) == CallbackFunctionSet::Size, "Error: CallbackFunctionSet size does not fit.");
		static_assert(sizeof(FilterPluginTable) == FilterPluginTable::Size, "Error: FilterPluginTable size does not fit.");
	}

	/// <summary>
	/// Input プラグイン
	/// </summary>
	namespace Input
	{
		/// <summary>
		/// 入力ファイル情報構造体
		/// <para>※ 画像フォーマットにはRGB,YUY2とインストールされているcodecのものが使えます。</para>
		/// <para>また、'Y''C''4''8'(biBitCountは48)でPIXEL_YC形式フォーマットで扱えます。(YUY2フィルタモードでは使用出来ません)</para>
		/// <para>音声フォーマットにはPCMとインストールされているcodecのものが使えます。</para>
		/// </summary>
		struct InputInfo final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 68 };

			/// <summary>
			/// 入力ファイル情報のフラグ定数
			/// </summary>
			enum class InfoFlag : int {
				/// <summary>
				/// 画像データあり
				/// </summary>
				Video = 1,

				/// <summary>
				/// 音声データあり
				/// </summary>
				Audio = 2,

				/// <summary>
				/// キーフレームを気にせずにReadVideo()を呼び出します
				/// <para>※標準ではキーフレームからシーケンシャルにReadVideo()が呼ばれるように制御されます</para>
				/// </summary>
				VideoRandomAccess = 8,
			};

			/// <summary>
			/// ビットマップ情報ヘッダ構造体
			/// <para>BITMAPINFOHEADER 構造体と互換</para>
			/// </summary>
			struct BitmapInfoHeader final {
				/// <summary>
				/// 構造体のサイズ
				/// </summary>
				unsigned long biSize;

				/// <summary>
				/// ビットマップの幅
				/// </summary>
				long biWidth;

				/// <summary>
				/// ビットマップの高さ
				/// </summary>
				long biHeight;

				/// <summary>
				/// プレーンの枚数(常に1)
				/// </summary>
				unsigned short biPlanes;

				/// <summary>
				/// 1画素当たりのビット数(BPP)
				/// <para>1, 4, 8, 24 が使用できます。</para>
				/// </summary>
				unsigned short biBitCount;

				/// <summary>
				/// ビットマップの圧縮方式
				/// </summary>
				unsigned long biCompression;

				/// <summary>
				/// ビットマップデータのサイズ
				/// </summary>
				unsigned long biSizeImage;

				/// <summary>
				/// 水平解像度
				/// </summary>
				long biXPelsPerMeter;

				/// <summary>
				/// 垂直解像度
				/// </summary>
				long biYPelsPerMeter;

				/// <summary>
				/// カラーテーブルのエントリ数
				/// </summary>
				unsigned long biClrUsed;

				/// <summary>
				/// ビットマップを表示するのに必要な色数
				/// </summary>
				unsigned long biClrImportant;
			};

			/// <summary>
			/// 音声データフォーマットの定義
			/// <para>WAVEFORMATEX 構造体と互換</para>
			/// </summary>
			struct WaveFormatEx final {
				/// <summary>
				/// フォーマットタグ
				/// </summary>
				unsigned short wFormatTag;

				/// <summary>
				/// チャネル数
				/// </summary>
				unsigned short nChannels;

				/// <summary>
				/// サンプリング周波数
				/// </summary>
				unsigned long nSamplesPerSec;

				/// <summary>
				/// データ転送量
				/// </summary>
				unsigned long nAvgBytesPerSec;

				/// <summary>
				/// ブロックアライメントサイズ
				/// </summary>
				unsigned short nBlockAlign;

				/// <summary>
				/// コンテナサイズ
				/// </summary>
				unsigned short wBitsPerSample;

				/// <summary>
				/// 拡張データサイズ
				/// </summary>
				unsigned short cbSize;
			};

			/// <summary>
			/// 入力ファイル情報のフラグ
			/// </summary>
			InfoFlag Flag;

			/// <summary>
			/// フレームレート
			/// </summary>
			int	Rate;

			/// <summary>
			/// フレームレート
			/// </summary>
			int	Scale;

			/// <summary>
			/// 総フレーム数
			/// </summary>
			int Frame_Total;

			/// <summary>
			/// 画像フォーマットへのポインタ(次に関数が呼ばれるまで内容を有効にしておく)
			/// </summary>
			BitmapInfoHeader* pFormat;

			/// <summary>
			/// 画像フォーマットのサイズ
			/// </summary>
			int Format_Size;

			/// <summary>
			/// 音声サンプル数
			/// </summary>
			int Audio_Total;

			/// <summary>
			/// 音声フォーマットへのポインタ(次に関数が呼ばれるまで内容を有効にしておく)
			/// </summary>
			WaveFormatEx* pAudio_Format;

			/// <summary>
			/// 音声フォーマットのサイズ
			/// </summary>
			int Audio_Format_Size;

			/// <summary>
			/// 画像codecハンドラ
			/// </summary>
			unsigned long Handler;

			/// <summary>
			/// 予約領域
			/// <para>NULLにしてください。</para>
			/// </summary>
			std::array<int, 7> Reserved;
		};

		/// <summary>
		/// 入力プラグイン構造体
		/// </summary>
		struct InputPluginTable final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 116 };

			/// <summary>
			/// 入力プラグインのフラグ定数
			/// </summary>
			enum class PluginFlag : int {
				/// <summary>
				/// 画像をサポートする
				/// </summary>
				Video = 1,

				/// <summary>
				/// 音声をサポートする
				/// </summary>
				Audio = 2,
			};

			/// <summary>
			/// 入力ファイルハンドル
			/// </summary>
			using InputHandle = void*;

			/// <summary>
			/// 入力プラグインのフラグ
			/// </summary>
			PluginFlag Flag;

			/// <summary>
			/// プラグインの名前
			/// </summary>
			char* pName;

			/// <summary>
			/// 入力ファイルフィルタ
			/// </summary>
			char* pFileFilter;

			/// <summary>
			/// プラグインの情報
			/// </summary>
			char* pInformation;

			/// <summary>
			/// DLL開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using Init_Func = int(*)(); Init_Func Init;

			/// <summary>
			/// DLL終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using Exit_Func = int(*)(); Exit_Func Exit;

			/// <summary>
			/// 入力ファイルをオープンする関数へのポインタ
			/// </summary>
			/// <param name="pFile">ファイル名</param>
			/// <returns>
			/// 入力ファイルハンドル
			/// </returns>
			using Open_Func = InputHandle(*)(char* pFile); Open_Func Open;

			/// <summary>
			/// 入力ファイルをクローズする関数へのポインタ
			/// </summary>
			/// <param name="hInput">入力ファイルハンドル</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using Close_Func = int(*)(InputHandle hInput); Close_Func Close;

			/// <summary>
			/// 入力ファイルの情報を取得する関数へのポインタ
			/// </summary>
			/// <param name="hInput">入力ファイルハンドル</param>
			/// <param name="pInputInfo">入力ファイル情報構造体へのポインタ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using GetInfo_Func = int(*)(InputHandle hInput, InputInfo* pInputInfo); GetInfo_Func GetInfo;

			/// <summary>
			/// 画像データを読み込む関数へのポインタ
			/// </summary>
			/// <param name="hInput">入力ファイルハンドル</param>
			/// <param name="frame">読み込むフレーム番号</param>
			/// <param name="pBuffer">データを読み込むバッファへのポインタ</param>
			/// <returns>
			/// 読み込んだデータサイズ
			/// </returns>
			using ReadVideo_Func = int(*)(InputHandle hInput, int frame, void* pBuffer); ReadVideo_Func ReadVideo;

			/// <summary>
			/// 音声データを読み込む関数へのポインタ
			/// </summary>
			/// <param name="hInput">入力ファイルハンドル</param>
			/// <param name="start">読み込み開始サンプル番号</param>
			/// <param name="length">読み込むサンプル数</param>
			/// <param name="pBuffer">データを読み込むバッファへのポインタ</param>
			/// <returns>
			/// 読み込んだサンプル数
			/// </returns>
			using ReadAudio_Func = int(*)(InputHandle hInput, int start, int length, void* pBuffer); ReadAudio_Func ReadAudio;

			/// <summary>
			/// キーフレームか調べる関数へのポインタ (NULLなら全てキーフレーム)
			/// </summary>
			/// <param name="hInput">入力ファイルハンドル</param>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 1 ならキーフレーム
			/// </returns>
			using IsKeyframe_Func = int(*)(InputHandle hInput, int frameNum); IsKeyframe_Func IsKeyframe;

			/// <summary>
			/// 入力設定のダイアログを要求された時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="hWnd">ウィンドウハンドル</param>
			/// <param name="hDllInstance">インスタンスハンドル</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using Config_Func = int(*)(void* hWnd, void* hDllInstance); Config_Func Config;

			/// <summary>
			/// 予約領域
			/// <para>NULLにしてください。</para>
			/// </summary>
			std::array<int, 16> Reserved;
		};

		// operators
		AU_DECLARE_ENUMCLASS_OPERATOR(InputInfo::InfoFlag)
		AU_DECLARE_ENUMCLASS_OPERATOR(InputPluginTable::PluginFlag)

		// POD check
		static_assert(std::is_pod<InputInfo>::value, "Error: InputInfo is non-POD.");
		static_assert(std::is_pod<InputPluginTable>::value, "Error: InputPluginTable is non-POD.");

		// Size check
		static_assert(sizeof(InputInfo) == InputInfo::Size, "Error: InputInfo size does not fit.");
		static_assert(sizeof(InputPluginTable) == InputPluginTable::Size, "Error: InputPluginTable size does not fit.");
	}

	/// <summary>
	/// Output プラグイン
	/// </summary>
	namespace Output
	{
		/// <summary>
		/// 出力ファイル情報の構造体
		/// </summary>
		struct OutputInfo final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 76 };

			/// <summary>
			/// 出力ファイル情報のフラグ定数
			/// </summary>
			enum class InfoFlag : int {
				/// <summary>
				/// 画像データあり
				/// </summary>
				Video = 1,

				/// <summary>
				/// 音声データあり
				/// </summary>
				Audio = 2,

				/// <summary>
				/// バッチ出力中
				/// </summary>
				Batch = 4,
			};

			/// <summary>
			/// 出力フレームの情報を取得するフラグ定数
			/// </summary>
			enum class FrameFlag : int {
				/// <summary>
				/// キーフレーム
				/// </summary>
				Keyframe = 1,

				/// <summary>
				/// コピーフレーム
				/// </summary>
				CopyFrame = 2,
			};

			/// <summary>
			/// フラグ情報定数
			/// </summary>
			InfoFlag Flag;

			struct {
				/// <summary>
				/// 幅
				/// </summary>
				int Width;

				/// <summary>
				/// 高さ
				/// </summary>
				int Height;
			};

			/// <summary>
			/// フレームレート
			/// </summary>
			int	Rate;

			/// <summary>
			/// フレームレート
			/// </summary>
			int	Scale;

			/// <summary>
			/// 総フレーム数
			/// </summary>
			int Frame_Total;

			/// <summary>
			/// 1フレームのバイト数
			/// </summary>
			int Frame_Size;

			/// <summary>
			/// 音声サンプリングレート
			/// </summary>
			int Audio_Rate;

			/// <summary>
			/// オーディオチャンネル数
			/// </summary>
			int Audio_Channel;

			/// <summary>
			/// オーディオサンプルの総数
			/// </summary>
			int Audio_Total;

			/// <summary>
			/// 音声1サンプルのバイト数
			/// </summary>
			int Audio_Size;

			/// <summary>
			/// セーブファイル名へのポインタ
			/// </summary>
			char* pSaveFileName;

			/// <summary>
			/// DIB形式(RGB24bit)の画像データへのポインタを取得します。
			/// <param name="frameNum">フレーム番号</param>
			/// </summary>
			/// <returns>
			/// データへのポインタ
			/// <para>画像データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効</para>
			/// </returns>
			using GetVideo_Func = void*(*)(int frameNum); GetVideo_Func GetVideo;

			/// <summary>
			/// 16bitPCM形式の音声データへのポインタを取得します。
			/// </summary>
			/// <param name="start">読み込み開始サンプル番号</param>
			/// <param name="length">読み込むサンプル数</param>
			/// <param name="pReaded">読み込まれたサンプル数</param>
			/// <returns>
			/// データへのポインタ
			/// <para>音声データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効</para>
			/// </returns>
			using GetAudio_Func = void*(*)(int start, int length, int* pReaded); GetAudio_Func GetAudio;

			/// <summary>
			/// 中断するか調べます。
			/// </summary>
			/// <returns>
			/// 1 なら中断
			/// </returns>
			using IsAbort_Func = int(*)(void); IsAbort_Func IsAbort;

			/// <summary>
			/// 残り時間を表示させます。
			/// </summary>
			/// <param name="now">処理中のフレーム番号</param>
			/// <param name="total">処理する総フレーム数</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using DispRestTime_Func = int(*)(int now, int total); DispRestTime_Func DispRestTime;

			/// <summary>
			/// フラグを取得します。
			/// </summary>
			/// <param name="frameNum">フレーム番号</param>
			/// <returns>
			/// 出力フレームの情報を取得するフラグ
			/// </returns>
			using GetFlag_Func = FrameFlag(*)(int frameNum); GetFlag_Func GetFlag;

			/// <summary>
			/// プレビュー画面を更新します。
			/// <para>最後にfunc_get_videoで読み込まれたフレームが表示されます。</para>
			/// </summary>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using UpdatePreview_Func = int(*)(void); UpdatePreview_Func UpdatePreview;

			/// <summary>
			/// DIB形式の画像データを取得します。
			/// </summary>
			/// <param name="frameNum">フレーム番号</param>
			/// <param name="format">画像フォーマット( NULL = RGB24bit / 'Y''U''Y''2' = YUY2 / 'Y''C''4''8' = PIXEL_YC )
			/// <para>※PIXEL_YC形式 は YUY2フィルタモードでは使用出来ません。</para>
			/// </param>
			/// <returns>
			/// データへのポインタ
			/// <para>画像データポインタの内容は次に外部関数を使うかメインに処理を戻すまで有効</para>
			/// </returns>
			using GetVideoEx_Func = void*(*)(int frameNum, unsigned long format); GetVideoEx_Func GetVideoEx;
		};

		/// <summary>
		/// 出力プラグイン構造体
		/// </summary>
		struct OutputPluginTable final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 104 };

			/// <summary>
			/// 出力プラグインのフラグ
			/// <remarks>
			/// <para>フラグが定義されておらず、サンプルでは、NULLが指定されていました。</para>
			/// </remarks>
			/// </summary>
			int Flag;

			/// <summary>
			/// プラグインの名前
			/// </summary>
			char* pName;

			/// <summary>
			/// ファイルのフィルタ
			/// </summary>
			char* pFilefilter;

			/// <summary>
			/// プラグインの情報
			/// </summary>
			char* pInformation;

			/// <summary>
			/// DLL開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using Init_Func = int(*)(void); Init_Func Init;

			/// <summary>
			/// DLL終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using Exit_Func = int(*)(void); Exit_Func Exit;

			/// <summary>
			/// 出力時に呼ばれる関数へのポインタ
			/// </summary>
			/// <param name="pOutputInfo">出力ファイル情報構造体のアドレス</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using Output_Func = int(*)(OutputInfo* pOutputInfo); Output_Func Output;

			/// <summary>
			/// 出力設定のダイアログを要求された時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="hWnd">ウィンドウハンドル</param>
			/// <param name="hDllInstance">インスタンスハンドル</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using Config_Func = int(*)(void* hWnd, void* hDllInstance); Config_Func Config;

			/// <summary>
			/// 出力設定データを取得する時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pData">設定データを書き込むバッファへのポインタ (NULLなら設定データサイズを返すだけ)</param>
			/// <param name="size">設定データを書き込むバッファのサイズ</param>
			/// <returns>
			/// 設定データのサイズ
			/// </returns>
			using GetConfig_Func = int(*)(void *pData, int size); GetConfig_Func GetConfig;

			/// <summary>
			/// 出力設定データを設定する時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pData">設定データへのポインタ</param>
			/// <param name="size">設定データのサイズ</param>
			/// <returns>
			/// 使用した設定データのサイズ
			/// </returns>
			using SetConfig_Func = int(*)(void *pData, int size); SetConfig_Func SetConfig;

			/// <summary>
			/// 予約領域
			/// </summary>
			std::array<int, 16> Reserved;
		};

		// operators
		AU_DECLARE_ENUMCLASS_OPERATOR(OutputInfo::InfoFlag)
			AU_DECLARE_ENUMCLASS_OPERATOR(OutputInfo::FrameFlag)

		// POD check
		static_assert(std::is_pod<OutputInfo>::value, "Error: OutputInfo is non-POD.");
		static_assert(std::is_pod<OutputPluginTable>::value, "Error: OutputPluginTable is non-POD.");

		// Size check
		static_assert(sizeof(OutputInfo) == OutputInfo::Size, "Error: OutputInfo size does not fit.");
		static_assert(sizeof(OutputPluginTable) == OutputPluginTable::Size, "Error: OutputPluginTable size does not fit.");
	}

	/// <summary>
	/// Color プラグイン
	/// </summary>
	namespace Color
	{
		// Filter 名前空間を使用する
		using namespace Filter;

		/// <summary>
		/// 色情報の構造体
		/// </summary>
		struct ColorInfo final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 100 };

			/// <summary>
			/// 色処理情報のフラグ定数
			/// </summary>
			enum class InfoFlag : int {
				/// <summary>
				/// pPixelの縦方向を上下逆に処理する
				/// </summary>
				InvertHeight = 1,

				/// <summary>
				/// SSE使用
				/// </summary>
				UseSSE = 256,

				/// <summary>
				/// SSE2使用
				/// </summary>
				UseSSE2 = 512,
			};

			/// <summary>
			/// フラグ情報定数
			/// </summary>
			InfoFlag Flag;

			/// <summary>
			/// Pixel_YC構造体へのポインタ
			/// </summary>
			Pixel_YC* pYC;

			/// <summary>
			/// DIB形式データへのポインタ
			/// </summary>
			void* pPixel;

			/// <summary>
			/// DIB形式データのフォーマット( NULL = RGB24bit / 'Y''U''Y''2' = YUY2 / 'Y''C''4''8' = Pixel_YC )
			/// </summary>
			unsigned long Format;

			struct {
				/// <summary>
				/// 画像データの横サイズ
				/// </summary>
				int Width;

				/// <summary>
				/// 画像データの縦サイズ
				/// </summary>
				int Height;
			};

			/// <summary>
			/// Pixel_YC構造体の横幅のバイトサイズ
			/// </summary>
			int Line_Size;

			/// <summary>
			/// Pixel_YC構造体の画素のバイトサイズ
			/// </summary>
			int YC_Size;

			/// <summary>
			/// 指定した関数をシステムの設定値に応じたスレッド数で呼び出します
			/// <para>呼び出された関数内からWin32APIや外部関数を使用しないでください</para>
			/// </summary>
			/// <param name="pFunc">マルチスレッドで呼び出す関数</param>
			/// <param name="pParam1">呼び出す関数に渡す汎用パラメータ</param>
			/// <param name="pParam2">呼び出す関数に渡す汎用パラメータ</param>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using ExecMultiThread_Func = int(*)(MultiThread_Func pFunc, void* pParam1, void* pParam2); ExecMultiThread_Func ExecMultiThread;

			/// <summary>
			/// 予約領域
			/// </summary>
			std::array<int, 16> Reserved;
		};

		/// <summary>
		/// 色プラグインテーブル構造体
		/// </summary>
		struct ColorPluginTable final
		{
			/// <summary>
			/// データサイズ
			/// </summary>
			enum { Size = 92 };

			/// <summary>
			/// フラグ
			/// </summary>
			int Flag;

			/// <summary>
			/// プラグインの名前
			/// </summary>
			char* pName;

			/// <summary>
			/// プラグインの情報
			/// </summary>
			char* pInformation;

			/// <summary>
			/// DLL開始時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using Init_Func = int(*)(void); Init_Func Init;

			/// <summary>
			/// DLL終了時に呼ばれる関数へのポインタ (NULLなら呼ばれません)
			/// </summary>
			/// <returns>
			/// 1 なら成功
			/// </returns>
			using Exit_Func = int(*)(void); Exit_Func Exit;

			/// <summary>
			/// DIB形式の画像からからPIXEL_YC形式の画像に変換します (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pColorInfo">色情報構造体のポインタ</param>
			/// <returns>
			/// 1 なら成功
			/// <para>1以外 ならAviUtl側でデフォルト変換されます</para>
			/// </returns>
			using PixelToYC_Func = int(*)(ColorInfo* pColorInfo); PixelToYC_Func PixelToYC;

			/// <summary>
			/// PIXEL_YC形式の画像からからDIB形式の画像に変換します (NULLなら呼ばれません)
			/// </summary>
			/// <param name="pColorInfo">色情報構造体のポインタ</param>
			/// <returns>
			/// 1 なら成功
			/// <para>1以外 ならAviUtl側でデフォルト変換されます</para>
			/// </returns>
			using YCToPixel_Func = int(*)(ColorInfo* pColorInfo); YCToPixel_Func YCToPixel;

			/// <summary>
			/// 予約領域
			/// </summary>
			std::array<int, 16> Reserved;
		};

		// operators
		AU_DECLARE_ENUMCLASS_OPERATOR(ColorInfo::InfoFlag)

		// POD check
		static_assert(std::is_pod<ColorInfo>::value, "Error: ColorInfo is non-POD.");
		static_assert(std::is_pod<ColorPluginTable>::value, "Error: ColorPluginTable is non-POD.");

		// Size check
		static_assert(sizeof(ColorInfo) == ColorInfo::Size, "Error: ColorInfo size does not fit.");
		static_assert(sizeof(ColorPluginTable) == ColorPluginTable::Size, "Error: ColorPluginTable size does not fit.");
	}

#endif

};
