#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "aitalk_AIAudio.h"
#include "aitalk_AITalk.h"

// VOICEROID2の音声合成ライブラリ(AITalk)を操作するラッパーライブラリ
class AITalkWrapper {
public:
    // 音声合成ライブラリを開く
    // install_path : インストールディレクトリへのパス
    //              : 通常は "<Program Files>\AHS\VOICEROID2"
    // auth_code    : DLLロードの際の認証コードのシード値
    //              : この値とライセンスファイルが揃ってDLLが使用可能になる
    static void openLibrary(const std::string &install_path, const std::string &auth_code);

    // 音声合成ライブラリを閉じる
    static void closeLibrary(void);

    // 音声合成ライブラリが開かれているか調べる
    static bool IsLibraryOpened(void);
    
    // 言語ファイルを選択する
    // language : 言語ファイルのディレクトリ名
    //          : "standard"あるいは"standard_kansai"
    static void loadLanguage(const std::string &language_name);

    // フレーズ辞書を読み込む
    // path : フレーズ辞書ファイルへのパス
    //      : 通常は "<User>\VOICEROID2\フレーズ辞書\user.pdic"
    // 戻値 : trueのときファイルが存在し読み込みが成功した，falseのときファイルが存在しなかった
    static bool loadPhraseDictionary(const std::string &path);

    // 単語辞書を読み込む
    // path : フレーズ辞書ファイルへのパス
    //      : 通常は "<User>\VOICEROID2\単語辞書\user.wdic"
    // 戻値 : trueのときファイルが存在し読み込みが成功した，falseのときファイルが存在しなかった
    static bool loadWordDictionary(const std::string &path);

    // 記号ポーズ辞書を読み込む
    // path : 記号ポーズ辞書ファイルへのパス
    //      : 通常は "<User>\VOICEROID2\記号ポーズ辞書\user.sdic"
    // 戻値 : trueのときファイルが存在し読み込みが成功した，falseのときファイルが存在しなかった
    static bool loadSymbolDictionary(const std::string &path);

    // ボイスライブラリを読み込む
    // voice_name : ボイス名 (キャラクター名ではなくボイスライブラリのディレクトリ名)
    //            : 例えば "akari_44"
    static void loadVoice(const std::string &voice_name);

    // コンストラクタ
    AITalkWrapper(void);

    // デストラクタ
    ~AITalkWrapper();

    // テキストを仮名に変換する
    // 仮名にはJEITA標準の発音記号やAITalk独自の制御記号も含まれる
    // JEITA IT-4006を参照せよ
    void textToKana(const std::string &text, std::string *kana, int timeout = 0);

    // 仮名を音声に変換する
    void kanaToSpeech(const std::string &kana, std::vector<int16_t> *data, int timeout = 0);

private:
    // ボイスライブラリのサンプリング周波数 [Hz]
    static const int VOICE_SAMPLERATE = 44100;

    // 音声合成のタイムアウト時間？ [ms]
    static const int TIMEOUT = 10000;

    // 仮名変換のバッファサイズ [byte]
    static const uint32_t KANA_BUFFER_SIZE = 0x1000;

    // 音声変換のバッファサイズ [byte]
    static const uint32_t SPEECH_BUFFER_SIZE = 0x10000;

    // 音声合成ライブラリの音声合成関数の型
    using Type_AITalkAPI_CloseKana = AITalkResultCode(__stdcall *)(int32_t, int32_t);
    using Type_AITalkAPI_CloseSpeech = AITalkResultCode(__stdcall *)(int32_t, int32_t);
    using Type_AITalkAPI_End = AITalkResultCode(__stdcall *)(void);
    using Type_AITalkAPI_GetData = AITalkResultCode(__stdcall *)(int32_t, int16_t*, uint32_t, uint32_t*);
    using Type_AITalkAPI_GetJeitaControl = AITalkResultCode(__stdcall *)(int32_t, const char*);
    using Type_AITalkAPI_GetKana = AITalkResultCode(__stdcall *)(int32_t, char*, uint32_t, uint32_t*, uint32_t*);
    using Type_AITalkAPI_GetParam = AITalkResultCode(__stdcall *)(AITalk_TTtsParam*, uint32_t*);
    using Type_AITalkAPI_GetStatus = AITalkResultCode(__stdcall *)(int32_t, AITalkStatusCode*);
    using Type_AITalkAPI_Init = AITalkResultCode(__stdcall *)(AITalk_TConfig*);
    using Type_AITalkAPI_LangClear = AITalkResultCode(__stdcall *)(void);
    using Type_AITalkAPI_LangLoad = AITalkResultCode(__stdcall *)(const char*);
    using Type_AITalkAPI_LicenseDate = AITalkResultCode(__stdcall *)(char*);
    using Type_AITalkAPI_LicenseInfo = AITalkResultCode(__stdcall *)(const char*, char*, uint32_t, uint32_t*);
    using Type_AITalkAPI_ModuleFlag = AITalkResultCode(__stdcall *)(void);
    using Type_AITalkAPI_ReloadPhraseDic = AITalkResultCode(__stdcall *)(const char*);
    using Type_AITalkAPI_ReloadSymbolDic = AITalkResultCode(__stdcall *)(const char*);
    using Type_AITalkAPI_ReloadWordDic = AITalkResultCode(__stdcall *)(const char*);
    using Type_AITalkAPI_SetParam = AITalkResultCode(__stdcall *)(const AITalk_TTtsParam*);
    using Type_AITalkAPI_TextToKana = AITalkResultCode(__stdcall *)(int32_t*, AITalk_TJobParam*, const char*);
    using Type_AITalkAPI_TextToSpeech = AITalkResultCode(__stdcall *)(int32_t*, AITalk_TJobParam*, const char*);
    using Type_AITalkAPI_VersionInfo = AITalkResultCode(__stdcall *)(int32_t, char*, uint32_t, uint32_t*);
    using Type_AITalkAPI_VoiceClear = AITalkResultCode(__stdcall *)(void);
    using Type_AITalkAPI_VoiceLoad = AITalkResultCode(__stdcall *)(const char*);

    // 音声合成ライブラリのインストールディレクトリ
    static std::string m_InstallDirectory;

    // 音声合成ライブラリのモジュールハンドル
    static void *m_ModuleHandle;

    // 音声合成ライブラリの音声合成関数ポインタ
    static Type_AITalkAPI_CloseKana m_AITalkAPI_CloseKana;
    static Type_AITalkAPI_CloseSpeech m_AITalkAPI_CloseSpeech;
    static Type_AITalkAPI_End m_AITalkAPI_End;
    static Type_AITalkAPI_GetData m_AITalkAPI_GetData;
    static Type_AITalkAPI_GetJeitaControl m_AITalkAPI_GetJeitaControl;
    static Type_AITalkAPI_GetKana m_AITalkAPI_GetKana;
    static Type_AITalkAPI_GetParam m_AITalkAPI_GetParam;
    static Type_AITalkAPI_GetStatus m_AITalkAPI_GetStatus;
    static Type_AITalkAPI_Init m_AITalkAPI_Init;
    static Type_AITalkAPI_LangClear m_AITalkAPI_LangClear;
    static Type_AITalkAPI_LangLoad m_AITalkAPI_LangLoad;
    static Type_AITalkAPI_LicenseDate m_AITalkAPI_LicenseDate;
    static Type_AITalkAPI_LicenseInfo m_AITalkAPI_LicenseInfo;
    static Type_AITalkAPI_ModuleFlag m_AITalkAPI_ModuleFlag;
    static Type_AITalkAPI_ReloadPhraseDic m_AITalkAPI_ReloadPhraseDic;
    static Type_AITalkAPI_ReloadSymbolDic m_AITalkAPI_ReloadSymbolDic;
    static Type_AITalkAPI_ReloadWordDic m_AITalkAPI_ReloadWordDic;
    static Type_AITalkAPI_SetParam m_AITalkAPI_SetParam;
    static Type_AITalkAPI_TextToKana  m_AITalkAPI_TextToKana;
    static Type_AITalkAPI_TextToSpeech m_AITalkAPI_TextToSpeech;
    static Type_AITalkAPI_VersionInfo m_AITalkAPI_VersionInfo;
    static Type_AITalkAPI_VoiceClear m_AITalkAPI_VoiceClear;
    static Type_AITalkAPI_VoiceLoad m_AITalkAPI_VoiceLoad;

    // ラッパーライブラリを初期化する
    static void clearLibrary(void);

    // テキスト変換の際のコールバック関数
    static int __stdcall callbackTextBuf(AITalkEventReasonCode reason_code, int32_t job_id, void *user_data);

    // 音声変換の際のコールバック関数
    static int __stdcall callbackRawBuf(AITalkEventReasonCode reason_code, int32_t job_id, uint64_t tick, void *user_data);

    // 音声とかなの対応付けの情報が返されるコールバック関数
    static int __stdcall callbackEventTts(AITalkEventReasonCode reason_code, int32_t job_id, uint64_t tick, const char *name, void *user_data);

    // 仮名変換の際のバッファ
    std::vector<char> m_KanaBuffer;

    // 仮名出力先のポインタ
    std::string *m_KanaOutput;

    // 音声変換の際のバッファ
    std::vector<int16_t> m_SpeechBuffer;

    // 音声出力先のポインタ
    std::vector<int16_t> *m_SpeechOutput;

    // 変換の終了を通知するためのイベントハンドル
    void *m_CloseEventHandle;
};
