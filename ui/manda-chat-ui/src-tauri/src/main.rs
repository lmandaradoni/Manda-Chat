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
    app_handle: tauri::AppHandle, 
) -> Result<String, String> {

   
    let current_dir = std::env::current_dir().map_err(|e| format!("Error de CWD: {}", e))?;

    
    let mut bin_path = current_dir.clone();
  
    if bin_path.ends_with("src-tauri") {
        bin_path.pop(); // a manda-chat-ui
    }
    
    bin_path.pop(); // de manda-chat-ui a ui
    bin_path.pop(); // de ui a Manda-Chat 
 
    bin_path.push("core");
    bin_path.push("client-chat");
    bin_path.push("bin");
    bin_path.push("client-chat");

    println!("Intentando ejecutar binario en: {:?}", bin_path);

  
    if !bin_path.exists() {
        return Err(format!(
            "El archivo no existe en la ruta: {:?}. ¿Hiciste 'make' en la carpeta del cliente?", 
            bin_path
        ));
    }

    let mut cmd = Command::new(bin_path);

    cmd.stdin(Stdio::piped())
       .stdout(Stdio::piped()) 
       .stderr(Stdio::inherit());

    let mut child = cmd.spawn().map_err(|e| format!("Fallo al lanzar proceso C: {}", e))?;
    let mut stdin = child.stdin.take().ok_or("No se pudo abrir el canal stdin del C")?;
    let stdout = child.stdout.take().ok_or("No se pudo abrir el canal stdout del C")?;

    let init_msg = format!(
        "{{\"cmd\":\"init\",\"name\":\"{}\",\"ip\":\"{}\",\"port\":\"{}\"}}\n",
        name, ip, port
    );
    
    use std::io::Write; // Asegúrate de tener esto para el write_all
    stdin.write_all(init_msg.as_bytes()).map_err(|e| e.to_string())?;
    stdin.flush().unwrap();

    *state.stdin.lock().unwrap() = Some(stdin);

    std::thread::spawn(move || {
        let reader = std::io::BufReader::new(stdout);
        use std::io::BufRead;
        for line in reader.lines() {
            if let Ok(l) = line {
                println!("C dice: {}", l); 
                let _ = app_handle.emit("backend-msg", l);
            }
        }
    });

    *state.child.lock().unwrap() = Some(child);
    Ok("Cliente iniciado y conectando...".into())
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

#[tauri::command]
fn stop_client(state: tauri::State<ClientState>) -> Result<(), String> {
    let mut child_guard = state.child.lock().unwrap();
    
    if let Some(mut child) = child_guard.take() {
       
        let _ = child.kill(); 
        let _ = child.wait(); 
    }
    
    let mut stdin_guard = state.stdin.lock().unwrap();
    *stdin_guard = None;

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
            send_message,
            stop_client
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}
