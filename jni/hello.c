#include <jni.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>

extern float rust_mining_next_val();
extern float get_hashrate_pct();

struct engine {
    struct android_app* app;
    EGLDisplay display; EGLSurface surface; EGLContext context;
    int width, height;
};

static void draw_frame(struct engine* eng) {
    if (eng->display == EGL_NO_DISPLAY) return;

    float hash_val = rust_mining_next_val();
    float rate_pct = get_hashrate_pct();

    // 1. Bersihkan layar (Background Hitam)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);

    // 2. GAMBAR HASHRATE BAR (DI ATAS)
    // Tinggi bar 30 pixel, panjang sesuai rate_pct
    int bar_height = 40;
    int bar_width = (int)(eng->width * rate_pct);
    
    // Background bar (Abu-abu gelap)
    glScissor(0, eng->height - bar_height, eng->width, bar_height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Isi bar (Hijau Neon / Glow)
    if (bar_width > 0) {
        glScissor(0, eng->height - bar_height, bar_width, bar_height);
        glClearColor(0.0f, 1.0f, 0.5f, 1.0f); // Hijau Cyan
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // 3. VISUAL MATRIX RAIN (DI BAWAH BAR)
    int num_cols = 25;
    int col_w = eng->width / num_cols;
    for(int i=0; i<num_cols; i++) {
        int y = (int)((eng->height - bar_height) * hash_val + (i * 20)) % (eng->height - bar_height);
        glScissor(i * col_w, y, col_w - 2, 30);
        glClearColor(0.0f, hash_val * 0.7f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glDisable(GL_SCISSOR_TEST);
    eglSwapBuffers(eng->display, eng->surface);
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* eng = (struct engine*)app->userData;
    if (cmd == APP_CMD_INIT_WINDOW) {
        eng->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(eng->display, NULL, NULL);
        const EGLint attr[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_BLUE_SIZE, 8, EGL_NONE };
        EGLConfig cfg; EGLint n;
        eglChooseConfig(eng->display, attr, &cfg, 1, &n);
        eng->surface = eglCreateWindowSurface(eng->display, cfg, app->window, NULL);
        eng->context = eglCreateContext(eng->display, cfg, NULL, (EGLint[]){EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE});
        eglMakeCurrent(eng->display, eng->surface, eng->surface, eng->context);
        eng->width = ANativeWindow_getWidth(app->window);
        eng->height = ANativeWindow_getHeight(app->window);
    }
}

void android_main(struct android_app* state) {
    struct engine eng = {0};
    state->userData = &eng;
    state->onAppCmd = handle_cmd;
    while (1) {
        int id, ev; struct android_poll_source* src;
        while ((id = ALooper_pollOnce(0, NULL, &ev, (void**)&src)) >= 0) {
            if (src != NULL) src->process(state, src);
            if (state->destroyRequested != 0) return;
        }
        if (eng.display != EGL_NO_DISPLAY) draw_frame(&eng);
    }
}
