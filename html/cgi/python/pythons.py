import os
import http.cookies
import random
import time
from datetime import timedelta

# Retrieve environment variables
cookie_string = os.environ.get('HTTP_COOKIE', '')
server_name = os.environ.get('SERVER_NAME', 'Unknown Server')

# Parse cookies
cookies = http.cookies.SimpleCookie(cookie_string)

# Initialize HTML body
body = "<!DOCTYPE html>"

# Function to generate HTTP headers and body
def generate_response(body, additional_headers=None):
    headers = [
        "Content-Type: text/html",
        "Cache-Control: no-cache"
    ]
    if additional_headers:
        headers.extend(additional_headers)
    headers.append(f"Content-Length: {len(body)}")
    print("\r\n".join(headers))
    print("\r\n")
    print(body)

def pick_random_image() -> int:
    return random.randint(0, 1)

# Generate the HTML content
set_cookie_header = None
if 'bgcolor' in cookies and 'user' in cookies:
    name = cookies['user'].value
    bgcolor = cookies['bgcolor'].value

    python_img_val = cookies.get('python_img')
    if python_img_val is None:
        python_img_val = pick_random_image()
        set_cookie_header = f"Set-Cookie: python_img={python_img_val}"
    else:
        python_img_val = int(python_img_val.value)
    
    python_img = "/python/python1.jpg" if python_img_val == 0 else "/python/python2.jpg"
    body += f"""
<head><title>{name}</title>
<link rel="icon" type="image/x-icon" href="py.jpg">
<style>
* {{font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}}
body {{ background-color: {bgcolor}; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}}
h1 {{ font-size: 3em; color: white; }}
</style></head>
<body>
<h1>{name}</h1>
<h2>Here is your python!</h2>
<img src="{python_img}" alt="your_own_python_img">
<br><a href="/python/welcome.py"><button>Go Back To Your Welcome Page!</button></a>
<h3>PS: It should always be this one as long as you don't log out!</h3>
</body></html>
"""
    generate_response(body, [set_cookie_header] if set_cookie_header else None)
else:
    body += """
<head><title>Unknown User</title>
<style>
* {font-family: system-ui, -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}
body { background-color: LightGoldenRodYellow; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column;}
h1 { font-size: 3em; color: black; }
</style></head>
<body>
<h1>Oops ! You're not connected yet</h1>
<br><a href="/python/connectionForm.html"><button>Create a new profile</button></a>
</body></html>
"""
    generate_response(body)
