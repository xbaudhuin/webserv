import os
import datetime
import sys
import urllib.parse

def read_form_data():
    # Read the content length from environment variable
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    
    if content_length > 0:
        form_data = sys.stdin.read(content_length)
        return urllib.parse.parse_qs(form_data)
    return {}

# Prepare the HTML body
body = """<!DOCTYPE html>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width, initial-scale=1.0">
<head><title> New User !</title>
<style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
h1 { font-size: 3em; color: black; } </style> </head>
<body><h1>Welcome !</h1></body></html>
<br><a href="/python/welcome.py"><button> See your profile</button></a>"""

# Get the form data
form = read_form_data()

# Print HTTP headers
print("Content-Type: text/html")
print("Cache-Control: no-cache")
print("Content-Length:", len(body))

# Set cookies if form data is present
if 'username' in form:
    user = form['username'][0]
    print(f"Set-Cookie: user={user}")
    sys.stderr.write("found cookie username\n")
else:
    print("Set-Cookie: user=toto")

if 'BackGroundColor' in form:
    bgcolor = form['BackGroundColor'][0]
    print(f"Set-Cookie: bgcolor={bgcolor}")
    sys.stderr.write("found cookie bgcolor\n")
else:
    print("Set-Cookie: bgcolor=PaleGreen")

# Print the HTML body
print()  # Ensure a blank line before the body
print(body)

# Print server name and date if available
if "SERVER_NAME" in os.environ:
    serverName = os.environ.get('SERVER_NAME')
    print(f"Server: {serverName}")

date = datetime.datetime.now().astimezone().strftime("%a, %d %b %G %T %Z")
print(f"Date : {date}")
