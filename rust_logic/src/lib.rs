use sha2::{Sha256, Digest};
use std::ffi::CString;
use std::os::raw::{c_char, c_int};

static mut NONCE: u64 = 0;
static mut LAST_HASH: String = String::new();

#[no_mangle]
pub extern "C" fn rust_mining_tick() -> *mut c_char {
    unsafe {
        let mut hasher = Sha256::new();
        let data = format!("OdfizBlock-{}", NONCE);
        hasher.update(data.as_bytes());
        let result = hasher.finalize();
        
        let hash_str = format!("{:x}", result);
        LAST_HASH = hash_str.clone();
        NONCE += 1;

        // Kirim string hash ke C untuk ditampilkan
        let c_str = CString::new(hash_str).unwrap();
        c_str.into_raw()
    }
}

#[no_mangle]
pub extern "C" fn get_nonce() -> c_int {
    unsafe { NONCE as c_int }
}
