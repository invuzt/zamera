#include <jni.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>

extern float rust_mining_next_val();
extern int get_nonce_last_digit();

struct engine {
    struct android_app* app;
    EGLDisplay display; EGLSurface surface; EGLContext context;
    int width, height;
};

static void draw_frame(struct engine* eng) {
    if (eng->display == EGL_NO_DISPLAY) return;

    float val = rust_mining_next_val();
    int digit = get_nonce_last_digit();

    // Background Hitam
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);

    // 1. GAMBAR BAR TENGAH (Indikator Power)
    // Kita buat di tengah layar agar pasti kelihatan (Sistem koordinat Y dari bawah)
    int bar_w = (int)(eng->width * val);
    int bar_h = 100; // Buat tebal 100 pixel
    int bar_y = eng->height / 2; 

    glScissor(0, bar_y, eng->width, bar_h);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Abu-abu (BG Bar)
    glClear(GL_COLOR_BUFFER_BIT);

    glScissor(0, bar_y, bar_w, bar_h);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // Hijau (Isi Bar)
    glClear(GL_COLOR_BUFFER_BIT);

    // 2. GAMBAR "PIXEL DIGIT" (Simulasi Teks Angka Terakhir Nonce)
    // Kita gambar kotak besar di pojok kiri atas sebagai ganti teks
    int square_size = 150;
    glScissor(50, eng->height - 200, square_size, square_size);
    
    // Warna kotak berubah sesuai angka terakhir Nonce (0-9)
    float color_shift = (float)digit / 10.0f;
    glClearColor(color_shift, 1.0f - color_shift, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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
        eng->surface = eglCreateWindowSurface(disp, cfg, app->window, NULL);
        eng->context = eglCreateContext(disp, cfg, NULL, (EGLint[]){EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE});
        eglMakeCurrent(disp, eng->surface, eng->surface, eng->context);
        
        eng->width = ANativeWindow_getWidth(app->window);
        eng->height = ANativeWindow_getHeight(app->window);
    }
}
// ... (android_main tetap sama seperti sebelumnya) ...
