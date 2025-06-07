import React, { useContext } from 'react';
import { View, Text, StyleSheet, Pressable } from 'react-native';
import { WebSocketContext } from '../contexts/WebSocketContext';

export default function HomeScreen({ navigation }) {
    const ws = useContext(WebSocketContext);

    const startSinglePlayer = () => {
        console.log('🎮 Navigating to difficulty selection');
        navigation.navigate('Difficulty');
    };


    const startMultiplayer = () => {
        const configMessage = {
            type: 'config',
            mode: 'multiplayer'
        };

        if (ws.current?.readyState === WebSocket.OPEN) {
            ws.current.send(JSON.stringify(configMessage));
            console.log('📨 Sent multiplayer config to backend:', configMessage);
        } else {
            console.warn('⚠️ WebSocket not ready, multiplayer config not sent.');
        }

        navigation.navigate('MultiplayerLobby');
    };

    return (
        <View style={styles.container}>
            <Text style={styles.title}>Labyrinth Sprint</Text>
            <Text style={styles.subtitle}>Ready to Play?</Text>

            <Pressable
                style={[styles.button, styles.singlePlayer]}
                onPress={startSinglePlayer} // ✅ Now sends config
            >
                <Text style={styles.buttonText}>Single Player</Text>
            </Pressable>

            <Pressable
                style={[styles.button, styles.multiPlayer]}
                onPress={startMultiplayer}
            >
                <Text style={styles.buttonText}>Multiplayer</Text>
            </Pressable>
        </View>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        backgroundColor: '#0d0d0d',
        alignItems: 'center',
        justifyContent: 'center',
        padding: 20,
    },
    title: {
        fontSize: 36,
        color: '#fff',
        fontWeight: 'bold',
        marginBottom: 10,
    },
    subtitle: {
        fontSize: 22,
        color: '#aaa',
        marginBottom: 40,
    },
    button: {
        width: '80%',
        paddingVertical: 16,
        borderRadius: 10,
        alignItems: 'center',
        marginVertical: 12,
    },
    buttonText: {
        fontSize: 20,
        fontWeight: '600',
        color: '#fff',
    },
    singlePlayer: {
        backgroundColor: '#3498db',
    },
    multiPlayer: {
        backgroundColor: '#9b59b6',
    },
});

