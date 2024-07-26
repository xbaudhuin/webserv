require 'cgi'
require 'time'

# Function to read form data from stdin
def read_form_data
  content_length = ENV['CONTENT_LENGTH'].to_i
  form_data = {}

  if content_length > 0
    form_data_raw = $stdin.read(content_length)
    form_data = CGI.parse(form_data_raw)
  end

  form_data
end

# Prepare the HTML body
body = <<~HTML
<!DOCTYPE html>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0">
<head><title> New User !</title>
<style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
h1 { font-size: 3em; color: black; } </style> </head>
<body><h1>Welcome !</h1></body></html>
<br><a href="/ruby/welcome.rb"><button> See your profile</button></a>
HTML

# Get the form data
form = read_form_data

# Print HTTP headers
puts "Content-Type: text/html"
puts "Cache-Control: no-cache"
puts "Content-Length: #{body.bytesize}\r\n"

# Set cookies if form data is present
if form['username'] && !form['username'].empty?
  user = form['username'].first
  puts "Set-Cookie: user=#{user}"
  $stderr.puts "found cookie username"
else
  puts "Set-Cookie: user=toto"
end

if form['BackGroundColor'] && !form['BackGroundColor'].empty?
  bgcolor = form['BackGroundColor'].first
  puts "Set-Cookie: bgcolor=#{bgcolor}"
  $stderr.puts "found cookie bgcolor"
else
  puts "Set-Cookie: bgcolor=PaleGreen"
end

# Print the HTML body
puts "\r\n"  # Ensure a blank line before the body
puts body

# Print server name and date if available
if ENV['SERVER_NAME']
  server_name = ENV['SERVER_NAME']
  puts "Server: #{server_name}"
end

date = Time.now.utc.strftime("%a, %d %b %Y %H:%M:%S %Z")
puts "Date: #{date}"
