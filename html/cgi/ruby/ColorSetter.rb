require 'cgi'
require 'time'

# Create a new CGI object to handle form data
cgi = CGI.new

# Retrieve the form data
form = cgi.params

# Prepare the HTML body
body = <<~HTML
  <head><title>BackGround Color Is Modified</title>
  <style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont,
  'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
  body { background-color: Azure; display: flex; justify-content: center; align-items: center; height: 100vh;
  margin: 0; flex-direction: column;} h1 { font-size: 3em; color: black; } </style> </head>
  <body><h1>Color Successfully Modified</h1></body></html>
  <br><a href="/ruby/welcome.rb"><button>Go back to your profile</button></a>
HTML

# Print HTTP headers
puts "Content-Type: text/html"

# Set cookies if form data is present
if form['BackGroundColor'] && !form['BackGroundColor'].empty?
  bgcolor = form['BackGroundColor'].first
  puts "Set-Cookie: bgcolor=#{bgcolor}"
end

# Print server name if available
if ENV['SERVER_NAME']
  server_name = ENV['SERVER_NAME']
  puts "Server: #{server_name}"
end

# Print current date
date = Time.now.utc.strftime("%a, %d %b %Y %H:%M:%S %Z")
puts "Date: #{date}"

# Print content length and body
puts "Content-Length: #{body.bytesize}\r\n"
puts body
