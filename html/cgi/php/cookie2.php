<?php
// Set a test cookie
setcookie('test_cookie', 'test_value', time() + (86400 * 30), '/');

// Check if the cookie is set
if (isset($_COOKIE['test_cookie'])) {
    echo "Cookie 'test_cookie' is set with value: " . $_COOKIE['test_cookie'];
} else {
    echo "Cookie 'test_cookie' is not set.";
}
?>
