import React from 'react';
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import HomeScreen from './screens/HomeScreen';
import DifficultyScreen from './screens/DifficultyScreen';
import MuliplayerLobby from './screens/MultiplayerLobby';
import GameScreen from './screens/GameScreen';
import { WebSocketProvider } from './contexts/WebSocketContext';

const Stack = createNativeStackNavigator();

export default function App() {
    return (
        <WebSocketProvider>  {/* ✅ Add this */}
            <NavigationContainer>
                <Stack.Navigator initialRouteName="Home" screenOptions={{ headerShown: false }}>
                    <Stack.Screen name="Home" component={HomeScreen} />
                    <Stack.Screen name="Difficulty" component={DifficultyScreen} />
                    <Stack.Screen name="MultiplayerLobby" component={MuliplayerLobby} />
                    <Stack.Screen name="Game" component={GameScreen} />
                </Stack.Navigator>
            </NavigationContainer>
        </WebSocketProvider>
    );
}






