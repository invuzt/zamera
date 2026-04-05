use sha2::{Sha256, Digest};
use std::os::raw::{c_float, c_int};
use std::thread;
use std::time::{Duration};

static mut NONCE: u64 = 0;

#[no_mangle]
pub extern "C" fn rust_mining_next_val() -> c_float {
    unsafe {
        let mut hasher = Sha256::new();
        let data = format!("Odfiz-{}", NONCE);
        hasher.update(data.as_bytes());
        let result = hasher.finalize();
        
        NONCE += 1;
        
        // PERLAMBAT: Beri jeda 10 milidetik setiap hash 
        // agar mata manusia bisa mengikuti pergerakan visualnya
        thread::sleep(Duration::from_millis(10));
        
        (result[0] as f32) / 255.0
    }
}

#[no_mangle]
pub extern "C" fn get_nonce_last_digit() -> c_int {
    unsafe { (NONCE % 10) as c_int }
}
