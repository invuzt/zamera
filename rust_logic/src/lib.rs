use sha2::{Sha256, Digest};
use std::os::raw::{c_float, c_int};
use std::time::{Instant};

static mut NONCE: u64 = 0;
static mut LAST_TIME: Option<Instant> = None;
static mut HASHRATE: f32 = 0.0;

#[no_mangle]
pub extern "C" fn rust_mining_next_val() -> c_float {
    unsafe {
        if LAST_TIME.is_none() { LAST_TIME = Some(Instant::now()); }
        
        let mut hasher = Sha256::new();
        let data = format!("Odfiz_Miner_{}", NONCE);
        hasher.update(data.as_bytes());
        let result = hasher.finalize();
        
        NONCE += 1;

        // Hitung Hashrate setiap 1000 nonce agar tidak berat
        if NONCE % 1000 == 0 {
            let now = Instant::now();
            let duration = now.duration_since(LAST_TIME.unwrap()).as_secs_f32();
            if duration > 1.0 {
                // Formula: (Jumlah Hash / Waktu) / 1.000.000 = MH/s
                HASHRATE = (NONCE as f32 / duration) / 1_000_000.0;
                // Reset untuk pengukuran berikutnya
                // NONCE = 0; // Opsional: reset atau lanjut
                // LAST_TIME = Some(now);
            }
        }
        
        (result[0] as f32) / 255.0
    }
}

#[no_mangle]
pub extern "C" fn get_hashrate() -> c_float {
    unsafe { HASHRATE }
}

#[no_mangle]
pub extern "C" fn get_nonce() -> c_int {
    unsafe { NONCE as c_int }
}
