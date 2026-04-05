#include <jni.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>
#include <android/log.h>
#include <stdio.h>

extern float rust_mining_next_val();
extern float get_hashrate();
extern int get_nonce();

struct engine {
    struct android_app* app;
    EGLDisplay display; EGLSurface surface;
    int width, height;
    int frame_count;
};

// Fungsi sakti untuk memunculkan Toast dari Native C
void show_toast(struct android_app* app, const char* message) {
    JNIEnv* env;
    (*app->activity->vm)->AttachCurrentThread(app->activity->vm, &env, NULL);

    jclass toast_class = (*env)->FindClass(env, "android/widget/Toast");
    jmethodID make_text = (*env)->GetStaticMethodID(env, toast_class, "makeText", 
        "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");
    
    jstring jmsg = (*env)->NewStringUTF(env, message);
    jobject toast_obj = (*env)->CallStaticObjectMethod(env, toast_class, make_text, 
        app->activity->clazz, jmsg, 0); // 0 = Toast.LENGTH_SHORT

    jmethodID show = (*env)->GetMethodID(env, toast_class, "show", "()V");
    (*env)->CallVoidMethod(env, toast_obj, show);

    (*app->activity->vm)->DetachCurrentThread(app->activity->vm);
}

static void draw_frame(struct engine* eng) {
    if (eng->display == EGL_NO_DISPLAY) return;

    float val = rust_mining_next_val();
    eng->frame_count++;

    // Munculkan Hashrate di layar setiap 120 frame (sekitar 2 detik)
    if (eng->frame_count % 120 == 0) {
        char msg[64];
        sprintf(msg, "Zamera Miner: %.2f MH/s", get_hashrate());
        show_toast(eng->app, msg);
        __android_log_print(ANDROID_LOG_INFO, "MINER", "%s", msg);
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Visual Matrix Rain tetap jalan
    glEnable(GL_SCISSOR_TEST);
    int num_cols = 30;
    for(int i=0; i<num_cols; i++) {
        int x = i * (eng->width/num_cols);
        int y = (int)(eng->height * val + (i*10)) % eng->height;
        glScissor(x, y, (eng->width/num_cols)-2, 40);
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
    while (1) {
        int id, ev; struct android_poll_source* src;
        while ((id = ALooper_pollOnce(0, NULL, &ev, (void**)&src)) >= 0) {
            if (src != NULL) src->process(state, src);
            if (state->destroyRequested != 0) return;
        }
        if (eng.display != EGL_NO_DISPLAY) draw_frame(&eng);
    }
}
