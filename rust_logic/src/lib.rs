use std::os::raw::c_float;

#[no_mangle]
pub extern "C" fn get_rust_rotation(t: c_float) -> c_float {
    // Putaran yang lebih dinamis: makin lama makin cepat
    (t * 50.0) % 360.0
}

#[no_mangle]
pub extern "C" fn get_rust_scale(t: c_float) -> c_float {
    // Efek detak jantung (Zoom in/out) dari Rust
    1.0 + (t * 4.0).sin() * 0.3
}

#[no_mangle]
pub extern "C" fn get_rust_message() -> *const u8 {
    // Teks yang akan ditampilkan di layar
    "Zamera: Hello from Rust!\0".as_ptr()
}
