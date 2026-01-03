#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use std::{
    io::{BufRead, BufReader, Write},
    process::{Command, Stdio},
    sync::mpsc::{channel, Sender},
    thread,
};

use tauri::{Emitter, Listener};

fn main() {
    tauri::Builder::default()
        .setup(|app| {
            let app_handle = app.handle().clone();

            // 1. Creamos un canal para mandar mensajes desde la UI hacia el hilo del proceso C
            let (tx, rx) = channel::<String>();

            // 2. Escuchamos el evento "ui-message" desde el Frontend (React)
            app.listen("ui-message", move |event| {
                // El payload es el texto que escribió el usuario
                let msg_text = event.payload(); 
                // Le damos formato JSON para C: {"cmd":"send", "text":"hola"}
                // Quitamos comillas extra si el payload viene como string JSONizado
                let clean_text = msg_text.trim_matches('"');
                let json_command = format!(r#"{{"cmd":"send","text":"{}"}}"#, clean_text);
                
                // Lo mandamos por el canal al hilo escritor
                let _ = tx.send(json_command);
            });

            // 3. Hilo principal del proceso C
            thread::spawn(move || {
                let mut child = Command::new("../../../core/client-chat/bin/client-chat")
                    .stdin(Stdio::piped())
                    .stdout(Stdio::piped())
                    .spawn()
                    .expect("No se pudo lanzar el cliente C");

                let mut stdin = child.stdin.take().expect("Falló stdin");
                let stdout = child.stdout.take().expect("Falló stdout");

                // --- SUB-HILO DE LECTURA (C -> UI) ---
                let handle_lectura = app_handle.clone();
                thread::spawn(move || {
                    let reader = BufReader::new(stdout);
                    for line in reader.lines() {
                        if let Ok(json_line) = line {
                            println!("C dice: {}", json_line);
                            let _ = handle_lectura.emit("client-event", json_line);
                        }
                    }
                });

                // --- HILO DE ESCRITURA (UI -> C) ---
                // A. Mandamos el INIT apenas arranca
                let init_cmd = r#"{"cmd":"init","name":"TauriUser","ip":"127.0.0.1","port":"9191"}"#;
                writeln!(stdin, "{}", init_cmd).expect("Error escribiendo init");

                // B. Esperamos mensajes que vengan del canal (desde la UI)
                for msg in rx {
                    writeln!(stdin, "{}", msg).expect("Error enviando mensaje a C");
                }
            });

            Ok(())
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}