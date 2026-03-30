#[no_mangle]
pub extern "C" fn get_rust_rotation(_t: f32) -> f32 {
    // Return angka statis dulu: 45 derajat
    45.0
}

#[no_mangle]
pub extern "C" fn get_rust_scale(_t: f32) -> f32 {
    // Skala normal
    1.0
}
