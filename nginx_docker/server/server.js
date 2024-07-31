const express = require('express');
const multer = require('multer');
const app = express();

// Configure multer for file uploads
const upload = multer({ dest: 'uploads/' });

// Endpoint for handling file uploads
app.post('/upload', upload.any(), (req, res) => {
    res.send('Files uploaded successfully');
});

app.listen(3000, () => {
    console.log('Server is running on http://localhost:3000');
});

