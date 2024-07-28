require 'cgi'
require 'time'

# Retrieve cookies from the environment
cookie_string = ENV['HTTP_COOKIE']

# Initialize the body of the HTML
body = "<!DOCTYPE html>"

if cookie_string
  cookies = CGI::Cookie.parse(cookie_string)

  if cookies['bgcolor'] && cookies['user']
    name = cookies['user'].first
    bgcolor = cookies['bgcolor'].first

    body += "<head><title>#{name}</title>"
    body += "<link rel=\"icon\" type=\"image/x-icon\" href=\"ruby.jpg\">"
    body += "<style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont,"
    body += "'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}"
    body += "body { background-color: #{bgcolor}; display: flex; justify-content: center; align-items: center; height: 100vh;"
    body += "margin: 0; flex-direction: column;} h1 { font-size: 3em; color: white; } </style> </head>"
    body += "<body><h1>#{name}</h1><h2>Welcome to your panel!</h2></body></html>"
    body += "<br><a href=\"/ruby/ColorModifier.html\"><button> Change Your Profile Color</button></a>"
    body += "<br><a href=\"/ruby/rubies.rb\"><button> Go check the number of rubies you own! </button><a>"
    body += "<br><a href=\"/ruby/LogOut.rb\"><button> Log Out </button></a>"
  else
    body += "<head><title>Unknown User</title>"
    body += "<link rel=\"icon\" type=\"image/x-icon\" href=\"ruby.jpg\">"
    body += "<style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont,"
    body += "'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}"
    body += "body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh;"
    body += "margin: 0; flex-direction: column;} h1 { font-size: 3em; color: black; } </style> </head>"
    body += "<body><h1>Oops! You're not connected yet</h1></body></html>"
    body += "<br><a href=\"/ruby/connectionForm.html\"><button> Create a new profile</button></a>"
  end
else
  body += "<head><title>Unknown User</title>"
  body += "<link rel=\"icon\" type=\"image/x-icon\" href=\"ruby.jpg\">"
  body += "<style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont,"
  body += "'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}"
  body += "body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh;"
  body += "margin: 0; flex-direction: column;} h1 { font-size: 3em; color: black; } </style> </head>"
  body += "<body><h1>Oops! You're not connected yet</h1></body></html>"
  body += "<br><a href=\"/ruby/connectionForm.html\"><button> Create a new profile</button></a>"
end

# Print server name if available
if ENV['SERVER_NAME']
  puts "Server: #{ENV['SERVER_NAME']}"
end

# Print the current date
date = Time.now.strftime("%a, %d %b %Y %H:%M:%S %Z")
puts "Date: #{date}"

# Print HTTP headers
puts "Content-Type: text/html"
puts "Content-Length: #{body.bytesize}\r\n"
puts "\r\n"

# Print the body of the response
puts body
