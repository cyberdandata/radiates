const WebSocket = require('ws');
const express = require('express');
const http = require('http');

// Set up an Express app and HTTP server
const app = express();
const server = http.createServer(app);

// Serve static files for frontend
app.use(express.static('public'));

// Create WebSocket server
const wss = new WebSocket.Server({ server });

let clients = [];

// WebSocket connection handler
wss.on('connection', (ws) => {
    console.log('New client connected');
    clients.push(ws);

    // When a client sends a message
    ws.on('message', (message) => {
        console.log('Received message:', message);

        // Broadcast the message to all connected clients
        clients.forEach(client => {
            if (client.readyState === WebSocket.OPEN) {
                client.send(message);
            }
        });
    });

    // Remove client on disconnect
    ws.on('close', () => {
        console.log('Client disconnected');
        clients = clients.filter(client => client !== ws);
    });
});

// Start the server
server.listen(3000, () => {
    console.log('Server is listening on port 3000');
});

