<?php
// Check if cookies are set
if (isset($_COOKIE['bgcolor']) && isset($_COOKIE['user'])) {
    $name = htmlspecialchars($_COOKIE['user']);
    $bgcolor = htmlspecialchars($_COOKIE['bgcolor']);
    $title = $name;
    $message = "Welcome to your panel !";
} else {
    $name = "Unknown User";
    $bgcolor = "LightGoldenRodYellow";
    $title = "Unknown User";
    $message = "Oops! You're not connected yet";
}

// Build the HTML body
$body = "<!DOCTYPE html>
<html>
<head>
    <title>$title</title>
    <style>
        * { font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif; }
        body { background-color: $bgcolor; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column; }
        h1 { font-size: 3em; color: white; }
    </style>
</head>
<body>
    <h1>$name</h1>
    <h2>$message</h2>";

// Add buttons based on user connection status
if (isset($_COOKIE['bgcolor']) && isset($_COOKIE['user'])) {
    $body .= "<br><a href=\"/php/ColorModifier.html\"><button> Change Your Profile Color</button></a>
    <br><a href=\"/php/LogOut.php\"><button> Log Out </button></a>";
} else {
    $body .= "<br><a href=\"/php/connectionForm.html\"><button> Create a new profile</button></a>";
}

$body .= "</body></html>";

// Print the HTTP headers
header("Content-Type: text/html");
header("Content-Length: " . strlen($body));

// Print the HTML body
echo $body;

// Print server name and date
if (isset($_SERVER['SERVER_NAME'])) {
    $serverName = $_SERVER['SERVER_NAME'];
    echo "Server: $serverName";
}

$date = gmdate("D, d M Y H:i:s T");
echo "Date: $date";
?>
