#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include "aitalk_AIAudio.h"
#include "aitalk_AITalk.h"

// VOICEROID2�̉����������C�u����(AITalk)�𑀍삷�郉�b�p�[���C�u����
class AITalkWrapper {
public:
    // �����������C�u�������J��
    // install_path : �C���X�g�[���f�B���N�g���ւ̃p�X
    //              : �ʏ�� "<Program Files>\AHS\VOICEROID2"
    // auth_code    : DLL���[�h�̍ۂ̔F�؃R�[�h�̃V�[�h�l
    //              : ���̒l�ƃ��C�Z���X�t�@�C����������DLL���g�p�\�ɂȂ�
    static void openLibrary(const std::string &install_path, const std::string &auth_code);

    // �����������C�u���������
    static void closeLibrary(void);

    // �����������C�u�������J����Ă��邩���ׂ�
    static bool IsLibraryOpened(void);
    
    // ����t�@�C����I������
    // language : ����t�@�C���̃f�B���N�g����
    //          : "standard"���邢��"standard_kansai"
    static void loadLanguage(const std::string &language_name);

    // �t���[�Y������ǂݍ���
    // path : �t���[�Y�����t�@�C���ւ̃p�X
    //      : �ʏ�� "<User>\VOICEROID2\�t���[�Y����\user.pdic"
    // �ߒl : true�̂Ƃ��t�@�C�������݂��ǂݍ��݂����������Cfalse�̂Ƃ��t�@�C�������݂��Ȃ�����
    static bool loadPhraseDictionary(const std::string &path);

    // �P�ꎫ����ǂݍ���
    // path : �t���[�Y�����t�@�C���ւ̃p�X
    //      : �ʏ�� "<User>\VOICEROID2\�P�ꎫ��\user.wdic"
    // �ߒl : true�̂Ƃ��t�@�C�������݂��ǂݍ��݂����������Cfalse�̂Ƃ��t�@�C�������݂��Ȃ�����
    static bool loadWordDictionary(const std::string &path);

    // �L���|�[�Y������ǂݍ���
    // path : �L���|�[�Y�����t�@�C���ւ̃p�X
    //      : �ʏ�� "<User>\VOICEROID2\�L���|�[�Y����\user.sdic"
    // �ߒl : true�̂Ƃ��t�@�C�������݂��ǂݍ��݂����������Cfalse�̂Ƃ��t�@�C�������݂��Ȃ�����
    static bool loadSymbolDictionary(const std::string &path);

    // �{�C�X���C�u������ǂݍ���
    // voice_name : �{�C�X�� (�L�����N�^�[���ł͂Ȃ��{�C�X���C�u�����̃f�B���N�g����)
    //            : �Ⴆ�� "akari_44"
    static void loadVoice(const std::string &voice_name);

    // �R���X�g���N�^
    AITalkWrapper(void);

    // �f�X�g���N�^
    ~AITalkWrapper();

    // �e�L�X�g�������ɕϊ�����
    // �����ɂ�JEITA�W���̔����L����AITalk�Ǝ��̐���L�����܂܂��
    // JEITA IT-4006���Q�Ƃ���
    void textToKana(const std::string &text, std::string *kana, int timeout = 0);

    // �����������ɕϊ�����
    void kanaToSpeech(const std::string &kana, std::vector<int16_t> *data, int timeout = 0);

private:
    // �{�C�X���C�u�����̃T���v�����O���g�� [Hz]
    static const int VOICE_SAMPLERATE = 44100;

    // ���������̃^�C���A�E�g���ԁH [ms]
    static const int TIMEOUT = 10000;

    // �����ϊ��̃o�b�t�@�T�C�Y [byte]
    static const uint32_t KANA_BUFFER_SIZE = 0x1000;

    // �����ϊ��̃o�b�t�@�T�C�Y [byte]
    static const uint32_t SPEECH_BUFFER_SIZE = 0x10000;

    // �����������C�u�����̉��������֐��̌^
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

    // �����������C�u�����̃C���X�g�[���f�B���N�g��
    static std::string m_InstallDirectory;

    // �����������C�u�����̃��W���[���n���h��
    static void *m_ModuleHandle;

    // �����������C�u�����̉��������֐��|�C���^
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

    // ���b�p�[���C�u����������������
    static void clearLibrary(void);

    // �e�L�X�g�ϊ��̍ۂ̃R�[���o�b�N�֐�
    static int __stdcall callbackTextBuf(AITalkEventReasonCode reason_code, int32_t job_id, void *user_data);

    // �����ϊ��̍ۂ̃R�[���o�b�N�֐�
    static int __stdcall callbackRawBuf(AITalkEventReasonCode reason_code, int32_t job_id, uint64_t tick, void *user_data);

    // �����Ƃ��Ȃ̑Ή��t���̏�񂪕Ԃ����R�[���o�b�N�֐�
    static int __stdcall callbackEventTts(AITalkEventReasonCode reason_code, int32_t job_id, uint64_t tick, const char *name, void *user_data);

    // �����ϊ��̍ۂ̃o�b�t�@
    std::vector<char> m_KanaBuffer;

    // �����o�͐�̃|�C���^
    std::string *m_KanaOutput;

    // �����ϊ��̍ۂ̃o�b�t�@
    std::vector<int16_t> m_SpeechBuffer;

    // �����o�͐�̃|�C���^
    std::vector<int16_t> *m_SpeechOutput;

    // �ϊ��̏I����ʒm���邽�߂̃C�x���g�n���h��
    void *m_CloseEventHandle;
};
