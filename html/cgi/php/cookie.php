<?php
function read_form_data() {
    $content_length = getenv('CONTENT_LENGTH');
    $form_data = [];

    if ($content_length > 0) {
        $form_data_raw = fread(fopen('php://input', 'r'), $content_length);
        parse_str($form_data_raw, $form_data);
    }
    
    return $form_data;
}

$body = <<<HTML
<!DOCTYPE html>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0">
<head><title> New User !</title>
<style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
h1 { font-size: 3em; color: black; } </style> </head>
<body><h1>Welcome !</h1></body></html>
<br><a href="/python/welcome.py"><button> See your profile</button></a>
HTML;

$form = read_form_data();

// Collect headers in an array
$headers = [];
$headers[] = "Content-Type: text/html";
$headers[] = "Cache-Control: no-cache";
$headers[] = "Content-Length: " . strlen($body);

if (array_key_exists('username', $form)) {
    $user = $form['username'];
    $headers[] = "Set-Cookie: user=$user";
    error_log("found cookie username\n");
} else {
    $headers[] = "Set-Cookie: user=toto";
}

if (array_key_exists('BackGroundColor', $form)) {
    $bgcolor = $form['BackGroundColor'];
    $headers[] = "Set-Cookie: bgcolor=$bgcolor";
    error_log("found cookie bgcolor\n");
} else {
    $headers[] = "Set-Cookie: bgcolor=PaleGreen";
}

// Print headers
foreach ($headers as $header) {
    echo "$header\r\n";
}

// Ensure a blank line before the body
echo "\r\n";

// Print the HTML body
echo $body;

$serverName = getenv('SERVER_NAME');
if ($serverName) {
    echo "Server: $serverName\r\n";
}

$date = (new DateTime('now', new DateTimeZone('UTC')))->format('D, d M Y H:i:s T');
echo "Date: $date\r\n";
?>
