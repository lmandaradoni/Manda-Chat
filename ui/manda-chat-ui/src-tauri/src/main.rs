#![cfg_attr(not(debug_assertions), windows_subsystem = "windows")]

use std::process::{Command, Stdio, ChildStdin};
use std::sync::Mutex;
use std::io::Write;

use tauri::State;

struct ClientState {
    stdin: Mutex<Option<ChildStdin>>
}


//#[tauri::command]
// fn start_client(name: String, ip: String, port: String) -> String {
//     println!("INIT recibido desde UI:");
//     println!("Nombre: {}", name);
//     println!("IP: {}", ip);
//     println!("Puerto: {}", port);

//     format!("Cliente {} inicializado en {}:{}", name, ip, port)
// }

#[tauri::command]
fn start_client(
    name: String,
    ip: String,
    port: String,
    state: State<ClientState>,
) -> Result<String, String> {

    println!("INIT recibido desde UI:");
    println!("Nombre: {}", name);
    println!("IP: {}", ip);
    println!("Puerto: {}", port);

    let mut child = Command::new("/home/utnso/Manda-Chat/core/client-chat/bin/client-chat",
    ) 
        .stdin(Stdio::piped())
        .stdout(Stdio::inherit())
        .stderr(Stdio::inherit())
        .spawn()
        .map_err(|e| e.to_string())?;

    let stdin = child.stdin.take();

    *state.stdin.lock().unwrap() = stdin;

    Ok(format!(
        "Cliente {} conectado a {}:{}",
        name, ip, port
    ))
}


#[tauri::command]
fn send_message(
    text: String,
    state: State<ClientState>,
) -> Result<(), String> {

    let payload = format!(
        r#"{{ "cmd": "send", "text": "{}" }}"#,
        text
    );

    let mut guard = state.stdin.lock().unwrap();

    if let Some(stdin) = guard.as_mut() {
        writeln!(stdin, "{}", payload)
            .map_err(|e| e.to_string())?;
        Ok(())
    } else {
        Err("Cliente no inicializado".into())
    }
}




fn main() {
    tauri::Builder::default()
        .manage(ClientState {
            stdin: Mutex::new(None),
        })
        .invoke_handler(tauri::generate_handler![
            start_client,
            send_message
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

