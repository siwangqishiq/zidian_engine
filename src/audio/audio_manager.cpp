#include "audio/audio_manager.h"

#ifdef __APPLE__
    #define MA_NO_RUNTIME_LINKING
#endif

#define MINIAUDIO_IMPLEMENTATION
#include "audio/miniaudio.h"

#include "utils/log.h"
#include "resource/asset_manager.h"

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>

namespace zidian {

namespace {

// AudioManager 是单例，因此整个音频系统只需要一个 ma_engine。
// ma_engine 内部管理唯一的输出设备以及音频混音图。
ma_engine gAudioEngine{};
bool gAudioEngineInitialized = false;

bool ensureAudioEngine() {
    if (gAudioEngineInitialized) {
        return true;
    }

    ma_result result = ma_engine_init(nullptr, &gAudioEngine);
    if (result != MA_SUCCESS) {
        Log::e("audio", "ma_engine_init failed: %d", result);
        return false;
    }

    gAudioEngineInitialized = true;
    return true;
}

} // namespace

class AudioEntity {
public:
    std::string name;

    // 保留原始文件数据。decoder 使用内存作为输入数据源，
    // 因此在 decoder / sound 释放前不能释放这块内存。
    std::unique_ptr<uint8_t[]> pData;

    ma_decoder decoder{};
    ma_sound sound{};

    bool isPlay = false;
    bool isLoop = false;
    bool isReleased = false;

    ma_uint64 pcmFrameLength = 0;
    double duration = 0.0;
    ma_uint64 readedFrame = 0;

    ProgressUpdateCallbackFn onPlayProgressUpdate = nullptr;
    PlayEndCallbackFn onPlayEndCallback = nullptr;

    void setPlayProgressUpdateCallback(ProgressUpdateCallbackFn callback) {
        onPlayProgressUpdate = callback;
    }

    void setPlayEndCallback(PlayEndCallbackFn callback) {
        onPlayEndCallback = callback;
    }
};

// ma_sound 自然播放结束时由 miniaudio 调用。
// 注意：该回调运行在音频线程，不要在其中执行阻塞操作或释放其他音频资源。
static void onSoundEnd(void* pUserData, ma_sound* /*pSound*/) {
    auto* entity = static_cast<AudioEntity*>(pUserData);
    if (entity == nullptr || entity->isReleased) {
        return;
    }

    entity->isPlay = false;
    entity->readedFrame = 0;

    if (entity->onPlayEndCallback != nullptr) {
        entity->onPlayEndCallback(entity->name);
    }
}

std::unique_ptr<AudioManager>& AudioManager::getInstance() {
    static std::unique_ptr<AudioManager> instance_;
    if (instance_ == nullptr) {
        instance_ = std::make_unique<AudioManager>();
    }
    return instance_;
}

std::shared_ptr<AudioEntity> AudioManager::loadAudioEntity(
    std::string path,
    bool playLoop,
    bool asset_path
) {
    if (!ensureAudioEngine()) {
        return nullptr;
    }

    long fileSize = 0;
    std::unique_ptr<uint8_t[]> fileData = asset_path
        ? AssetManager::getInstance()->readAssetFileAsBinary(path, fileSize)
        : ReadFileAsBinary(path, fileSize);

    if (fileData == nullptr || fileSize <= 0) {
        Log::e("audio", "loadAudioResource read file %s failed", path.c_str());
        return nullptr;
    }

    auto entity = std::make_shared<AudioEntity>();
    entity->name = path;
    entity->pData = std::move(fileData);
    entity->isLoop = playLoop;

    ma_result result = ma_decoder_init_memory(
        entity->pData.get(),
        static_cast<size_t>(fileSize),
        nullptr,
        &entity->decoder
    );

    if (result != MA_SUCCESS) {
        Log::e("audio", "loadAudioResource %s create decoder error: %d",
               path.c_str(), result);
        entity->pData.reset();
        return nullptr;
    }

    Log::i(
        "audio",
        "%s info channel: %d sampleRate: %d outputFormat: %d",
        path.c_str(),
        entity->decoder.outputChannels,
        entity->decoder.outputSampleRate,
        entity->decoder.outputFormat
    );

    ma_uint64 pcmLength = 0;
    result = ma_data_source_get_length_in_pcm_frames(
        &entity->decoder,
        &pcmLength
    );

    if (result != MA_SUCCESS) {
        Log::e("audio", "get audio frame length failed: %d", result);
        ma_decoder_uninit(&entity->decoder);
        entity->pData.reset();
        return nullptr;
    }

    entity->pcmFrameLength = pcmLength;
    entity->duration = entity->decoder.outputSampleRate > 0
        ? static_cast<double>(pcmLength) /
              static_cast<double>(entity->decoder.outputSampleRate)
        : 0.0;

    Log::i("audio", "frames length: %llu", pcmLength);
    Log::i("audio", "durationTime: %f", entity->duration);


    result = ma_sound_init_from_data_source(
        &gAudioEngine,
        &entity->decoder,
        0,
        nullptr,
        &entity->sound
    );

    if (result != MA_SUCCESS) {
        Log::e("audio", "ma_sound_init_from_data_source failed: %d", result);
        ma_decoder_uninit(&entity->decoder);
        entity->pData.reset();
        return nullptr;
    }

    // 设置是否循环播放
    ma_sound_set_looping(
        &entity->sound,
        playLoop ? MA_TRUE : MA_FALSE
    );

    ma_sound_set_end_callback(&entity->sound, onSoundEnd, entity.get());

    entity->isPlay = false;
    entity->isReleased = false;

    return entity;
}

void AudioManager::playAudioEntity(std::shared_ptr<AudioEntity> entity) {
    if (entity == nullptr || entity->isReleased) {
        return;
    }

    if (!ensureAudioEngine()) {
        return;
    }

    ma_result result = ma_sound_start(&entity->sound);
    if (result != MA_SUCCESS) {
        Log::e("audio", "ma_sound_start failed: %d", result);
        return;
    }

    entity->isPlay = true;
}

void AudioManager::stopAudioEntity(std::shared_ptr<AudioEntity> entity) {
    if (entity == nullptr || entity->isReleased) {
        return;
    }

    ma_result result = ma_sound_stop(&entity->sound);
    if (result != MA_SUCCESS) {
        Log::e("audio", "ma_sound_stop failed: %d", result);
    }

    entity->isPlay = false;
}

bool AudioManager::isPlayAudioEntity(std::shared_ptr<AudioEntity> entity) {
    if (entity == nullptr || entity->isReleased) {
        return false;
    }

    return ma_sound_is_playing(&entity->sound) == MA_TRUE;
}

void AudioManager::restartAudioEntity(std::shared_ptr<AudioEntity> entity) {
    if (entity == nullptr || entity->isReleased) {
        return;
    }

    ma_result result = ma_sound_seek_to_pcm_frame(&entity->sound, 0);
    if (result != MA_SUCCESS) {
        Log::e("audio", "ma_sound_seek_to_pcm_frame failed: %d", result);
        return;
    }

    playAudioEntity(entity);
}

void AudioManager::setAudioPlayProgressCallback(
    std::shared_ptr<AudioEntity> entity,
    ProgressUpdateCallbackFn callback
) {
    if (entity == nullptr || entity->isReleased) {
        return;
    }

    entity->setPlayProgressUpdateCallback(callback);
}

void AudioManager::setAudioPlayEndCallback(
    std::shared_ptr<AudioEntity> entity,
    PlayEndCallbackFn callback
) {
    if (entity == nullptr || entity->isReleased) {
        return;
    }

    entity->setPlayEndCallback(callback);
}

AudioInfo AudioManager::getAudioEntityInfo(std::shared_ptr<AudioEntity> entity) {
    AudioInfo retInfo{};

    if (entity == nullptr || entity->isReleased) {
        return retInfo;
    }

    ma_uint64 cursor = 0;
    if (ma_sound_get_cursor_in_pcm_frames(&entity->sound, &cursor) == MA_SUCCESS) {
        entity->readedFrame = cursor;
    }

    retInfo.duration = entity->duration;
    retInfo.isLoop = entity->isLoop;
    retInfo.isPlay = ma_sound_is_playing(&entity->sound) == MA_TRUE;
    retInfo.pcmFrameLength = entity->pcmFrameLength;
    retInfo.readedFrame = entity->readedFrame;
    retInfo.name = entity->name;

    return retInfo;
}

void AudioManager::releaseAudioEntity(std::shared_ptr<AudioEntity> entity) {
    if (entity == nullptr || entity->isReleased) {
        return;
    }

    Log::i("audio", "audio %s release", entity->name.c_str());

    entity->isPlay = false;

    // 先释放 sound，确保它不再引用 decoder；
    // 再释放 decoder 和其依赖的原始内存。
    ma_sound_stop(&entity->sound);
    ma_sound_uninit(&entity->sound);
    ma_decoder_uninit(&entity->decoder);

    entity->pData.reset();
    entity->isReleased = true;
}

std::shared_ptr<AudioEntity> AudioManager::loadAudio(
    std::string path,
    std::string key,
    bool playloop,
    bool asset_path
) {
    auto entity = loadAudioEntity(path, playloop, asset_path);
    if (entity == nullptr) {
        return nullptr;
    }

    auto it = audioMap_.find(key);
    if (it != audioMap_.end()) {
        releaseAudioEntity(it->second);
        it->second = entity;
    } else {
        audioMap_.emplace(key, entity);
    }

    return entity;
}

void AudioManager::playAudio(std::string key) {
    auto it = audioMap_.find(key);
    if (it != audioMap_.end()) {
        playAudioEntity(it->second);
    }
}

void AudioManager::stopAudio(std::string key) {
    auto it = audioMap_.find(key);
    if (it != audioMap_.end()) {
        stopAudioEntity(it->second);
    }
}

void AudioManager::restartAudio(std::string key) {
    auto it = audioMap_.find(key);
    if (it != audioMap_.end()) {
        restartAudioEntity(it->second);
    }
}

void AudioManager::releaseAudio(std::string key) {
    auto it = audioMap_.find(key);
    if (it == audioMap_.end()) {
        return;
    }

    releaseAudioEntity(it->second);
    audioMap_.erase(it);
}

void AudioManager::init() {
    Log::i("audio", "audio manager init");

    if (!ensureAudioEngine()) {
        Log::e("audio", "audio manager init failed");
    }
}

void AudioManager::dispose() {
    Log::i("audio", "audio manager dispose");

    for (auto& pair : audioMap_) {
        if (pair.second != nullptr) {
            releaseAudioEntity(pair.second);
        }
    }

    audioMap_.clear();

    if (gAudioEngineInitialized) {
        ma_engine_uninit(&gAudioEngine);
        gAudioEngineInitialized = false;
    }
}

} // namespace zidian
