#include <jni.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>

extern float get_rust_status();

struct engine {
    struct android_app* app;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
};

static int engine_init_display(struct engine* engine) {
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(display, 0, 0);
    const EGLint attribs[] = { EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, EGL_BLUE_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_RED_SIZE, 8, EGL_NONE };
    EGLConfig config; EGLint numConfigs;
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    EGLSurface surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    EGLContext context = eglCreateContext(display, config, NULL, (EGLint[]){EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE});
    eglMakeCurrent(display, surface, surface, context);
    engine->display = display; engine->surface = surface; engine->context = context;
    return 0;
}

static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) return;
    
    // TANYA RUST
    if (get_rust_status() > 0.5f) {
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f); // HIJAU MANTAP
    } else {
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f); // MERAH GAGAL
    }
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(engine->display, engine->surface);
}

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    if (cmd == APP_CMD_INIT_WINDOW) {
        engine_init_display(engine);
        engine_draw_frame(engine);
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
        while ((ident=ALooper_pollOnce(0, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) source->process(state, source);
            if (state->destroyRequested != 0) return;
        }
        if (engine.display != NULL) engine_draw_frame(&engine);
    }
}
