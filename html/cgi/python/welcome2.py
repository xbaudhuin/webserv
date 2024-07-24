import os
import datetime
from http.cookies import SimpleCookie


def get_cookie_value(cookie_name):
    """Retrieve the value of a specified cookie
 from the HTTP_COOKIE environment variable."""
    cookie_string = os.environ.get('HTTP_COOKIE')
    if cookie_string:
        cookie = SimpleCookie()
        cookie.load(cookie_string)
        if cookie_name in cookie:
            return cookie[cookie_name].value
    return None


def generate_html_body(name=None, bgcolor=None):
    """Generate the HTML body based on the user’s name and background color."""
    body = "<!DOCTYPE html>"
    body += "<html><head><title>"
    body += name if name else "Unknown User"
    body += "</title><style>* {font-family: system-ui, -apple-system, BlinkMacSystemFont,"
    body += "'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', 'Helvetica Neue', sans-serif;}"
    body += "body { background-color: "
    body += bgcolor if bgcolor else "LightGoldenRodYellow"
    body += "; display: flex; justify-content: center; align-items: center; height: 100vh;"
    body += "margin: 0; flex-direction: column;} h1 { font-size: 3em; color: "
    body += "white" if bgcolor else "black"
    body += "; } </style></head>"
    body += "<body><h1>"
    body += name if name else "Oops ! You're not connected yet"
    body += "</h1><h2>Welcome to your panel!</h2></body></html>"
    if name:
        body += "<br><a href=\"/python/ColorModifier.html\"><button>Change Your Profile Color</button></a>"
        body += "<br><a href=\"/python/LogOut.py\"><button>Log Out</button></a>"
    else:
        body += "<br><a href=\"/python/connectionForm.html\"><button>Create a new profile</button></a>"
    return body


def print_server_info():
    """Print server information if available."""
    if "SERVER_NAME" in os.environ:
        server_name = os.environ.get('SERVER_NAME')
        print(f"Server: {server_name}")


def print_date():
    """Print the current date and time."""
    datetimenow = datetime.datetime.now()
    now = datetimenow.astimezone().strftime("%a, %d %b %Y %H:%M:%S %Z")
    print(f"Date: {now}")


def main():
    """Main function to handle CGI request and response."""
    # Get cookie values
    name = get_cookie_value('user')
    bgcolor = get_cookie_value('bgcolor')

    # Generate HTML body
    body = generate_html_body(name, bgcolor)

    # Print headers and content
    print("Content-Type: text/html")
    print(f"Content-Length: {len(body)}\r\n")
    print(body)

    # Print additional information
    print_server_info()
    print_date()


if __name__ == "__main__":
    main()
