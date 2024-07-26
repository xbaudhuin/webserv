require 'time'

def log_out
  # Prepare the HTML body
  body = <<~HTML
    <!DOCTYPE html>
    <html><head><title>Bye Bye!</title>
    <style>* {
    font-family: system-ui, -apple-system, BlinkMacSystemFont,
    'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans',
    'Helvetica Neue', sans-serif;} body { background-color:
    CornflowerBlue; display: flex; justify-content: center;
    align-items: center; height: 100vh; margin: 0; flex-direction:
    column;} h1 { font-size: 3em; color: black; } </style></head>
    <body><h1>See Ya!</h1>
    <br><a href="/ruby/welcome.rb"><button>Go back to main page</button></a></body></html>
  HTML

  # Print HTTP headers
  puts "Content-Type: text/html"
  puts "Set-Cookie: user=blbkvvv; Expires=Thu, 01-Jan-1970 00:00:10 GMT"
  puts "Set-Cookie: bgcolor=spspspsp; Expires=Thu, 01-Jan-1970 00:00:10 GMT"

  # Print server name if available
  if ENV['SERVER_NAME']
    server_name = ENV['SERVER_NAME']
    puts "Server: #{server_name}"
  end

  # Print current date
  date = Time.now.strftime("%a, %d %b %Y %H:%M:%S %Z")
  puts "Date: #{date}"

  # Print content length and body
  puts "Content-Length: #{body.bytesize}\r\n"
  puts "\r\n"
  puts body
end

if __FILE__ == $0
  log_out
end
