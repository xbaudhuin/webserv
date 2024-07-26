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
<h2>Welcome to your panel !</h2>
<br><a href="/perl/ColorModifier.html"><button> Change Your Profile Color</button></a>
<br><a href="/perl/perls.pl"><button> Go check the number of perls you have there! </button><a>
<br><a href="/perl/LogOut.pl"><button> Log Out </button></a>
</body></html>
END_HTML
} else {
    $body .= <<"END_HTML";
<head><title> Unknown User</title>
<style>
* {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
h1 { font-size: 3em; color: black; }
</style></head>
<body>
<h1>Oops ! You're not connected yet</h1>
<br><a href="/perl/connectionForm.html"><button> Create a new profile</button></a>
</body></html>
END_HTML
}

# Print server and date info
print "Server: $server_name\n";
print "Date: $date\n";

# Print the HTTP headers and body content
print "Content-Type: text/html\n";
print "Content-Length: " . length($body) . "\r\n\r\n";
print $body;
