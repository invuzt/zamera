use std::os::raw::{c_int};

static mut SNAKE_X: [c_int; 100] = [10; 100];
static mut SNAKE_Y: [c_int; 100] = [10; 100];
static mut SNAKE_LEN: c_int = 3;
static mut DIR: c_int = 0; // 0:Right, 1:Left, 2:Up, 3:Down
static mut FOOD_X: c_int = 5;
static mut FOOD_Y: c_int = 5;

#[no_mangle]
pub extern "C" fn update_game() {
    unsafe {
        // Geser badan ular
        for i in (1..SNAKE_LEN as usize).rev() {
            SNAKE_X[i] = SNAKE_X[i-1];
            SNAKE_Y[i] = SNAKE_Y[i-1];
        }

        // Gerakkan kepala
        match DIR {
            0 => SNAKE_X[0] += 1,
            1 => SNAKE_X[0] -= 1,
            2 => SNAKE_Y[0] += 1,
            3 => SNAKE_Y[0] -= 1,
            _ => {}
        }

        // Cek Makan
        if SNAKE_X[0] == FOOD_X && SNAKE_Y[0] == FOOD_Y {
            SNAKE_LEN += 1;
            FOOD_X = (SNAKE_X[0] * 7 + 3) % 20; // Random sederhana
            FOOD_Y = (SNAKE_Y[0] * 3 + 7) % 30;
        }

        // Batas Layar (Warp)
        if SNAKE_X[0] > 20 { SNAKE_X[0] = 0; }
        if SNAKE_X[0] < 0 { SNAKE_X[0] = 20; }
        if SNAKE_Y[0] > 35 { SNAKE_Y[0] = 0; }
        if SNAKE_Y[0] < 0 { SNAKE_Y[0] = 35; }
    }
}

#[no_mangle] pub extern "C" fn set_dir(d: c_int) { unsafe { DIR = d; } }
#[no_mangle] pub extern "C" fn get_snake_x(i: c_int) -> c_int { unsafe { SNAKE_X[i as usize] } }
#[no_mangle] pub extern "C" fn get_snake_y(i: c_int) -> c_int { unsafe { SNAKE_Y[i as usize] } }
#[no_mangle] pub extern "C" fn get_snake_len() -> c_int { unsafe { SNAKE_LEN } }
#[no_mangle] pub extern "C" fn get_food_x() -> c_int { unsafe { FOOD_X } }
#[no_mangle] pub extern "C" fn get_food_y() -> c_int { unsafe { FOOD_Y } }
