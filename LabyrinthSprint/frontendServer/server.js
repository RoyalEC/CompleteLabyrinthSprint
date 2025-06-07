import express from "express";
import WebSocket from "ws";

const app = express();
app.use(express.static("public"));

let socket; // Declare socket at module level for future use

app.listen(3005, () => {
    console.log("✅ Frontend server is listening on port 3005");

    // Optional: keep backend WebSocket connection open for debug/logging
    setTimeout(() => {
        socket = new WebSocket("ws://localhost:9002");

        socket.on("open", () => {
            console.log("✅ WebSocket connected to backend");
            // Do not send anything automatically
        });

        socket.on("message", (message) => {
            console.log("📥 Message received from server:", message.toString());
        });

        socket.on("error", (err) => {
            console.error("❌ WebSocket error:", err.message);
        });
    }, 2000);
});




