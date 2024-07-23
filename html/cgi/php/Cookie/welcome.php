<?php
// Enable error reporting for debugging
error_reporting(E_ALL);
ini_set('display_errors', 1);

// Function to print cookies (equivalent to the print statements in the Python script)
function print_cookies($cookies) {
    foreach ($cookies as $key => $value) {
        echo "$key is $value<br>";
    }
}

// Retrieve cookies and server name
$cookies = $_COOKIE;
$serverName = $_SERVER['SERVER_NAME'] ?? '';

// Initialize the HTML body
$body = "<!DOCTYPE html>";

if (!empty($cookies)) {
    print_cookies($cookies);

    if (isset($cookies['bgcolor']) && isset($cookies['user'])) {
        $name = htmlspecialchars($cookies['user']);
        $bgcolor = htmlspecialchars($cookies['bgcolor']);
        $body .= "<head><title>";
        $body .= $name;
        $body .= "</title><style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont,"
            . "'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}"
            . "body { background-color: "
            . $bgcolor
            . "; display: flex; justify-content: center; align-items: center; height: 100vh;"
            . "margin: 0; flex-direction: column;} h1 { font-size: 3em; color: white;}"
            . "</style></head><body><h1>"
            . $name
            . "</h1><h2>Welcome to your panel!</h2><br><a href=\"/php/ColorModifier.html\"><button> Change Your Profile Color</button></a>"
            . "<br><a href=\"/php/LogOut.php\"><button> Log Out </button></a></body></html>";
    } else {
        $body .= "<head><title> Unknown User</title><style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont,"
            . "'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}"
            . "body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh;"
            . "margin: 0; flex-direction: column;} h1 { font-size: 3em; color: black;}"
            . "</style></head><body><h1>Oops! You're not connected yet</h1>"
            . "<br><a href=\"/php/connectionForm.html\"><button> Create a new profile</button></a></body></html>";
    }
} else {
    $body .= "<head><title> Unknown User</title><style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont,"
        . "'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}"
        . "body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh;"
        . "margin: 0; flex-direction: column;} h1 { font-size: 3em; color: black;}"
        . "</style></head><body><h1>Oops! You're not connected yet</h1>"
        . "<br><a href=\"/php/connectionForm.html\"><button> Create a new profile</button></a></body></html>";
}

// Print the HTTP headers
header("Content-Type: text/html");
header("Content-Length: " . strlen($body));

// Print the server name and the HTML body
echo "Server: $serverName<br>";
echo $body;
?>
