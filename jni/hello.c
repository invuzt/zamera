#include <jni.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#include <stdlib.h>
#include <math.h>

extern float rust_mining_next_val();
extern int get_nonce();

struct engine {
    struct android_app* app;
    EGLDisplay display; EGLSurface surface; EGLContext context;
    int width, height;
};

static void draw_frame(struct engine* eng) {
    if (eng->display == EGL_NO_DISPLAY) return;

    // Ambil "Hash Power" dari Rust
    float val = rust_mining_next_val();
    int nonce = get_nonce();

    // Background Hitam khas Terminal
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Aktifkan Scissor Test untuk menggambar "Bar Data" Mining
    glEnable(GL_SCISSOR_TEST);
    
    // Kita gambar 20 baris data simulasi berdasarkan Hash
    for(int i = 0; i < 20; i++) {
        float speed = (float)(i + 1) * val;
        int x_pos = (nonce + (i * 50)) % eng->width;
        int y_pos = (i * (eng->height / 20));
        
        // Warna Hijau Matrix yang bervariasi sesuai Hash
        glScissor(x_pos, y_pos, 100 * val, 20); 
        glClearColor(0.0f, val, 0.0f, 1.0f);
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
        
        // Ambil ukuran layar untuk kalkulasi posisi visual
        eng->width = ANativeWindow_getWidth(app->window);
        eng->height = ANativeWindow_getHeight(app->window);
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
        if (eng.display != EGL_NO_DISPLAY) draw_frame(&eng);
    }
}
