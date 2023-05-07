/// Copyright (c) 2020 namarium
/// Licensed under the MIT License.
/// https://github.com/namarium/AviUtl_Plugin_SDK/blob/master/LICENSE

#pragma once

#include "AviUtl.h"
#define SAMPLE_API __declspec(dllexport)

/// <summary>
/// Sample
/// </summary>
namespace Sample
{
	/// <summary>
	/// プラグイン名
	/// </summary>
	AU_DECLARE_CONSTANT_STRING(PluginName, "プラグイン名");

	/// <summary>
	/// プラグインの説明
	/// </summary>
	AU_DECLARE_CONSTANT_STRING(PluginInfoText, " 説明 バージョン番号 by 作者名");
}

extern "C"
{
	/// <summary>
	/// DLLに公開する、フィルターテーブル取得関数
	/// <para>※ SDKの仕様では、ポインタを返却するインターフェースでしたが、nullptr を許容していなかったため参照に変更しています。</para>
	/// </summary>
	/// <returns>
	/// フィルター構造体の参照
	/// </returns>
	//SAMPLE_API AviUtl::Filter::FilterPluginTable& __stdcall GetFilterTable();
	
	// 下記のようにすると1つのaufファイルで複数のフィルタ構造体を渡すことが出来ます
	/*
	FILTER_DLL *filter_list[] = {&filter,&filter2,NULL};
	EXTERN_C FILTER_DLL __declspec(dllexport) ** __stdcall GetFilterTableList( void )
	{
	return (FILTER_DLL **)&filter_list;
	}
	//*/

	/*
	/// <summary>
	/// DLLに公開する、入力プラグインテーブル取得関数
	/// <para>※ SDKの仕様では、ポインタを返却するインターフェースでしたが、nullptr を許容していなかったため参照に変更しています。</para>
	/// </summary>
	/// <returns>
	/// 入力プラグインテーブル構造体の参照
	/// </returns>
	SAMPLE_API AviUtl::Input::InputPluginTable& __stdcall GetInputPluginTable();
	//*/

	/*
	/// <summary>
	/// DLLに公開する、出力プラグインテーブル取得関数
	/// <para>※ SDKの仕様では、ポインタを返却するインターフェースでしたが、nullptr を許容していなかったため参照に変更しています。</para>
	/// </summary>
	/// <returns>
	/// 出力プラグインテーブル構造体の参照
	/// </returns>
	SAMPLE_API AviUtl::Output::OutputPluginTable& __stdcall GetOutputPluginTable();
	//*/
}
