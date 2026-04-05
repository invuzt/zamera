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
    EGLDisplay display; EGLSurface surface;
    int width, height;
    long last_tick;
};

static void draw_frame(struct engine* eng) {
    if (eng->display == EGL_NO_DISPLAY) return;

    // Game Tick (Jalan setiap 150ms agar tidak terlalu cepat)
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long current_ms = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    
    if (current_ms - eng->last_tick > 150) {
        update_game();
        eng->last_tick = current_ms;
    }

    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);

    int cell = eng->width / 22; // Ukuran kotak ular

    // Gambar Makanan (Merah)
    glScissor(get_food_x() * cell, get_food_y() * cell, cell-2, cell-2);
    glClearColor(1.0f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Gambar Ular (Hijau)
    for (int i = 0; i < get_snake_len(); i++) {
        glScissor(get_snake_x(i) * cell, get_snake_y(i) * cell, cell-2, cell-2);
        glClearColor(0.0f, 1.0f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glDisable(GL_SCISSOR_TEST);
    eglSwapBuffers(eng->display, eng->surface);
}

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        float x = AMotionEvent_getX(event, 0);
        float y = AMotionEvent_getY(event, 0);
        struct engine* eng = (struct engine*)app->userData;
        
        if (x < eng->width/3) set_dir(1);
        else if (x > (eng->width/3)*2) set_dir(0);
        else if (y < eng->height/2) set_dir(2);
        else set_dir(3);
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
        EGLContext ctx = eglCreateContext(eng->display, cfg, NULL, (EGLint[]){EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE});
        eglMakeCurrent(eng->display, eng->surface, eng->surface, ctx);
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
