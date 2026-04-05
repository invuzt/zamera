use sha2::{Sha256, Digest};
use std::os::raw::{c_float, c_int};

static mut NONCE: u64 = 0;

#[no_mangle]
pub extern "C" fn rust_mining_next_val() -> c_float {
    unsafe {
        let mut hasher = Sha256::new();
        let data = format!("Odfiz-{}", NONCE);
        hasher.update(data.as_bytes());
        let result = hasher.finalize();
        
        NONCE += 1;
        // Ambil byte pertama dari hash dan ubah jadi float 0.0 - 1.0
        (result[0] as f32) / 255.0
    }
}

#[no_mangle]
pub extern "C" fn get_nonce() -> c_int {
    unsafe { NONCE as c_int }
}
