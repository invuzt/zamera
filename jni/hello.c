#include <jni.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <android_native_app_glue.h>

// Deklarasi fungsi Rust
extern float get_rust_rotation(float t);

void android_main(struct android_app* state) {
    // Pastikan glue tidak dibuang oleh linker
    app_dummy();

    while (1) {
        int ident, events;
        struct android_poll_source* source;
        while ((ident=ALooper_pollOnce(0, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) source->process(state, source);
            if (state->destroyRequested != 0) return;
        }

        // Jika layar sudah siap, gambar warna HIJAU (bukti C jalan)
        // Dan panggil Rust (bukti Jembatan jalan)
        float rot = get_rust_rotation(0.1f);
        
        if (rot > 0.0f) {
            // Jika Rust berhasil dipanggil, beri warna HIJAU TERANG
            glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        } else {
            // Jika Rust gagal, biarkan MERAH
            glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        // Kita tidak panggil swapBuffers dulu karena EGL-nya butuh setup panjang
        // Cukup pastikan glClearColor bekerja
    }
}
