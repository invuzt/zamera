use sha2::{Sha256, Digest};
use std::os::raw::{c_float};
use std::time::{Instant};

static mut NONCE: u64 = 0;
static mut LAST_TIME: Option<Instant> = None;
static mut HASHRATE_PCT: f32 = 0.0; // 0.0 sampai 1.0 (Target 5 MH/s)

#[no_mangle]
pub extern "C" fn rust_mining_next_val() -> c_float {
    unsafe {
        if LAST_TIME.is_none() { LAST_TIME = Some(Instant::now()); }
        
        let mut hasher = Sha256::new();
        let data = format!("Odfiz_Miner_{}", NONCE);
        hasher.update(data.as_bytes());
        let result = hasher.finalize();
        
        NONCE += 1;

        if NONCE % 2000 == 0 {
            let now = Instant::now();
            let duration = now.duration_since(LAST_TIME.unwrap()).as_secs_f32();
            if duration > 0.5 {
                let mhs = (NONCE as f32 / duration) / 1_000_000.0;
                // Kita asumsikan 5 MH/s adalah 100% panjang bar
                HASHRATE_PCT = (mhs / 5.0).min(1.0); 
            }
        }
        (result[0] as f32) / 255.0
    }
}

#[no_mangle]
pub extern "C" fn get_hashrate_pct() -> c_float {
    unsafe { HASHRATE_PCT }
}
