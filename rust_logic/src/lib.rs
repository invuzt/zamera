use sha2::{Sha256, Digest};
use std::os::raw::{c_float, c_int};

static mut NONCE: u64 = 0;

// Fungsi Rust untuk memberikan data hash yang acak dan dinamis
#[no_mangle]
pub extern "C" fn rust_mining_next_val() -> c_float {
    unsafe {
        let mut hasher = Sha256::new();
        let data = format!("Odfiz_Digital_{}", NONCE);
        hasher.update(data.as_bytes());
        let result = hasher.finalize();
        
        NONCE += 1;
        
        // Kita gabungkan byte hash agar lebih dinamis nilainya
        let combined_hash_val = (result[0] as f32) * (result[3] as f32) / 65025.0; // 0.0 - 1.0
        combined_hash_val
    }
}

#[no_mangle]
pub extern "C" fn get_nonce() -> c_int {
    unsafe { NONCE as c_int }
}
