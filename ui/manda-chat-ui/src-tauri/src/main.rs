#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use std::process::{Command, Stdio, Child, ChildStdin};
use std::sync::Mutex;
use std::io::{BufRead, BufReader, Write};
use tauri::{AppHandle, Emitter};

struct ClientState {
    child: Mutex<Option<Child>>,
    stdin: Mutex<Option<ChildStdin>>,
}


#[tauri::command]
fn start_client(
    name: String,
    ip: String,
    port: String,
    state: tauri::State<ClientState>,
    app_handle: AppHandle, // Inyectamos el handle para emitir eventos
) -> Result<String, String> {

    let mut cmd = Command::new("../../../core/client-chat/bin/client-chat");

    // Importante: piped para ambos
    cmd.stdin(Stdio::piped())
       .stdout(Stdio::piped()) 
       .stderr(Stdio::inherit());

    let mut child = cmd.spawn().map_err(|e| e.to_string())?;
    let mut stdin = child.stdin.take().ok_or("No stdin")?;
    let stdout = child.stdout.take().ok_or("No stdout")?;

    // Enviamos el comando init
    let init_msg = format!(
        "{{\"cmd\":\"init\",\"name\":\"{}\",\"ip\":\"{}\",\"port\":\"{}\"}}\n",
        name, ip, port
    );
    stdin.write_all(init_msg.as_bytes()).map_err(|e| e.to_string())?;
    stdin.flush().unwrap();

    // Guardamos stdin en el estado
    *state.stdin.lock().unwrap() = Some(stdin);

    // Hilo para leer stdout del C y mandarlo a la UI
    std::thread::spawn(move || {
        let reader = BufReader::new(stdout);
        for line in reader.lines() {
            if let Ok(l) = line {

                println!("C dice: {}", l); 

                let _ = app_handle.emit("backend-msg", l);
            }
        }
    });

    *state.child.lock().unwrap() = Some(child);
    Ok("Cliente conectado".into())
}

#[tauri::command]
fn send_message(
    text: String,
    state: tauri::State<ClientState>,
) -> Result<(), String> {

    let mut guard = state.stdin.lock().unwrap();
    let stdin = guard.as_mut().ok_or("Cliente no iniciado")?;

    let msg = format!(
        "{{\"cmd\":\"send\",\"text\":\"{}\"}}\n",
        text
    );

    use std::io::Write;
    stdin.write_all(msg.as_bytes())
        .map_err(|e| e.to_string())?;
    stdin.flush().unwrap();

    Ok(())
}

fn main() {
    tauri::Builder::default()
        .manage(ClientState {
            child: Mutex::new(None),
            stdin: Mutex::new(None),
        })
        .invoke_handler(tauri::generate_handler![
            start_client,
            send_message
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
