#include <jni.h>
#include <errno.h>
#include <gl2.h>
#include <android_native_app_glue.h>

// --- JEMBATAN KE RUST ---
extern float get_rust_rotation(float t);
extern float get_rust_scale(float t);
extern const char* get_rust_message();
// ------------------------

struct engine {
    struct android_app* app;
    int animating;
    float state_tick;
};

static void engine_draw_frame(struct engine* engine) {
    if (engine->app->window == NULL) return;

    // Ambil data dari "Otak" Rust
    float rotation = get_rust_rotation(engine->state_tick);
    float scale = get_rust_scale(engine->state_tick);
    const char* msg = get_rust_message();

    glClearColor(0.2f, 0.2f, 0.2f, 1); // Background abu-abu gelap
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();
    glScalef(scale, scale, 1.0f);    // Pakai Skala dari Rust
    glRotatef(rotation, 0, 0, 1);    // Pakai Rotasi dari Rust

    // Gambar kotak sederhana
    glBegin(GL_TRIANGLE_STRIP);
        glColor4f(0, 1, 0, 1); glVertex2f(-0.5f, -0.5f);
        glColor4f(1, 0, 0, 1); glVertex2f(0.5f, -0.5f);
        glColor4f(0, 0, 1, 1); glVertex2f(-0.5f, 0.5f);
        glColor4f(1, 1, 1, 1); glVertex2f(0.5f, 0.5f);
    glEnd();

    engine->state_tick += 0.01f;
    eglSwapBuffers(engine->app->display, engine->app->surface);
}

// Logika handle input dan lifecycle tetap di sini agar stabil
void android_main(struct android_app* state) {
    struct engine engine = {0};
    state->userData = &engine;
    engine.app = state;
    
    while (1) {
        int ident, events;
        struct android_poll_source* source;
        while ((ident=ALooper_pollAll(0, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) source->process(state, source);
            if (state->destroyRequested != 0) return;
        }
        engine_draw_frame(&engine);
    }
}
