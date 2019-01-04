#include "aitalk_wrapper.h"
#include <cerrno>
#include <Windows.h>

// モジュールから関数をロードする
template<typename T> static bool getFunction(void *module_handle, const char *function_name, T **function) {
    FARPROC fp = GetProcAddress(reinterpret_cast<HMODULE>(module_handle), function_name);
    *function = reinterpret_cast<T*>(fp);
    return (fp != nullptr);
}

// 一時的にカレントディレクトリを変更するクラス
class CdChanger {
public:
    CdChanger(const std::string &new_path) {
        std::vector<char> buf(MAX_PATH);
        int result = GetCurrentDirectoryA(MAX_PATH, buf.data());
        if (result == 0) {
            throw std::errc::invalid_argument;
        }
        if (buf.size() < static_cast<size_t>(result)) {
            buf.resize(result);
            result = GetCurrentDirectoryA(result, buf.data());
            if ((result == 0) || (buf.size() < static_cast<size_t>(result))) {
                throw std::errc::invalid_argument;
            }
        }
        m_Cd = buf.data();
        if (SetCurrentDirectoryA(new_path.c_str()) == FALSE) {
            throw std::errc::no_such_file_or_directory;
        }
    }

    ~CdChanger() {
        SetCurrentDirectoryA(m_Cd.c_str());
    }

private:
    std::string m_Cd;
};

void AITalkWrapper::openLibrary(const std::string &install_path, const std::string &auth_code) {
    closeLibrary();

    m_InstallDirectory = install_path;

    // DLLをロードする
    std::string module_path = install_path + "\\aitalked.dll";
    m_ModuleHandle = LoadLibraryA(module_path.c_str());
    if (m_ModuleHandle == nullptr) {
        throw std::errc::no_such_file_or_directory;
    }

    // 関数ポインタを取得する
    bool ok = true;
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_CloseKana@8", &m_AITalkAPI_CloseKana);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_CloseSpeech@8", &m_AITalkAPI_CloseSpeech);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_End@0", &m_AITalkAPI_End);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_GetData@16", &m_AITalkAPI_GetData);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_GetJeitaControl@8", &m_AITalkAPI_GetJeitaControl);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_GetKana@20", &m_AITalkAPI_GetKana);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_GetParam@8", &m_AITalkAPI_GetParam);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_GetStatus@8", &m_AITalkAPI_GetStatus);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_Init@4", &m_AITalkAPI_Init);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_LangClear@0", &m_AITalkAPI_LangClear);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_LangLoad@4", &m_AITalkAPI_LangLoad);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_LicenseDate@4", &m_AITalkAPI_LicenseDate);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_LicenseInfo@16", &m_AITalkAPI_LicenseInfo);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_ModuleFlag@0", &m_AITalkAPI_ModuleFlag);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_ReloadPhraseDic@4", &m_AITalkAPI_ReloadPhraseDic);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_ReloadSymbolDic@4", &m_AITalkAPI_ReloadSymbolDic);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_ReloadWordDic@4", &m_AITalkAPI_ReloadWordDic);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_SetParam@4", &m_AITalkAPI_SetParam);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_TextToKana@12", &m_AITalkAPI_TextToKana);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_TextToSpeech@12", &m_AITalkAPI_TextToSpeech);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_VersionInfo@16", &m_AITalkAPI_VersionInfo);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_VoiceClear@0", &m_AITalkAPI_VoiceClear);
    ok &= getFunction(m_ModuleHandle, "_AITalkAPI_VoiceLoad@4", &m_AITalkAPI_VoiceLoad);
    if (ok == false) {
        closeLibrary();
        throw std::errc::function_not_supported;
    }

    // ライブラリを初期化する
    AITalkResultCode result;
    std::string voice_db_dir = install_path + "\\Voice";
    std::string license_path = install_path + "\\aitalk.lic";
    AITalk_TConfig config;
    config.hzVoiceDB = VOICE_SAMPLERATE;
    config.dirVoiceDBS = voice_db_dir.c_str();
    config.msecTimeout = TIMEOUT;
    config.pathLicense = license_path.c_str();
    config.codeAuthSeed = auth_code.c_str();
    config.__reserved__ = 0;
    result = m_AITalkAPI_Init(&config);
    if (result != AITALKERR_SUCCESS) {
        throw result;
    }
}

void AITalkWrapper::closeLibrary(void) {
    AITalkResultCode result = AITALKERR_SUCCESS;
    if (m_ModuleHandle != nullptr) {
        // ライブラリを閉じる
        result = m_AITalkAPI_End();
    }
    clearLibrary();
    if (result != AITALKERR_SUCCESS) {
        throw result;
    }
}

void AITalkWrapper::clearLibrary(void) {
    m_InstallDirectory.clear();
    if (m_ModuleHandle != nullptr) {
        FreeLibrary(reinterpret_cast<HMODULE>(m_ModuleHandle));
        m_ModuleHandle = nullptr;
    }
    m_AITalkAPI_CloseKana = nullptr;
    m_AITalkAPI_CloseSpeech = nullptr;
    m_AITalkAPI_End = nullptr;
    m_AITalkAPI_GetData = nullptr;
    m_AITalkAPI_GetJeitaControl = nullptr;
    m_AITalkAPI_GetKana = nullptr;
    m_AITalkAPI_GetParam = nullptr;
    m_AITalkAPI_GetStatus = nullptr;
    m_AITalkAPI_Init = nullptr;
    m_AITalkAPI_LangClear = nullptr;
    m_AITalkAPI_LangLoad = nullptr;
    m_AITalkAPI_LicenseDate = nullptr;
    m_AITalkAPI_LicenseInfo = nullptr;
    m_AITalkAPI_ModuleFlag = nullptr;
    m_AITalkAPI_ReloadPhraseDic = nullptr;
    m_AITalkAPI_ReloadSymbolDic = nullptr;
    m_AITalkAPI_ReloadWordDic = nullptr;
    m_AITalkAPI_SetParam = nullptr;
    m_AITalkAPI_TextToKana = nullptr;
    m_AITalkAPI_TextToSpeech = nullptr;
    m_AITalkAPI_VersionInfo = nullptr;
    m_AITalkAPI_VoiceClear = nullptr;
    m_AITalkAPI_VoiceLoad = nullptr;
}

bool AITalkWrapper::IsLibraryOpened(void) {
    return (m_ModuleHandle != nullptr);
}

void AITalkWrapper::loadLanguage(const std::string &language_name) {
    if (IsLibraryOpened() == false) {
        throw std::errc::function_not_supported;
    }
    std::string language_path = m_InstallDirectory + "\\Lang\\" + language_name;
    AITalkResultCode result;
    {
        CdChanger changer(m_InstallDirectory);
        result = m_AITalkAPI_LangLoad(language_path.c_str());
    }
    if (result != AITALKERR_SUCCESS) {
        throw std::errc::no_such_file_or_directory;
    }
}

bool AITalkWrapper::loadPhraseDictionary(const std::string &path) {
    if (IsLibraryOpened() == false) {
        throw std::errc::function_not_supported;
    }
    AITalkResultCode result;
    result = m_AITalkAPI_ReloadPhraseDic(nullptr);
    result = m_AITalkAPI_ReloadPhraseDic(path.c_str());
    switch (result) {
    case AITALKERR_SUCCESS:
    case AITALKERR_USERDIC_NOENTRY:
        return true;
    
    case AITALKERR_FILE_NOT_FOUND:
    case AITALKERR_USERDIC_LOCKED:
        return false;

    default:
        throw result;
    }
}

bool AITalkWrapper::loadWordDictionary(const std::string &path) {
    if (IsLibraryOpened() == false) {
        throw std::errc::function_not_supported;
    }
    AITalkResultCode result;
    result = m_AITalkAPI_ReloadWordDic(nullptr);
    result = m_AITalkAPI_ReloadWordDic(path.c_str());
    switch (result) {
    case AITALKERR_SUCCESS:
    case AITALKERR_USERDIC_NOENTRY:
        return true;

    case AITALKERR_FILE_NOT_FOUND:
    case AITALKERR_USERDIC_LOCKED:
        return false;

    default:
        throw result;
    }
}

bool AITalkWrapper::loadSymbolDictionary(const std::string &path) {
    if (IsLibraryOpened() == false) {
        throw std::errc::function_not_supported;
    }
    AITalkResultCode result;
    result = m_AITalkAPI_ReloadSymbolDic(nullptr);
    result = m_AITalkAPI_ReloadSymbolDic(path.c_str());
    switch (result) {
    case AITALKERR_SUCCESS:
    case AITALKERR_USERDIC_NOENTRY:
        return true;

    case AITALKERR_FILE_NOT_FOUND:
    case AITALKERR_USERDIC_LOCKED:
        return false;

    default:
        throw result;
    }
}

void AITalkWrapper::loadVoice(const std::string &voice_name) {
    if (IsLibraryOpened() == false) {
        throw std::errc::function_not_supported;
    }

    // ボイスライブラリを読み込む
    AITalkResultCode result;
    result = m_AITalkAPI_VoiceLoad(voice_name.c_str());
    if (result != AITALKERR_SUCCESS) {
        throw std::errc::no_such_file_or_directory;
    }
    
    // 動作パラメータの初期値を取得する
    uint32_t size = 0;
    result = m_AITalkAPI_GetParam(nullptr, &size);
    if ((result != AITALKERR_INSUFFICIENT) || (static_cast<size_t>(size) < sizeof(AITalk_TTtsParam))) {
        throw std::errc::invalid_argument;
    }
    std::vector<uint8_t> param_buffer(size);
    AITalk_TTtsParam *param = reinterpret_cast<AITalk_TTtsParam*>(param_buffer.data());
    param->size = size;
    result = m_AITalkAPI_GetParam(param, &size);
    if (result != AITALKERR_SUCCESS) {
        throw std::errc::invalid_argument;
    }

    // コールバック関数を設定する
    param->procTextBuf = callbackTextBuf;
    param->procRawBuf = callbackRawBuf;
    param->procEventTts = callbackEventTts;
    param->extendFormat = static_cast<ExtendFormat>(JeitaRuby | AutoBookmark);
    result = m_AITalkAPI_SetParam(param);
    if (result != AITALKERR_SUCCESS) {
        throw std::errc::invalid_argument;
    }
}

AITalkWrapper::AITalkWrapper(void)
    : m_KanaBuffer(KANA_BUFFER_SIZE), m_KanaOutput(nullptr)
    , m_SpeechBuffer(SPEECH_BUFFER_SIZE), m_SpeechOutput(nullptr)
{
    m_CloseEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (m_CloseEventHandle == NULL) {
        throw std::errc::invalid_argument;
    }
}

AITalkWrapper::~AITalkWrapper() {
    CloseHandle(reinterpret_cast<HANDLE>(m_CloseEventHandle));
}

void AITalkWrapper::textToKana(const std::string &text, std::string *kana, int timeout) {
    if (IsLibraryOpened() == false) {
        throw std::errc::not_connected;
    }
    if (kana == nullptr) {
        throw std::errc::invalid_argument;
    }
    m_KanaOutput = kana;
    m_KanaOutput->clear();

    ResetEvent(reinterpret_cast<HANDLE>(m_CloseEventHandle));
    
    // 変換を開始する
    AITalk_TJobParam job_param;
    job_param.modeInOut = AITALKIOMODE_PLAIN_TO_AIKANA;
    job_param.userData = this;
    int32_t job_id = 0;
    AITalkResultCode result;
    result = m_AITalkAPI_TextToKana(&job_id, &job_param, text.c_str());
    if (result != AITALKERR_SUCCESS) {
        throw std::errc::invalid_argument;
    }

    // 変換の終了を待つ
    // timeoutで与えられた時間だけ待つ
    DWORD timeout_winapi = (0 < timeout) ? timeout : INFINITE;
    DWORD result_winapi;
    result_winapi = WaitForSingleObject(reinterpret_cast<HANDLE>(m_CloseEventHandle), timeout_winapi);

    // 変換を終了する
    result = m_AITalkAPI_CloseKana(job_id, 0);
    if (result != AITALKERR_SUCCESS) {
        throw std::errc::invalid_argument;
    }
    if (result_winapi != WAIT_OBJECT_0) {
        // タイムアウトしたので例外を返す
        kana->clear();
        throw std::errc::timed_out;
    }
}

void AITalkWrapper::kanaToSpeech(const std::string &kana, std::vector<int16_t> *data, int timeout) {
    if (IsLibraryOpened() == false) {
        throw std::errc::not_connected;
    }
    if (data == nullptr) {
        throw std::errc::invalid_argument;
    }
    m_SpeechOutput = data;
    m_SpeechOutput->clear();

    ResetEvent(reinterpret_cast<HANDLE>(m_CloseEventHandle));

    // 変換を開始する
    AITalk_TJobParam job_param;
    job_param.modeInOut = AITALKIOMODE_AIKANA_TO_WAVE;
    job_param.userData = this;
    int32_t job_id = 0;
    AITalkResultCode result;
    result = m_AITalkAPI_TextToSpeech(&job_id, &job_param, kana.c_str());
    if (result != AITALKERR_SUCCESS) {
        throw std::errc::invalid_argument;
    }

    // 変換の終了を待つ
    // timeoutで与えられた時間だけ待つ
    DWORD timeout_winapi = (0 < timeout) ? timeout : INFINITE;
    DWORD result_winapi;
    result_winapi = WaitForSingleObject(reinterpret_cast<HANDLE>(m_CloseEventHandle), timeout_winapi);

    // 変換を終了する
    result = m_AITalkAPI_CloseSpeech(job_id, 0);
    if (result != AITALKERR_SUCCESS) {
        throw std::errc::invalid_argument;
    }
    if (result_winapi != WAIT_OBJECT_0) {
        // タイムアウトしたので例外を返す
        data->clear();
        throw std::errc::timed_out;
    }
}

std::string AITalkWrapper::m_InstallDirectory;

void *AITalkWrapper::m_ModuleHandle;

AITalkWrapper::Type_AITalkAPI_CloseKana AITalkWrapper::m_AITalkAPI_CloseKana;
AITalkWrapper::Type_AITalkAPI_CloseSpeech AITalkWrapper::m_AITalkAPI_CloseSpeech;
AITalkWrapper::Type_AITalkAPI_End AITalkWrapper::m_AITalkAPI_End;
AITalkWrapper::Type_AITalkAPI_GetData AITalkWrapper::m_AITalkAPI_GetData;
AITalkWrapper::Type_AITalkAPI_GetJeitaControl AITalkWrapper::m_AITalkAPI_GetJeitaControl;
AITalkWrapper::Type_AITalkAPI_GetKana AITalkWrapper::m_AITalkAPI_GetKana;
AITalkWrapper::Type_AITalkAPI_GetParam AITalkWrapper::m_AITalkAPI_GetParam;
AITalkWrapper::Type_AITalkAPI_GetStatus AITalkWrapper::m_AITalkAPI_GetStatus;
AITalkWrapper::Type_AITalkAPI_Init AITalkWrapper::m_AITalkAPI_Init;
AITalkWrapper::Type_AITalkAPI_LangClear AITalkWrapper::m_AITalkAPI_LangClear;
AITalkWrapper::Type_AITalkAPI_LangLoad AITalkWrapper::m_AITalkAPI_LangLoad;
AITalkWrapper::Type_AITalkAPI_LicenseDate AITalkWrapper::m_AITalkAPI_LicenseDate;
AITalkWrapper::Type_AITalkAPI_LicenseInfo AITalkWrapper::m_AITalkAPI_LicenseInfo;
AITalkWrapper::Type_AITalkAPI_ModuleFlag AITalkWrapper::m_AITalkAPI_ModuleFlag;
AITalkWrapper::Type_AITalkAPI_ReloadPhraseDic AITalkWrapper::m_AITalkAPI_ReloadPhraseDic;
AITalkWrapper::Type_AITalkAPI_ReloadSymbolDic AITalkWrapper::m_AITalkAPI_ReloadSymbolDic;
AITalkWrapper::Type_AITalkAPI_ReloadWordDic AITalkWrapper::m_AITalkAPI_ReloadWordDic;
AITalkWrapper::Type_AITalkAPI_SetParam AITalkWrapper::m_AITalkAPI_SetParam;
AITalkWrapper::Type_AITalkAPI_TextToKana  AITalkWrapper::m_AITalkAPI_TextToKana;
AITalkWrapper::Type_AITalkAPI_TextToSpeech AITalkWrapper::m_AITalkAPI_TextToSpeech;
AITalkWrapper::Type_AITalkAPI_VersionInfo AITalkWrapper::m_AITalkAPI_VersionInfo;
AITalkWrapper::Type_AITalkAPI_VoiceClear AITalkWrapper::m_AITalkAPI_VoiceClear;
AITalkWrapper::Type_AITalkAPI_VoiceLoad AITalkWrapper::m_AITalkAPI_VoiceLoad;

int __stdcall AITalkWrapper::callbackTextBuf(AITalkEventReasonCode reason_code, int32_t job_id, void *user_data) {
    AITalkWrapper *aitalk = reinterpret_cast<AITalkWrapper*>(user_data);
    //if (reason_code == AITALKEVENT_TEXTBUF_FULL) {
    //    printf("AITALKEVENT_TEXTBUF_FULL(job=%d)\n", job_id);
    //} else if (reason_code == AITALKEVENT_TEXTBUF_FLUSH) {
    //    printf("AITALKEVENT_TEXTBUF_FLUSH(job=%d)\n", job_id);
    //} else if (reason_code == AITALKEVENT_TEXTBUF_CLOSE) {
    //    printf("AITALKEVENT_TEXTBUF_CLOSE(job=%d)\n", job_id);
    //} else {
    //    printf("AITALKEVENT_%d(job=%d)\n", reason_code, job_id);
    //}
    if ((reason_code == AITALKEVENT_TEXTBUF_FULL) || (reason_code == AITALKEVENT_TEXTBUF_FLUSH) || (reason_code == AITALKEVENT_TEXTBUF_CLOSE)) {
        char *buffer = aitalk->m_KanaBuffer.data();
        const uint32_t buffer_size = static_cast<uint32_t>(aitalk->m_KanaBuffer.size());
        uint32_t read_bytes;
        do {
            AITalkResultCode result;
            uint32_t pos;
            result = m_AITalkAPI_GetKana(job_id, buffer, buffer_size, &read_bytes, &pos);
            //printf("AITalkAPI_GetKana(job=%d, size=%d, pos=%d) => result=%d\n", job_id, read_bytes, pos, result);
            if (result != AITALKERR_SUCCESS) {
                break;
            }
            aitalk->m_KanaOutput->append(aitalk->m_KanaBuffer.data(), read_bytes);
        } while ((buffer_size - 1) <= read_bytes);
        if (reason_code != AITALKEVENT_TEXTBUF_CLOSE) {
            return 0;
        }
    }
    SetEvent(aitalk->m_CloseEventHandle);
    return 0;
}

int __stdcall AITalkWrapper::callbackRawBuf(AITalkEventReasonCode reason_code, int32_t job_id, uint64_t tick, void *user_data) {
    AITalkWrapper *aitalk = reinterpret_cast<AITalkWrapper*>(user_data);
    //if (reason_code == AITALKEVENT_RAWBUF_FULL) {
    //    printf("AITALKEVENT_RAWBUF_FULL(job=%d, tick=%lld)\n", job_id, tick);
    //} else if (reason_code == AITALKEVENT_RAWBUF_FLUSH) {
    //    printf("AITALKEVENT_RAWBUF_FLUSH(job=%d, tick=%lld)\n", job_id, tick);
    //} else if (reason_code == AITALKEVENT_RAWBUF_CLOSE) {
    //    printf("AITALKEVENT_RAWBUF_CLOSE(job=%d, tick=%lld)\n", job_id, tick);
    //} else {
    //    printf("AITALKEVENT_%d(job=%d, tick=%lld)\n", reason_code, job_id, tick);
    //}
    if ((reason_code == AITALKEVENT_RAWBUF_FULL) || (reason_code == AITALKEVENT_RAWBUF_FLUSH) || (reason_code == AITALKEVENT_RAWBUF_CLOSE)) {
        int16_t *buffer = aitalk->m_SpeechBuffer.data();
        const uint32_t buffer_size = static_cast<uint32_t>(aitalk->m_SpeechBuffer.size());
        uint32_t read_samples;
        do {
            AITalkResultCode result;
            result = m_AITalkAPI_GetData(job_id, buffer, buffer_size, &read_samples);
            //printf("m_AITalkAPI_GetData(job=%d, samples=%d) => result=%d\n", job_id, read_samples, result);
            if (result != AITALKERR_SUCCESS) {
                break;
            }
            aitalk->m_SpeechOutput->insert(aitalk->m_SpeechOutput->end(), aitalk->m_SpeechBuffer.begin(), aitalk->m_SpeechBuffer.begin() + read_samples);
        } while (buffer_size <= read_samples);
        if (reason_code != AITALKEVENT_RAWBUF_CLOSE) {
            return 0;
        }
    }
    SetEvent(aitalk->m_CloseEventHandle);
    return 0;
}

int __stdcall AITalkWrapper::callbackEventTts(AITalkEventReasonCode reason_code, int32_t job_id, uint64_t tick, const char *name, void *user_data) {
    //printf("ProcEventTTS(%d, %d, %lld, '%s', %p)\n", reason_code, job_id, tick, name, user_data);
    // name=発音記号あるいは数
    // name=数のとき、現在の文節が終わる入力文章の位置を表す
    return 0;
}
