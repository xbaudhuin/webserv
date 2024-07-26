<?php
// Define a list of 20 jokes
error_reporting(E_ALL);
ini_set('display_errors', 1);
ini_set('log_errors', 1);
ini_set('error_log', '/tmp/php_errors.log');
$jokes = [
    "Why don't scientists trust atoms? Because they make up everything!",
    "What do you get if you cross a snowman and a vampire? Frostbite.",
    "Why did the scarecrow win an award? Because he was outstanding in his field!",
    "How does a penguin build its house? Igloos it together.",
    "Why don't programmers like nature? It has too many bugs.",
    "Why did the bicycle fall over? It was two-tired.",
    "What do you call fake spaghetti? An impasta.",
    "How does a penguin drink its coke? On the rocks.",
    "Why did the math book look sad? Because it had too many problems.",
    "What do you call cheese that isn't yours? Nacho cheese.",
    "Why don’t skeletons fight each other? They don’t have the guts.",
    "What do you call a fish with no eyes? Fsh.",
    "Why don’t some couples go to the gym? Because some relationships don’t work out.",
    "How does a farmer count his cows? With a cowculator.",
    "Why did the coffee file a police report? It got mugged.",
    "Why don’t scientists trust atoms? Because they make up everything!",
    "What did the grape do when he got stepped on? Nothing but let out a little wine.",
    "Why did the scarecrow become a successful neurosurgeon? Because he was outstanding in his field.",
    "What’s brown and sticky? A stick.",
    "Why don’t programmers like nature? It has too many bugs."
];

// Get a random joke
$random_joke = $jokes[array_rand($jokes)];

// Create a simple HTML page
$body = <<<HTML
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Random Joke Generator</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            text-align: center;
            margin-top: 50px;
            background-color: #f0f8ff;
        }
        .joke {
            font-size: 24px;
            color: #333;
        }
        a {
            text-decoration: none;
            color: #007bff;
            font-weight: bold;
        }
        a:hover {
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <h1>Here's a Random Joke for You!</h1>
    <p class="joke">{$random_joke}</p>
    <p><a href="/php/jokes.php">Get Another Joke</a></p>
</body>
</html>
HTML;

echo "Content-Type: text/html;";
echo "\r\n";
echo "Cache-Control: no-cache";
echo "\r\n";
echo "Content-Length: " . strlen($body);
echo "\r\n";
echo "\r\n";
echo $body
?>