#include <jni.h>
#include <GLES2/gl2.h>
#include <android_native_app_glue.h>
#include <android/log.h>
#include <stdlib.h>

extern char* rust_mining_tick();
extern int get_nonce();

struct engine {
    struct android_app* app;
    EGLDisplay display; EGLSurface surface;
};

static void draw_frame(struct engine* eng) {
    if (eng->display == NULL) return;

    // Ambil data mining dari Rust
    char* current_hash = rust_mining_tick();
    int nonce = get_nonce();

    // Log ke Logcat agar terlihat seperti proses mining di terminal
    __android_log_print(ANDROID_LOG_INFO, "BITCOIN_SIM", "Block #%d | Hash: %s", nonce, current_hash);

    // Efek Visual: Warna latar berubah sedikit setiap kali hash ditemukan
    // (Simulasi visual sederhana karena kita belum pasang font renderer)
    float r = (float)(nonce % 100) / 100.0f;
    glClearColor(0.0f, r * 0.2f, 0.0f, 1.0f); // Hijau Matrix
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(eng->display, eng->surface);
    
    // Bebaskan memori string dari Rust
    free(current_hash);
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* eng = (struct engine*)app->userData;
    if (cmd == APP_CMD_INIT_WINDOW) {
        EGLDisplay disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(disp, 0, 0);
        const EGLint attr[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_BLUE_SIZE, 8, EGL_NONE };
        EGLConfig cfg; EGLint n;
        eglChooseConfig(disp, attr, &cfg, 1, &n);
        eng->surface = eglCreateWindowSurface(disp, cfg, app->window, NULL);
        eng->display = disp;
        EGLContext ctx = eglCreateContext(disp, cfg, NULL, (EGLint[]){EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE});
        eglMakeCurrent(disp, eng->surface, eng->surface, ctx);
    }
}

void android_main(struct android_app* state) {
    struct engine eng = {0};
    state->userData = &eng;
    state->onAppCmd = handle_cmd;
    
    while (1) {
        int id, ev;
        struct android_poll_source* src;
        while ((id = ALooper_pollOnce(0, NULL, &ev, (void**)&src)) >= 0) {
            if (src != NULL) src->process(state, src);
            if (state->destroyRequested != 0) return;
        }
        if (eng.display != NULL) draw_frame(&eng);
    }
}
