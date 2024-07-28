#!/usr/bin/perl
use strict;
use warnings;
use POSIX qw(strftime);

# Read form data from standard input
sub read_form_data {
    my $content_length = $ENV{'CONTENT_LENGTH'} || 0;
    if ($content_length > 0) {
        read(STDIN, my $form_data, $content_length);
        my %form;
        foreach my $pair (split /&/, $form_data) {
            my ($key, $value) = split /=/, $pair;
            $key = uri_unescape($key);
            $value = uri_unescape($value);
            push @{$form{$key}}, $value;
        }
        return %form;
    }
    return ();
}

# URL decode function
sub uri_unescape {
    my $str = shift;
    $str =~ s/%([0-9A-Fa-f]{2})/chr(hex($1))/ge;
    return $str;
}

# Get the form data
my %form = read_form_data();

# Prepare the HTML body
my $body = <<"END_HTML";
<!DOCTYPE html>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0">
<head><title>New User !</title>
<link rel="icon" type="image/x-icon" href="perl.jpg">
<style>
* {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
h1 { font-size: 3em; color: black; }
</style></head>
<body><h1>Welcome !</h1></body></html>
<br><a href="/perl/welcome.pl"><button> See your profile</button></a>
END_HTML

# Print HTTP headers
print "Content-Type: text/html\r\n";
print "Cache-Control: no-cache\r\n";
print "Content-Length: " . length($body) . "\r\n";

# Set cookies if form data is present
if (exists $form{'username'}) {
    my $user = $form{'username'}[0];
    print "Set-Cookie: user=$user\n";
    warn "found cookie username\n";
} else {
    print "Set-Cookie: user=toto\n";
}

if (exists $form{'BackGroundColor'}) {
    my $bgcolor = $form{'BackGroundColor'}[0];
    print "Set-Cookie: bgcolor=$bgcolor\n";
    warn "found cookie bgcolor\n";
} else {
    print "Set-Cookie: bgcolor=PaleGreen\n";
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
