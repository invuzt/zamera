use std::os::raw::c_float;

#[no_mangle]
pub extern "C" fn get_rust_status() -> c_float { 
    1.0 
}

#[no_mangle]
pub extern "C" fn get_rust_color_r(t: c_float) -> c_float {
    (t * 1.5).sin().abs()
}

#[no_mangle]
pub extern "C" fn get_rust_color_g(t: c_float) -> c_float {
    (t * 2.0).cos().abs()
}
