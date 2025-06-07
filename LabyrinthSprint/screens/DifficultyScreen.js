import React, { useContext, useEffect, useState } from 'react';
import { View, Text, StyleSheet, Pressable } from 'react-native';
import { WebSocketContext } from '../contexts/WebSocketContext';

export default function DifficultyScreen({ navigation, route }) {
    const { latestGameState, sendGameConfig } = useContext(WebSocketContext); // ✅ Use sendGameConfig
    const [difficulty, setDifficulty] = useState(null);
    const [configSent, setConfigSent] = useState(false);

    const mode = route.params?.mode || 'single';

    const selectDifficulty = (level) => {
        const config = {
            mode: mode,
            difficulty: level.toLowerCase()
        };

        sendGameConfig(config); // ✅ Replaces direct WebSocket send
        setConfigSent(true);
        setDifficulty(level);
    };

    useEffect(() => {
        if (configSent && latestGameState?.labyrinth && latestGameState?.player) {
            console.log('✅ Game state received, navigating to Game');
            navigation.navigate('Game', { difficulty, mode }); // optionally pass mode
        }
    }, [configSent, latestGameState]);

    return (
        <View style={styles.container}>
            <Text style={styles.title}>Choose Difficulty</Text>
            <Text style={styles.modeText}>Mode: {mode === 'single' ? 'Local (vs AI)' : 'Multiplayer'}</Text>

            <Pressable style={[styles.button, styles.easy]} onPress={() => selectDifficulty('Easy')}>
                <Text style={styles.buttonText}>Easy</Text>
            </Pressable>

            <Pressable style={[styles.button, styles.medium]} onPress={() => selectDifficulty('Medium')}>
                <Text style={styles.buttonText}>Medium</Text>
            </Pressable>

            <Pressable style={[styles.button, styles.hard]} onPress={() => selectDifficulty('Hard')}>
                <Text style={styles.buttonText}>Hard</Text>
            </Pressable>
        </View>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        backgroundColor: '#000',
        justifyContent: 'center',
        alignItems: 'center',
        padding: 20,
    },
    title: {
        fontSize: 26,
        fontWeight: 'bold',
        color: '#fff',
        marginBottom: 10,
    },
    modeText: {
        fontSize: 16,
        color: '#888',
        marginBottom: 20,
    },
    button: {
        paddingVertical: 14,
        paddingHorizontal: 40,
        borderRadius: 10,
        marginVertical: 10,
        width: '80%',
        alignItems: 'center',
    },
    easy: {
        backgroundColor: '#27ae60',
    },
    medium: {
        backgroundColor: '#f39c12',
    },
    hard: {
        backgroundColor: '#c0392b',
    },
    buttonText: {
        fontSize: 18,
        fontWeight: 'bold',
        color: '#fff',
    },
});
