import { useEffect, useState } from "react";
import { listen, emit } from "@tauri-apps/api/event"; // <--- Importar 'emit'

// ... (tus tipos y estados siguen igual) ...

function App() {
  const [messages, setMessages] = useState<ServerMessage[]>([]);
  const [status, setStatus] = useState("Desconectado");
  const [inputText, setInputText] = useState(""); // Estado para el input

  // ... (tu useEffect sigue igual) ...

  // Función para enviar mensaje
  async function sendMessage(e: React.FormEvent) {
    e.preventDefault();
    if (!inputText.trim()) return;

    // Enviamos el texto a Rust
    await emit("ui-message", inputText);
    
    setInputText(""); // Limpiar input
  }

  return (
    <div style={{ padding: 20, fontFamily: "sans-serif", display: "flex", flexDirection: "column", height: "90vh" }}>
      <h1>🟢 Manda Chat</h1>
      <div style={{ padding: "10px", background: "#eee", marginBottom: 10 }}>
        Estado: <strong>{status}</strong>
      </div>

      <div style={{ flex: 1, border: "1px solid #ccc", padding: 10, overflowY: "auto", marginBottom: 10 }}>
        {messages.map((m, index) => (
          <div key={index} style={{ marginBottom: 8 }}>
            {m.type === "mensaje" && (
              <span><strong>{m.de}:</strong> {m.text}</span>
            )}
            {m.type === "error" && (
              <span style={{ color: "red" }}>⚠️ {m.msg}</span>
            )}
          </div>
        ))}
      </div>

      {/* Formulario de envío */}
      <form onSubmit={sendMessage} style={{ display: "flex", gap: 10 }}>
        <input
          value={inputText}
          onChange={(e) => setInputText(e.target.value)}
          placeholder="Escribe un mensaje..."
          style={{ flex: 1, padding: 8 }}
        />
        <button type="submit" style={{ padding: "8px 16px" }}>Enviar</button>
      </form>
    </div>
  );
}

export default App;