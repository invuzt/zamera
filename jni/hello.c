#include <jni.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#include <time.h>

extern void update_game();
extern void set_dir(int d);
extern int get_snake_x(int i), get_snake_y(int i), get_snake_len(), get_food_x(), get_food_y();

struct engine {
    struct android_app* app;
    EGLDisplay display; EGLSurface surface; EGLContext context;
    int width, height;
    long last_tick;
};

static void draw_frame(struct engine* eng) {
    if (eng->display == EGL_NO_DISPLAY) return;

    // Game Speed: 150ms
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long current_ms = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    if (current_ms - eng->last_tick > 150) {
        update_game();
        eng->last_tick = current_ms;
    }

    glClearColor(0.0f, 0.05f, 0.0f, 1.0f); // Hijau Tua Gelap
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);

    int grid = eng->width / 20;

    // 1. GAMBAR ULAR & MAKANAN
    for (int i = 0; i < get_snake_len(); i++) {
        glScissor(get_snake_x(i)*grid, get_snake_y(i)*grid + 300, grid-2, grid-2);
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    glScissor(get_food_x()*grid, get_food_y()*grid + 300, grid-2, grid-2);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 2. GAMBAR MENU NAVIGASI (DI BAWAH)
    int btn_w = eng->width / 4;
    int btn_h = 200;
    
    // Tombol Kiri (Kuning)
    glScissor(0, 50, btn_w-5, btn_h);
    glClearColor(0.8f, 0.8f, 0.0f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);
    // Tombol Atas (Biru)
    glScissor(btn_w, 50, btn_w-5, btn_h);
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);
    // Tombol Bawah (Putih)
    glScissor(btn_w*2, 50, btn_w-5, btn_h);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);
    // Tombol Kanan (Kuning)
    glScissor(btn_w*3, 50, btn_w-5, btn_h);
    glClearColor(0.8f, 0.8f, 0.0f, 1.0f); glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_SCISSOR_TEST);
    eglSwapBuffers(eng->display, eng->surface);
}

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        struct engine* eng = (struct engine*)app->userData;
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        
        // Deteksi klik di area tombol bawah (Y > height - 300)
        if (y > eng->height - 300) {
            if (x < eng->width/4) set_dir(1);           // Kiri
            else if (x < (eng->width/4)*2) set_dir(2);  // Atas
            else if (x < (eng->width/4)*3) set_dir(3);  // Bawah
            else set_dir(0);                            // Kanan
        }
        return 1;
    }
    return 0;
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
    state->onInputEvent = handle_input;
    while (1) {
        int id, ev; struct android_poll_source* src;
        while ((id = ALooper_pollOnce(0, NULL, &ev, (void**)&src)) >= 0) {
            if (src != NULL) src->process(state, src);
            if (state->destroyRequested != 0) return;
        }
        if (eng.display != EGL_NO_DISPLAY) draw_frame(&eng);
    }
}
