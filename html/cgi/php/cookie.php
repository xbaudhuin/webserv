<?php
// Enable error reporting for debugging
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Set cookies if form data is present
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $username = isset($_POST['username']) ? htmlspecialchars($_POST['username']) : null;
    $bgcolor = isset($_POST['BackGroundColor']) ? htmlspecialchars($_POST['BackGroundColor']) : null;

    if ($username) {
        setcookie('user', $username, time() + (86400 * 30), '/'); // Set cookie for 30 days
    }
    if ($bgcolor) {
        setcookie('bgcolor', $bgcolor, time() + (86400 * 30), '/'); // Set cookie for 30 days
    }
}

// Retrieve cookie values
$username = isset($_COOKIE['user']) ? htmlspecialchars($_COOKIE['user']) : null;
$bgcolor = isset($_COOKIE['bgcolor']) ? htmlspecialchars($_COOKIE['bgcolor']) : null;

// Build the HTML body
$body = "<!DOCTYPE html>
<html>
<head>
    <title>New User!</title>
    <style>
        * { font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif; }
        body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column; }
        h1 { font-size: 3em; color: black; }
    </style>
</head>
<body>
    <h1>Welcome!</h1>
    <br><a href=\"/php/welcome.php\"><button>See your profile</button></a>
</body>
</html>";

// Print the HTTP headers
header("Content-Type: text/html");
header("Content-Length: " . strlen($body));

// Output the body content
echo $body;

// Print server name and date if available
if (isset($_SERVER['SERVER_NAME'])) {
    $serverName = $_SERVER['SERVER_NAME'];
    echo "Server: $serverName<br>";
}

$timezone = new DateTimeZone('Europe/Berlin'); // CEST timezone

// Create a DateTime object with the current date and time
$datenow = new DateTime('now', $timezone);

// Format the date to the desired format
$date = $datenow->format('D, d M Y H:i:s T');
echo "Date: $date<br>";
?>
