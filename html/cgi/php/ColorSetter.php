<?php
// Enable error reporting for debugging
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Retrieve form data and server name
$form = $_POST;
$serverName = $_SERVER['SERVER_NAME'] ?? '';

// Initialize the HTML body
$body = "<!DOCTYPE html>
<html>
<head>
    <title>BackGround Color Is Modified</title>
    <style>
        * { font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif; }
        body { background-color: Azure; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column; }
        h1 { font-size: 3em; color: black; }
    </style>
</head>
<body>
    <h1>Color Successfully Modified</h1>
    <br><a href=\"/php/welcome.php\"><button>Go back to your profile</button></a>
</body>
</html>";

// Print the HTTP headers

// Set the background color cookie if it's in the form data
if (isset($form['BackGroundColor'])) {
    $bgcolor = htmlspecialchars($form['BackGroundColor']);
    setcookie('bgcolor', $bgcolor,  time() + (86400 * 30), '/'); // 1 hour expiration
}

header("Content-Type: text/html");
header("Content-Length: " . strlen($body));

// Print the server name and current date if available
if (!empty($serverName)) {
    echo "Server: $serverName<br>";
}

$date = gmdate("D, d M Y H:i:s T");
echo "Date: $date<br>";

// Print the HTML body
echo $body;
?>
