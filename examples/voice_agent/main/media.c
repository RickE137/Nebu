#include "esp_check.h"
#include "esp_log.h"
#include "codec_init.h"
#include "esp_capture_path_simple.h"
#include "esp_capture_audio_enc.h"
#include "av_render_default.h"
#include "esp_audio_dec_default.h"
#include "esp_audio_enc_default.h"
#include "esp_capture_defaults.h"

#include "media.h"

static const char *TAG = "media";

#define NULL_CHECK(pointer, message) \
    ESP_RETURN_ON_FALSE(pointer != NULL, -1, TAG, message)

typedef struct {
    esp_capture_aenc_if_t      *audio_encoder;
    esp_capture_audio_src_if_t *audio_source;
    esp_capture_path_if_t      *capture_path;
    esp_capture_path_handle_t   capturer_handle;
} capture_system_t;

typedef struct {
    audio_render_handle_t audio_renderer;
    av_render_handle_t    av_renderer_handle;
} renderer_system_t;

static capture_system_t  capturer_system;
static renderer_system_t renderer_system;

static int build_capturer_system(void)
{
    // 1. Create audio encoder
    capturer_system.audio_encoder = esp_capture_new_audio_encoder();
    NULL_CHECK(capturer_system.audio_encoder, "Failed to create audio encoder");

    // 2. Create audio source - CONFIGURACIÓN ORIGINAL FUNCIONAL
    esp_codec_dev_handle_t record_handle = get_record_handle();
    NULL_CHECK(record_handle, "Failed to get record handle");

    // Configuración AEC (funcional) pero con AEC desactivado
    esp_capture_audio_aec_src_cfg_t codec_cfg = {
        .record_handle = record_handle,
        .channel = 1,        // INMP441 es mono (1 canal)
        .channel_mask = 1     // Solo canal izquierdo
    };
    capturer_system.audio_source = esp_capture_new_audio_aec_src(&codec_cfg);
    NULL_CHECK(capturer_system.audio_source, "Failed to create audio source");
    
    // CAMBIO MÍNIMO: Solo aumentar ganancia (funcional)
    esp_codec_dev_set_in_gain(record_handle, 20);  // 20dB gain (conservador)
    
    ESP_LOGI(TAG, "✅ Audio source creado con AEC (funcional)");
    ESP_LOGI(TAG, "🎤 Ganancia configurada: 20dB para INMP441");
    ESP_LOGI(TAG, "📊 Configuración: 48kHz, mono, 16-bit");
    ESP_LOGI(TAG, "🎯 Frame size esperado: 960 samples (20ms @ 48kHz)");

    // 3. Create capture path
    esp_capture_simple_path_cfg_t path_cfg = {
        .aenc = capturer_system.audio_encoder,
    };
    capturer_system.capture_path = esp_capture_build_simple_path(&path_cfg);
    NULL_CHECK(capturer_system.capture_path, "Failed to create capture path");

    // 4. Create capture system
    esp_capture_cfg_t cfg = {
        .sync_mode = ESP_CAPTURE_SYNC_MODE_AUDIO,
        .audio_src = capturer_system.audio_source,
        .capture_path = capturer_system.capture_path,
    };
    esp_capture_open(&cfg, &capturer_system.capturer_handle);
    NULL_CHECK(capturer_system.capturer_handle, "Failed to open capture system");
    return 0;
}

static int build_renderer_system(void)
{
    // 1. Create audio renderer
    i2s_render_cfg_t i2s_cfg = {
        .play_handle = get_playback_handle()
    };
    renderer_system.audio_renderer = av_render_alloc_i2s_render(&i2s_cfg);
    NULL_CHECK(renderer_system.audio_renderer, "Failed to create I2S renderer");

    // Set initial speaker volume
    esp_codec_dev_set_out_vol(i2s_cfg.play_handle, CONFIG_DEFAULT_PLAYBACK_VOL);
    
    // CRÍTICO: Mantener ganancia alta como Wheatley (NO sobrescribir)
    // esp_codec_dev_set_in_gain(get_record_handle(), 30);  // Ya configurado arriba

    // 2. Create AV renderer
    // For this example, this only includes an audio renderer.
    av_render_cfg_t render_cfg = {
        .audio_render = renderer_system.audio_renderer,
        .audio_raw_fifo_size = 8 * 4096,
        .audio_render_fifo_size = 100 * 1024,
        .allow_drop_data = false,
    };
    renderer_system.av_renderer_handle = av_render_open(&render_cfg);
    NULL_CHECK(renderer_system.av_renderer_handle, "Failed to create AV renderer");

    // 3. Set frame info - ESTÁNDAR OPUS 48kHz
    av_render_audio_frame_info_t frame_info = {
        .sample_rate = 48000,    // ESTÁNDAR OPUS (recomendado por IA)
        .channel = 1,            // MAX98357 es mono (1 canal)
        .bits_per_sample = 16,
    };
    av_render_set_fixed_frame_info(renderer_system.av_renderer_handle, &frame_info);

    return 0;
}

int media_init(void)
{
    // Register default audio encoder and decoder
    esp_audio_enc_register_default();
    esp_audio_dec_register_default();

    // Build capturer and renderer systems
    build_capturer_system();
    build_renderer_system();
    return 0;
}

esp_capture_handle_t media_get_capturer(void)
{
    return capturer_system.capturer_handle;
}

av_render_handle_t media_get_renderer(void)
{
    return renderer_system.av_renderer_handle;
}