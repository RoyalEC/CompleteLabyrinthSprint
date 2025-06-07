import React, { useEffect, useRef, useState } from 'react';
import {
    View,
    Text,
    StyleSheet,
    Animated,
    TouchableOpacity,
    Platform,
} from 'react-native';
import { useNavigation } from '@react-navigation/native';

export default function MultiplayerLobby() {
    const [progress] = useState(new Animated.Value(0));
    const ws = useRef(null);
    const navigation = useNavigation();

    useEffect(() => {
        Animated.loop(
            Animated.timing(progress, {
                toValue: 1,
                duration: 4000,
                useNativeDriver: false,
            })
        ).start();

        let wsUrl;

        if (Platform.OS === 'web') {
            const { hostname } = window.location;
            wsUrl = `ws://${hostname}:9002`;
        } else {
            // Replace with your machine's local IP address during development
            wsUrl = 'ws://10.0.0.81:9002';
        }

        ws.current = new WebSocket(wsUrl);

        ws.current.onopen = () => {
            console.log('WebSocket connected');
        };

        ws.current.onmessage = (event) => {
            console.log('Received message:', event.data);
        };

        ws.current.onerror = (error) => {
            console.error('WebSocket error:', error.message || error);
        };

        ws.current.onclose = () => {
            console.log('WebSocket disconnected');
        };

        return () => {
            if (ws.current) {
                ws.current.close();
            }
        };
    }, []);

    return (
        <View style={styles.container}>
            <Text style={styles.title}>Select Multiplayer Mode</Text>

            <TouchableOpacity
                style={styles.activeButton}
                onPress={() => navigation.navigate('Difficulty', { mode: 'local' })}
            >
                <Text style={styles.buttonText}>Local (vs AI)</Text>
            </TouchableOpacity>

            <TouchableOpacity style={styles.disabledButton} disabled>
                <Text style={styles.disabledText}>Online (coming soon)</Text>
            </TouchableOpacity>

            <TouchableOpacity
                style={styles.cancelButton}
                onPress={() => navigation.navigate('Home')}
            >
                <Text style={styles.cancelText}>Cancel</Text>
            </TouchableOpacity>
        </View>
    );
}

const styles = StyleSheet.create({
    container: {
        flex: 1,
        backgroundColor: '#0d0d0d',
        justifyContent: 'center',
        alignItems: 'center',
        padding: 20,
    },
    title: {
        fontSize: 26,
        fontWeight: 'bold',
        color: '#ffffff',
        marginBottom: 30,
    },
    activeButton: {
        backgroundColor: '#27ae60',
        paddingVertical: 14,
        paddingHorizontal: 30,
        borderRadius: 10,
        marginBottom: 20,
    },
    buttonText: {
        color: '#fff',
        fontSize: 18,
        fontWeight: 'bold',
    },
    disabledButton: {
        backgroundColor: '#555',
        paddingVertical: 14,
        paddingHorizontal: 30,
        borderRadius: 10,
        marginBottom: 20,
    },
    disabledText: {
        color: '#aaa',
        fontSize: 18,
        fontWeight: 'bold',
    },
    cancelButton: {
        marginTop: 20,
        backgroundColor: '#e74c3c',
        paddingVertical: 12,
        paddingHorizontal: 25,
        borderRadius: 10,
    },
    cancelText: {
        color: '#fff',
        fontSize: 16,
        fontWeight: 'bold',
    },
});
