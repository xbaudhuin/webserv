#!/usr/bin/env ruby
require 'cgi'
require 'time'

# Retrieve environment variables
cookie_string = ENV['HTTP_COOKIE'] || ''
server_name = ENV['SERVER_NAME'] || 'Unknown Server'

# Initialize CGI and HTML body
cgi = CGI.new
body = "<!DOCTYPE html>"

# Parse cookies manually
cookies = Hash[cookie_string.split(/;\s*/).map { |c| c.split('=', 2) }]

# Generate HTTP headers and HTML content
puts "Content-Type: text/html"
puts "Cache-Control: no-cache"

if cookies.key?('bgcolor') && cookies.key?('user')
  name = cookies['user']
  bgcolor = cookies['bgcolor']
  rubies = cookies['rubies'] ? cookies['rubies'].to_i : 0
  random_increment = 0
  if cookies.key?('rubies')
    random_increment = rand(1..20)
  end
  rubies += random_increment
  cookie_rubies = "Set-Cookie: rubies=#{rubies}"
  puts cookie_rubies

  body += <<~END_HTML
    <head><title>#{name}</title>
    <link rel="icon" type="image/x-icon" href="ruby.jpg">
    <style>
    * {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
    body { background-color: #{bgcolor}; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
    h1 { font-size: 3em; color: white; }
    </style></head>
    <body>
    <h1>#{name}</h1>
    <h2>Here are your rubies!</h2>
    <h3>You have #{rubies} rubies!</h3>
    <br><a href="/ruby/welcome.rb"><button>Go Back To Your Welcome Page!</button></a>
    <h3>PS: Each time you come back, your number of rubies increases by a random number between 1 and 20!</h3>
    </body></html>
  END_HTML
else
  body += <<~END_HTML
    <head><title>Unknown User</title>
    <link rel="icon" type="image/x-icon" href="ruby.jpg">
    <style>
    * {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
    body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
    h1 { font-size: 3em; color: black; }
    </style></head>
    <body>
    <h1>Oops! You're not connected yet</h1>
    <br><a href="/ruby/connectionForm.html"><button>Create a new profile</button></a>
    </body></html>
  END_HTML
end

puts "Content-Length: #{body.length}"
puts
puts body
