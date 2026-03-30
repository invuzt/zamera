use std::os::raw::c_float;

#[no_mangle]
pub extern "C" fn get_rust_rotation(current_time: c_float) -> c_float {
    // Logika aman Rust: Menghitung rotasi berdasarkan waktu
    // Di sini kamu bisa tambah logika kompleks tanpa takut memory leak
    (current_time * 2.0).sin() * 50.0
}

#[no_mangle]
pub extern "C" fn get_rust_color() -> u32 {
    // Contoh return warna dari Rust (Format HEX: 0xAABBGGRR)
    0xFF00FF00 // Hijau mantap
}
