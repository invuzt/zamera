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

// --- FUNGSI BARU UNTUK VISUALISASI MATRIX RAIN ---
static void draw_frame(struct engine* eng) {
    if (eng->display == EGL_NO_DISPLAY) return;

    // Ambil data 'Hash Power' dari Rust
    float val = rust_mining_next_val();
    int nonce = get_nonce();

    // Background Hitam pekat agar hijau Matrix terlihat tajam
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_SCISSOR_TEST);
    
    // Jumlah 'kolom data' yang mengalir ke bawah (Matrix columns)
    int num_columns = 40; 
    int column_width = eng->width / num_columns;

    for(int i = 0; i < num_columns; i++) {
        // Tentukan kecepatan jatuh kolom data berdasarkan Hash dari Rust
        float speed = ((float)(i + 1) * val) * 0.5f; 
        
        // Posisi vertikal jatuh (Y_pos), dikendalikan oleh nonce dan val Rust
        int y_pos = eng->height - ((int)((nonce + (i * 100)) * speed) % eng->height);
        
        // Efek Visual: Kita gambar 'jejak data' hijau Matrix.
        // Bagian atas jejak lebih terang, bagian bawah lebih gelap.
        for(int j = 0; j < 5; j++) {
            int segment_y = y_pos + (j * 20);
            if (segment_y > eng->height) segment_y %= eng->height;
            
            // Warna Hijau Matrix, memudar ke bawah (Gradiasi Matrix)
            float brightness = 1.0f - ((float)j * 0.2f);
            
            // Gambar kotak data digital di kolom i
            glScissor(i * column_width, segment_y, column_width - 2, 20); 
            glClearColor(0.0f, brightness * val, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }
    }
    
    glDisable(GL_SCISSOR_TEST);
    eglSwapBuffers(eng->display, eng->surface);
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
        
        // Ambil ukuran layar saat jendela dibuat (Sekarang akan Portrait)
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
