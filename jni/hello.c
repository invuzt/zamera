#include <jni.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#include <android/log.h>

extern float rust_mining_next_val();
extern int get_nonce_last_digit();

struct engine {
    struct android_app* app;
    EGLDisplay display; 
    EGLSurface surface; 
    EGLContext context;
    int width, height;
};

static void draw_frame(struct engine* eng) {
    if (eng->display == EGL_NO_DISPLAY) return;

    // Ambil data dari Rust (Sudah diperlambat di sisi Rust)
    float val = rust_mining_next_val();
    int digit = get_nonce_last_digit();

    // 1. Clear Screen (Hitam)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);

    // 2. GAMBAR BAR INDIKATOR (TENGAH LAYAR)
    // Tinggi 100px, lebar sesuai progress hash
    int bar_h = 100;
    int bar_y = eng->height / 2;
    int current_bar_w = (int)(eng->width * val);

    // Background Bar (Abu-abu)
    glScissor(0, bar_y, eng->width, bar_h);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Isi Bar (Hijau Terang)
    if (current_bar_w > 0) {
        glScissor(0, bar_y, current_bar_w, bar_h);
        glClearColor(0.0f, 1.0f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // 3. KOTAK STATUS (POJOK KIRI ATAS)
    // Ukuran 150x150, warna berubah tiap ganti nonce
    int box_s = 150;
    glScissor(50, eng->height - 250, box_s, box_s);
    float hue = (float)digit / 10.0f;
    glClearColor(hue, 1.0f - hue, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_SCISSOR_TEST);
    eglSwapBuffers(eng->display, eng->surface);
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* eng = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (app->window != NULL) {
                eng->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
                eglInitialize(eng->display, NULL, NULL);
                
                const EGLint attr[] = { 
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, 
                    EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, 
                    EGL_NONE 
                };
                EGLConfig cfg; 
                EGLint n;
                eglChooseConfig(eng->display, attr, &cfg, 1, &n);
                
                eng->surface = eglCreateWindowSurface(eng->display, cfg, app->window, NULL);
                eng->context = eglCreateContext(eng->display, cfg, NULL, (EGLint[]){EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE});
                
                eglMakeCurrent(eng->display, eng->surface, eng->surface, eng->context);
                
                eng->width = ANativeWindow_getWidth(app->window);
                eng->height = ANativeWindow_getHeight(app->window);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            eglMakeCurrent(eng->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
            eglDestroyContext(eng->display, eng->context);
            eglDestroySurface(eng->display, eng->surface);
            eglTerminate(eng->display);
            eng->display = EGL_NO_DISPLAY;
            break;
    }
}

void android_main(struct android_app* state) {
    struct engine eng = {0};
    eng.app = state;
    state->userData = &eng;
    state->onAppCmd = handle_cmd;

    while (1) {
        int id, ev;
        struct android_poll_source* src;
        while ((id = ALooper_pollOnce(0, NULL, &ev, (void**)&src)) >= 0) {
            if (src != NULL) src->process(state, src);
            if (state->destroyRequested != 0) return;
        }
        if (eng.display != EGL_NO_DISPLAY) {
            draw_frame(&eng);
        }
    }
}
