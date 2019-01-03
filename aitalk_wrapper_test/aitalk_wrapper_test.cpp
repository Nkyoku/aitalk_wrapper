#include "aitalk_wrapper.h"
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

int main(void) {
    AITalkWrapper::openLibrary("C:\\Program Files (x86)\\AHS\\VOICEROID2", "<ここに認証コードのシード値を入れる>");
    AITalkWrapper::loadLanguage("standard");
    //AITalkWrapper::loadPhraseDictionary("C:\\Users\\<ユーザー名>\\Documents\\VOICEROID2\\フレーズ辞書\\user.pdic");
    //AITalkWrapper::loadWordDictionary("C:\\Users\\<ユーザー名>\\Documents\\VOICEROID2\\単語辞書\\user.wdic");
    //AITalkWrapper::loadSymbolDictionary("C:\\Users\\<ユーザー名>\\Documents\\VOICEROID2\\記号ポーズ辞書\\user.sdic");
    AITalkWrapper::loadVoice("akari_44");

    AITalkWrapper aitalk;

    std::string text = "こんにちは。";
    printf("text='%s'\n", text.c_str());
    
    std::string kana;
    aitalk.textToKana(text, &kana);
    
    printf("kana='%s'\n", kana.c_str());

    std::vector<int16_t> speech;
    aitalk.kanaToSpeech(kana, &speech);

    AITalkWrapper::closeLibrary();

    if (speech.empty() == false) {
        HWAVEOUT waveout;
        WAVEFORMATEX format;
        format.wFormatTag = WAVE_FORMAT_PCM;
        format.nChannels = 1;
        format.nSamplesPerSec = 44100;
        format.nAvgBytesPerSec = 44100 * 2;
        format.nBlockAlign = 2;
        format.wBitsPerSample = 16;
        format.cbSize = 0;
        waveOutOpen(&waveout, WAVE_MAPPER, &format, NULL, NULL, CALLBACK_NULL);

        WAVEHDR hdr;
        hdr.lpData = (LPSTR)speech.data();
        hdr.dwBufferLength = speech.size() * 2;
        hdr.dwFlags = 0;
        waveOutPrepareHeader(waveout, &hdr, sizeof(WAVEHDR));
        waveOutWrite(waveout, &hdr, sizeof(WAVEHDR));
        printf("Speech playing\n");
    }

    printf("Press any key...");
    (void)getchar();
    return 0;
}
