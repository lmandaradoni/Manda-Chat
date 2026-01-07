import { useState } from "react";
import "./styles.css";
import { invoke } from "@tauri-apps/api/core";
import Chat from "./Chat";



function App() {
  const [name, setName] = useState("");
  const [ip, setIp] = useState("");
  const [port, setPort] = useState("");


  const handleConnect = async () => {
    try {
      setStatus("connecting");
      setMessage("");

      const response = await invoke<string>("start_client", {
        name,
        ip,
        port,
      });

      setStatus("connected");
      setMessage(response);
    } catch (err) {
      setStatus("error");
      setMessage(String(err));
    }
  };



  const [status, setStatus] = useState<
    "idle" | "connecting" | "connected" | "error"
  >("idle");

  const [message, setMessage] = useState("");

  const isDisabled = status === "connecting" || status === "connected";


  const handleDisconnect = () => {
    setStatus("idle");

    invoke("stop_client").catch(console.error);
  };


return (
  <div className="app">
    {status !== "connected" ? (
      <>
        <h1>Manda Chat</h1>

        <form
          className="form"
          onSubmit={(e) => {
            e.preventDefault();
            if (!isDisabled) {
              handleConnect();
            }
          }}
        >
          <input
            placeholder="Nombre"
            value={name}
            disabled={isDisabled}
            onChange={(e) => setName(e.target.value)}
          />

          <input
            placeholder="IP del servidor"
            value={ip}
            disabled={isDisabled}
            onChange={(e) => setIp(e.target.value)}
          />

          <input
            placeholder="Puerto"
            value={port}
            disabled={isDisabled}
            onChange={(e) => setPort(e.target.value)}
          />

          <button type="submit" disabled={isDisabled}>
            {status === "connecting" ? "Conectando..." : "Conectar"}
          </button>

          {status === "error" && (
            <div className="error">{message}</div>
          )}
        </form>
      </>
    ) : (
      <Chat onDisconnect={handleDisconnect} />
    )}
  </div>
);


}

export default App;