import { useState, useEffect } from "react";
import { invoke } from "@tauri-apps/api/core";
import { listen } from "@tauri-apps/api/event";

interface ChatProps {
  onDisconnect: () => void;
}

interface BackendPayload {
  type: string;
  de?: string;
  text?: string;
  mensaje?: string;
}

function Chat({ onDisconnect }: ChatProps) {
  const [messages, setMessages] = useState<string[]>([]);
  const [input, setInput] = useState("");

  useEffect(() => {
    const setupListener = async () => {
      const unlisten = await listen<string>("backend-msg", (event) => {
        console.log("Llegó de Rust:", event.payload); 
        try {
          const data: BackendPayload = JSON.parse(event.payload);
          
          if (data.type === "mensaje") {
            setMessages((prev) => [...prev, `${data.de}: ${data.text}`]);
          }
          
          else if (data.type === "desconexion" || data.type === "error"){
            console.log("Desconexion recibida:", data);
            onDisconnect(); 
          }

        } catch (e) {
          console.error("Error parseando JSON:", event.payload);
        }
      });
      return unlisten;
    };

    const listenerPromise = setupListener();


    return () => {
      listenerPromise.then(unlisten => unlisten());
    };
  }, [onDisconnect]); 

  const handleQuit = async () => {
    
    await invoke("stop_client");
    
      onDisconnect(); 
  };

  const sendMessage = async () => {
    if (!input.trim()) return;
    await invoke("send_message", { text: input });
    setInput("");
  };

  return (
    <div className="chat-container" style={{ padding: 20 }}>
      <div style={{ marginBottom: 10, overflow: 'hidden' }}>
        <h2 style={{ float: 'left', margin: 0 }}>Sala de Chat</h2>
        <button onClick={handleQuit} style={{ float: 'right' }}>Desconectar</button>
      </div>
      
      <div className="messages" style={{ 
          border: '1px solid #ccc', 
          height: '300px', 
          overflowY: 'auto', 
          padding: '10px',
          marginBottom: '10px',
          background: '#fff'
      }}>
        {messages.map((m, i) => (
          <div key={i} className="message-line" style={{ marginBottom: '5px' }}>{m}</div>
        ))}
      </div>

      <div className="input-area" style={{ display: 'flex', gap: '10px' }}>
        <input
          value={input}
          onChange={(e) => setInput(e.target.value)}
          onKeyDown={(e) => e.key === 'Enter' && sendMessage()}
          placeholder="Escribí un mensaje..."
          style={{ flex: 1, padding: '5px' }}
        />
        <button onClick={sendMessage} style={{ padding: '5px 15px' }}>Enviar</button>
      </div>
    </div>
  );
}

export default Chat;