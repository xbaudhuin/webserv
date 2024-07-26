#!/usr/bin/perl
use strict;
use warnings;
use POSIX qw(strftime);

# Define the logOut subroutine
sub logOut {
    # Initialize HTML body
    my $body = "<!DOCTYPE html>";
    $body .= "<html><head><title>Bye Bye!</title>";
    $body .= "<style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont,";
    $body .= "'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}";
    $body .= "body { background-color: CornflowerBlue; display: flex; justify-content: center;";
    $body .= "align-items: center; height: 100vh; margin: 0; flex-direction: column;} ";
    $body .= "h1 { font-size: 3em; color: black; } </style></head>";
    $body .= "<body><h1>See Ya!</h1>";
    $body .= "<br><a href=\"/perl/welcome.pl\"><button>Go back to main page</button></a></body></html>";

    # Print server and date info
    if (exists $ENV{'SERVER_NAME'}) {
        my $server_name = $ENV{'SERVER_NAME'};
        print "Server: $server_name\n";
    }

    my $datetimenow = strftime "%a, %d %b %Y %H:%M:%S %Z", localtime;
    print "Date: $datetimenow\r\n";

    my $expires_time = "Thu, 01 Jan 1970 00:00:10 GMT";
    print "Set-Cookie: user=blbkvvv; Expires=$expires_time;\n";
    print "Set-Cookie: bgcolor=spspspsp; Expires=$expires_time;\n";
    print "Set-Cookie: perls=0; Expires=$expires_time;\n";
    # Print the HTTP headers and body content
    print "Content-Type: text/html\n";
    print "Content-Length: " . length($body) . "\r\n\r\n";
    print "$body";
}

# Call the logOut subroutine if this script is executed directly
logOut() if __FILE__ eq $0;
