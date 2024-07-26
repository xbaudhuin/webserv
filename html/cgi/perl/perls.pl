#!/usr/bin/perl
use strict;
use warnings;
use POSIX qw(strftime);

# Retrieve environment variables
my $cookie_string = $ENV{'HTTP_COOKIE'} || '';
my $server_name = $ENV{'SERVER_NAME'} || 'Unknown Server';

# Format date to match Python output
my $date = strftime "%a, %d %b %Y %T %Z", localtime;

# Initialize HTML body
my $body = "<!DOCTYPE html>";

# Parse cookies manually
my %cookies = map { split /=/, $_, 2 } split /;\s*/, $cookie_string;

# Initialize cookie values
# my $username = $cookies{'username'};
# my $bgcolor = $cookies{'bgcolor'};
my $perls = $cookies{'perls'} // -1;  # Default to 0 if the perls cookie doesn't exist

# Increment perls cookie value
$perls++;

# Set expiration date for the perls cookie
my $expires = strftime("%a, %d-%b-%Y %H:%M:%S GMT", localtime(time + 24 * 60 * 60)); # Expires in 1 day
my $cookie_perls = "Set-Cookie: perls=$perls; Expires=$expires";

# Generate the HTML content
if (exists $cookies{'bgcolor'} && exists $cookies{'user'}) {
    my $name = $cookies{'user'};
    my $bgcolor = $cookies{'bgcolor'};
    $body .= <<"END_HTML";
<head><title>$name</title>
<style>
* {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
body { background-color: $bgcolor; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
h1 { font-size: 3em; color: white; }
</style></head>
<body>
<h1>$name</h1>
<h2>Here are your perls!</h2>
<h3>You have $perls perls!</h3>
<br><a href="/perl/welcome.pl"><button>Go Back To Your Welcome Page!</button></a>
<h3>PS: Each time you come back, your number of perls increases</h3>
</body></html>
END_HTML
} else {
    $body .= <<"END_HTML";
<head><title>Unknown User</title>
<style>
* {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
h1 { font-size: 3em; color: black; }
</style></head>
<body>
<h1>Oops! You're not connected yet</h1>
<br><a href="/perl/connectionForm.html"><button>Create a new profile</button></a>
</body></html>
END_HTML
}

# Print the HTTP headers and body content
print "Content-Type: text/html\r\n";
print "Cache-Control: no-cache\r\n";
print "Content-Length: " . length($body) . "\r\n";
print "$cookie_perls\r\n";
print "\r\n";  # Ensure a blank line before the body
print $body;
