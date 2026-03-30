use std::os::raw::c_float;

static mut TOUCH_X: f32 = 0.5;
static mut TOUCH_Y: f32 = 0.5;

#[no_mangle]
pub extern "C" fn set_rust_touch(x: c_float, y: c_float) {
    unsafe {
        TOUCH_X = x;
        TOUCH_Y = y;
    }
}

#[no_mangle]
pub extern "C" fn get_rust_color_r(t: c_float) -> c_float {
    unsafe { (t * (TOUCH_X * 5.0)).sin().abs() }
}

#[no_mangle]
pub extern "C" fn get_rust_color_g(t: c_float) -> c_float {
    unsafe { (t * (TOUCH_Y * 5.0)).cos().abs() }
}
