#include <jni.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>

// Jembatan ke Rust
extern float get_rust_status();
extern float get_rust_color_r(float t);
extern float get_rust_color_g(float t);

struct engine {
    struct android_app* app;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    float tick;
};

static int init_display(struct engine* eng) {
    EGLDisplay disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(disp, 0, 0);
    const EGLint attr[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_NONE };
    EGLConfig cfg; EGLint n;
    eglChooseConfig(disp, attr, &cfg, 1, &n);
    EGLSurface surf = eglCreateWindowSurface(disp, cfg, eng->app->window, NULL);
    EGLContext ctx = eglCreateContext(disp, cfg, NULL, (EGLint[]){EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE});
    eglMakeCurrent(disp, surf, surf, ctx);
    eng->display = disp; eng->surface = surf; eng->context = ctx;
    return 0;
}

static void draw_frame(struct engine* eng) {
    if (eng->display == NULL) return;
    
    // Ambil warna dinamis dari Otak Rust
    float r = get_rust_color_r(eng->tick);
    float g = get_rust_color_g(eng->tick);
    
    glClearColor(r, g, 0.5f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(eng->display, eng->surface);
    eng->tick += 0.02f;
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* eng = (struct engine*)app->userData;
    if (cmd == APP_CMD_INIT_WINDOW) {
        init_display(eng);
    } else if (cmd == APP_CMD_TERM_WINDOW) {
        eng->display = NULL; // Stop drawing
    }
}

void android_main(struct android_app* state) {
    struct engine eng = {0};
    state->userData = &eng;
    state->onAppCmd = handle_cmd;
    eng.app = state;

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
