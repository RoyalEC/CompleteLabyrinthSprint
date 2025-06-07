import React, { createContext, useRef, useEffect, useState } from 'react';

export const WebSocketContext = createContext(null);

export const WebSocketProvider = ({ children }) => {
    const ws = useRef(null);
    const [latestGameState, setLatestGameState] = useState(null);
    const [gameOver, setGameOver] = useState(false);
    const [lastGameConfig, setLastGameConfig] = useState(null); // ✅ NEW

    const resetGameOver = () => setGameOver(false);
    const resetGameState = () => setLatestGameState(null);

    // ✅ New function to send game config and store it
    const sendGameConfig = (config) => {
        if (ws.current?.readyState === WebSocket.OPEN) {
            const message = { type: 'config', ...config };
            ws.current.send(JSON.stringify(message));
            setLastGameConfig(config); // ✅ Save it
            console.log('📤 Sent game config:', message);
        } else {
            console.warn('⚠️ WebSocket not ready');
        }
    };

    useEffect(() => {
        ws.current = new WebSocket('ws://10.0.0.81:9002');

        ws.current.onopen = () => console.log('✅ WebSocket connected');
        ws.current.onerror = (e) => console.error('❌ WebSocket error:', e.message);
        ws.current.onclose = () => console.warn('⚠️ WebSocket closed');

        ws.current.onmessage = (e) => {
            try {
                const data = JSON.parse(e.data);
                console.log('📥 WS Global Message:', data);

                if (data.type === 'gameOver') {
                    setGameOver(true);
                } else {
                    setLatestGameState(data);
                }
            } catch (err) {
                console.error('❌ WebSocket parse error:', err);
            }
        };

        return () => ws.current && ws.current.close();
    }, []);

    return (
        <WebSocketContext.Provider value={{
            ws,
            latestGameState,
            gameOver,
            resetGameOver,
            resetGameState,
            sendGameConfig,    // ✅ Expose
            lastGameConfig     // ✅ Expose
        }}>
            {children}
        </WebSocketContext.Provider>
    );
};
