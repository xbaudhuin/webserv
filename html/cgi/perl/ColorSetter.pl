#!/usr/bin/perl
use strict;
use warnings;
use POSIX qw(strftime);

# Function to decode URL-encoded form data
sub uri_unescape {
    my $str = shift;
    $str =~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/ge;
    return $str;
}

# Function to read form data from QUERY_STRING
sub read_form_data {
    my $query_string = $ENV{'QUERY_STRING'} || '';
    my %form;
    foreach my $pair (split /&/, $query_string) {
        my ($key, $value) = split /=/, $pair, 2; # Split only once to handle values with '='
        $key = uri_unescape($key);
        $value = uri_unescape($value);
        push @{$form{$key}}, $value;
    }
    return %form;
}

# Read form data
my %form = read_form_data();

# Default background color
my $bgcolor = "Azure";

# Update background color if provided in form data
if (exists $form{'BackGroundColor'}) {
    $bgcolor = $form{'BackGroundColor'}[0];
}

# Prepare the HTML body with dynamic background color
my $body = <<"END_HTML";
<!DOCTYPE html><html>
<head>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>BackGround Color Is Modified</title>
<style>
* {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
body { background-color: Azure; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
h1 { font-size: 3em; color: black; }
</style></head>
<body><h1>Color Successfully Modified</h1></body></html>
<br><a href="/perl/welcome.pl"><button> Go back to your profile</button></a></html>
END_HTML

# Print HTTP headers
print "Content-Type: text/html\r\n";
print "Cache-Control: no-cache\r\n";
print "Content-Length: " . length($body) . "\r\n";

# Set cookies if form data is present
if (exists $form{'BackGroundColor'}) {
    my $bgcolor = $form{'BackGroundColor'}[0];
    print "Set-Cookie: bgcolor=$bgcolor\r\n";
}

# Print server name and date if available
if (exists $ENV{'SERVER_NAME'}) {
    my $server_name = $ENV{'SERVER_NAME'};
    print "Server: $server_name\r\n";
}

my $date = strftime "%a, %d %b %Y %H:%M:%S %Z", localtime;
print "Date: $date\r\n";

# Print the HTML body
print "\r\n";  # Ensure a blank line before the body
print $body;
