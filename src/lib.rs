use android_activity::{AndroidApp, MainEvent, PollEvent};
use log::info;

#[no_mangle]
fn android_main(app: AndroidApp) {
    // Setup Logger agar muncul di logcat
    android_logger::init_once(
        android_logger::Config::default().with_tag("ZAMERA").with_max_level(log::LevelFilter::Info),
    );

    info!("Zamera Pure Rust Started!");

    loop {
        // Hemat Baterai: poll_events akan menunggu event masuk
        // Timeout Some(0) agar tidak memblock total jika ingin rendering kontinyu
        app.poll_events(Some(std::time::Duration::from_millis(10)), |event| {
            match event {
                PollEvent::Main(MainEvent::InitWindow { .. }) => {
                    info!("Window telah siap di Rust!");
                }
                PollEvent::Main(MainEvent::Destroy) => {
                    info!("Aplikasi ditutup");
                    return;
                }
                _ => {}
            }
        });
    }
}
