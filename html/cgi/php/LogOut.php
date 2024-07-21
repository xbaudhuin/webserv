<?php
// Enable error reporting for debugging
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Clear cookies by setting their expiration date to a past date
setcookie('user', '', time() - 3600, '/');
setcookie('bgcolor', '', time() - 3600, '/');

// Build the HTML body
$body = "<!DOCTYPE html>
<html>
<head>
    <title>Bye Bye!</title>
    <style>
        * { font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif; }
        body { background-color: CornflowerBlue; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column; }
        h1 { font-size: 3em; color: black; }
    </style>
</head>
<body>
    <h1>See Ya!</h1>
    <br><a href=\"/php/welcome.php\"><button>Go back to main page</button></a>
</body>
</html>";

// Print the HTTP headers
header("Content-Type: text/html");
header("Content-Length: " . strlen($body));

// Print the HTML body
echo $body;

// Print server name and date if available
if (isset($_SERVER['SERVER_NAME'])) {
    $serverName = $_SERVER['SERVER_NAME'];
    echo "Server: $serverName<br>";
}

$date = gmdate("D, d M Y H:i:s T");
echo "Date: $date<br>";
?>
