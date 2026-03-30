#include <jni.h>
#include <errno.h>
#include <GLES2/gl2.h>
#include <android/log.h>
#include <android_native_app_glue.h>

// Log helper agar kita bisa pantau lewat adb logcat di Termux
#define LOG_TAG "ZAMERA"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// --- JEMBATAN KE RUST ---
extern float get_rust_rotation(float t);
extern float get_rust_scale(float t);
// ------------------------

struct engine {
    struct android_app* app;
    float state_tick;
};

static void engine_draw_frame(struct engine* engine) {
    if (engine->app->window == NULL) return;

    // Ambil data dari "Otak" Rust
    float rotation = get_rust_rotation(engine->state_tick);
    float scale = get_rust_scale(engine->state_tick);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f); // Warna navy gelap
    glClear(GL_COLOR_BUFFER_BIT);

    // Render sangat sederhana (GLES 2.0 style)
    // Catatan: Karena ini demo, kita pakai state_tick untuk rotasi
    // Di aplikasi nyata, kita akan pakai Shader (GLSL)
    
    engine->state_tick += 0.01f;
    eglSwapBuffers(engine->app->display, engine->app->surface);
}

void android_main(struct android_app* state) {
    struct engine engine = {0};
    state->userData = &engine;
    engine.app = state;
    
    LOGI("Zamera Hybrid Started!");

    while (1) {
        int ident, events;
        struct android_poll_source* source;
        while ((ident=ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) source->process(state, source);
            if (state->destroyRequested != 0) return;
        }
        engine_draw_frame(&engine);
    }
}
