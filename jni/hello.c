#include <jni.h>
#include <errno.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOG_TAG "ZAMERA"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// --- JEMBATAN KE RUST ---
extern float get_rust_rotation(float t);
extern float get_rust_scale(float t);
// ------------------------

struct engine {
    struct android_app* app;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    float state_tick;
};

static void engine_draw_frame(struct engine* engine) {
    if (engine->display == EGL_NO_DISPLAY) return;

    // Ambil data dari Rust
    float rotation = get_rust_rotation(engine->state_tick);
    
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    engine->state_tick += 0.01f;
    eglSwapBuffers(engine->display, engine->surface);
}

// Handler untuk inisialisasi window (Penting untuk NDK baru)
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (app->window != NULL) {
                // Inisialisasi EGL sederhana di sini biasanya, 
                // tapi kita biarkan kosong dulu agar tidak error linking
                engine_draw_frame(engine);
            }
            break;
    }
}

void android_main(struct android_app* state) {
    struct engine engine = {0};
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    engine.app = state;
    
    while (1) {
        int ident, events;
        struct android_poll_source* source;
        // Gunakan ALooper_pollOnce sesuai saran error tadi
        while ((ident=ALooper_pollOnce(0, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) source->process(state, source);
            if (state->destroyRequested != 0) return;
        }
        engine_draw_frame(&engine);
    }
}
