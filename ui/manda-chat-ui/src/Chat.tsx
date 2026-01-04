import { useState } from "react";
import { invoke } from "@tauri-apps/api/core";

function Chat() {
  const [messages, setMessages] = useState<string[]>([]);
  const [input, setInput] = useState("");

  const sendMessage = async () => {
    if (!input.trim()) return;

    await invoke("send_message", {
      text: input,
    });

    setMessages([...messages, input]);
    setInput("");
  };

  return (
    <div>
      <div>
        {messages.map((m, i) => (
          <div key={i}>{m}</div>
        ))}
      </div>

      <input
        value={input}
        onChange={(e) => setInput(e.target.value)}
        placeholder="Escribí un mensaje"
      />

      <button onClick={sendMessage}>Enviar</button>
    </div>
  );
}

export default Chat;
