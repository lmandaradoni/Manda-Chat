import { useState, useEffect } from "react";
import { invoke } from "@tauri-apps/api/core";
import { listen } from "@tauri-apps/api/event";

interface BackendPayload {
  type: string;
  de?: string;
  text?: string;
  mensaje?: string;
}

function Chat() {
  const [messages, setMessages] = useState<string[]>([]);
  const [input, setInput] = useState("");

  useEffect(() => {
    // Escuchar eventos desde Rust
    const unlisten = listen<string>("backend-msg", (event) => {
      try {
        const data: BackendPayload = JSON.parse(event.payload);
        
        if (data.type === "mensaje") {
          setMessages((prev) => [...prev, `${data.de}: ${data.text}`]);
        } else if (data.type === "error") {
          setMessages((prev) => [...prev, `SISTEMA: ${data.mensaje}`]);
        }
      } catch (e) {
        console.error("Error parseando JSON del C:", e);
      }
    });

    return () => {
      unlisten.then((f) => f());
    };
  }, []);

  const sendMessage = async () => {
    if (!input.trim()) return;

    // Ya no agregamos el mensaje manualmente aquí, 
    // porque el servidor nos lo devolverá y el listener lo atrapará.
    await invoke("send_message", { text: input });
    setInput("");
  };

  return (
    <div className="chat-container">
      <div className="messages">
        {messages.map((m, i) => (
          <div key={i} className="message-line">{m}</div>
        ))}
      </div>
      <div className="input-area">
        <input
          value={input}
          onChange={(e) => setInput(e.target.value)}
          onKeyDown={(e) => e.key === 'Enter' && sendMessage()}
          placeholder="Escribí un mensaje"
        />
        <button onClick={sendMessage}>Enviar</button>
      </div>
    </div>
  );
}

export default Chat;