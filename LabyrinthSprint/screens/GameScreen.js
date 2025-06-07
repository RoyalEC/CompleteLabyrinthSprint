import React, { useEffect, useContext, useState } from 'react';
import { View, Text, StyleSheet, ScrollView, Pressable } from 'react-native';
import { WebSocketContext } from '../contexts/WebSocketContext';

export default function GameScreen({ navigation }) {
    const {
        ws,
        latestGameState,
        gameOver,
        resetGameOver,
        resetGameState,
        sendGameConfig,
        lastGameConfig, // ✅ new
    } = useContext(WebSocketContext);
    const [labyrinth, setLabyrinth] = useState([]);
    const [playerPosition, setPlayerPosition] = useState({ x: 0, y: 0 });
    const [aiPosition, setAIPosition] = useState(null); // New AI state

    useEffect(() => {
        if (latestGameState?.labyrinth) {
            setLabyrinth(latestGameState.labyrinth);
        }
        if (latestGameState?.player) {
            setPlayerPosition(latestGameState.player);
        }
        if (latestGameState?.ai) {
            setAIPosition(latestGameState.ai);
        }
    }, [latestGameState, gameOver]);

    const handlePlayAgain = () => {
        resetGameOver();
        resetGameState();
        setLabyrinth([]);
        setPlayerPosition({ x: 0, y: 0 });
        setAIPosition(null);

        if (lastGameConfig) {
            console.log('🔁 Replaying with last config:', lastGameConfig);
            sendGameConfig(lastGameConfig);
        } else {
            console.warn('⚠️ No previous config found, returning to difficulty screen');
            navigation.replace('Difficulty');
        }
    };


    const handleBackToMenu = () => {
        resetGameOver();
        resetGameState();
        setLabyrinth([]);
        setPlayerPosition({ x: 0, y: 0 });
        setAIPosition(null);
        navigation.navigate('Home');
    };

    const sendMove = (direction) => {
        if (gameOver) return;
        const moveMessage = {
            playerId: 1,
            action: direction,
        };
        if (ws.current?.readyState === WebSocket.OPEN) {
            ws.current.send(JSON.stringify(moveMessage));
            console.log('📤 Sent move:', moveMessage);
        }
    };

    return (
        <View style={styles.screen}>
            <Text style={styles.title}>Labyrinth</Text>

            {labyrinth.length > 0 && (
                <Text style={styles.status}>
                    Player at ({playerPosition.x}, {playerPosition.y})
                    {aiPosition && ` | AI at (${aiPosition.x}, ${aiPosition.y})`}
                </Text>
            )}

            {gameOver && (
                <Text style={styles.gameOverText}>🎉 Game Over! You reached the end! 🎉</Text>
            )}

            {gameOver && (
                <View style={styles.postGameButtons}>
                    <Pressable style={styles.menuButton} onPress={handlePlayAgain}>
                        <Text style={styles.buttonText}>🔁 Play Again</Text>
                    </Pressable>
                    <Pressable style={styles.menuButton} onPress={handleBackToMenu}>
                        <Text style={styles.buttonText}>🏠 Main Menu</Text>
                    </Pressable>
                </View>
            )}

            <ScrollView contentContainerStyle={styles.container}>
                {labyrinth.length > 0 ? (
                    labyrinth.map((row, rowIndex) => (
                        <View key={rowIndex} style={styles.row}>
                            {typeof row === 'string' &&
                                row.split('').map((cell, colIndex) => {
                                    const isPlayerHere = playerPosition.x === colIndex && playerPosition.y === rowIndex;
                                    const isAIHere = aiPosition?.x === colIndex && aiPosition?.y === rowIndex;

                                    return (
                                        <Text
                                            key={colIndex}
                                            style={[
                                                styles.cell,
                                                isPlayerHere && styles.player,
                                                isAIHere && styles.ai
                                            ]}
                                        >
                                            {isPlayerHere ? 'P' : isAIHere ? 'A' : cell}
                                        </Text>
                                    );
                                })}
                        </View>
                    ))
                ) : (
                    <Text style={styles.waiting}>Waiting for labyrinth...</Text>
                )}
            </ScrollView>

            {!gameOver && (
                <View style={styles.controls}>
                    <Pressable onPress={() => sendMove('MoveUp')} style={styles.button}>
                        <Text style={styles.buttonText}>↑</Text>
                    </Pressable>
                    <View style={styles.row}>
                        <Pressable onPress={() => sendMove('MoveLeft')} style={styles.button}>
                            <Text style={styles.buttonText}>←</Text>
                        </Pressable>
                        <Pressable onPress={() => sendMove('MoveDown')} style={styles.button}>
                            <Text style={styles.buttonText}>↓</Text>
                        </Pressable>
                        <Pressable onPress={() => sendMove('MoveRight')} style={styles.button}>
                            <Text style={styles.buttonText}>→</Text>
                        </Pressable>
                    </View>
                </View>
            )}
        </View>
    );
}

const styles = StyleSheet.create({
    screen: {
        flex: 1,
        backgroundColor: '#000',
        paddingTop: 50,
        paddingBottom: 20,
    },
    container: {
        alignItems: 'center',
        paddingBottom: 20,
    },
    title: {
        color: '#fff',
        fontSize: 24,
        fontWeight: 'bold',
        textAlign: 'center',
        marginBottom: 10,
    },
    status: {
        color: '#0f0',
        fontSize: 14,
        textAlign: 'center',
        marginBottom: 10,
    },
    gameOverText: {
        color: '#ffcc00',
        fontSize: 16,
        textAlign: 'center',
        marginVertical: 10,
        fontWeight: 'bold',
    },
    waiting: {
        color: '#fff',
        marginTop: 20,
        textAlign: 'center',
    },
    row: {
        flexDirection: 'row',
    },
    cell: {
        width: 20,
        height: 20,
        color: '#fff',
        fontSize: 16,
        textAlign: 'center',
        fontFamily: 'Courier',
        backgroundColor: '#111',
        borderWidth: 0.5,
        borderColor: '#333',
        lineHeight: 20,
    },
    player: {
        color: '#00ffcc',
        fontWeight: 'bold',
        backgroundColor: '#005f5f',
    },
    ai: {
        color: '#ff6f00',
        fontWeight: 'bold',
        backgroundColor: '#7f3f00',
    },
    controls: {
        alignItems: 'center',
        marginTop: 20,
    },
    button: {
        backgroundColor: '#444',
        paddingVertical: 10,
        paddingHorizontal: 20,
        margin: 5,
        borderRadius: 8,
    },
    buttonText: {
        color: '#fff',
        fontSize: 20,
        fontWeight: 'bold',
    },
    postGameButtons: {
        marginTop: 20,
        alignItems: 'center',
    },
    menuButton: {
        backgroundColor: '#555',
        paddingVertical: 10,
        paddingHorizontal: 24,
        marginVertical: 6,
        borderRadius: 10,
    },
});
