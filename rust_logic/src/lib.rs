use std::os::raw::c_float;

#[no_mangle]
pub extern "C" fn get_rust_rotation(t: c_float) -> c_float {
    // Putaran super cepat biar kelihatan bedanya
    (t * 200.0) % 360.0
}

#[no_mangle]
pub extern "C" fn get_rust_scale(t: c_float) -> c_float {
    // Efek membal (bounce)
    1.0 + (t * 5.0).sin().abs() * 0.5
}

#[no_mangle]
pub extern "C" fn get_rust_message() -> *const u8 {
    "ZAMERA HYBRID V1\0".as_ptr()
}
