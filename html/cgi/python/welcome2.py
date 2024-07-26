import os
import datetime
from http.cookies import SimpleCookie


def define_html(cookie, body):
    name = cookie['user'].value
    bgcolor = cookie['bgcolor'].value
    body.append("<head><title>")
    body.append(name)
    body.append("</title><style>* {font-family: system-ui,")
    body.append(" -apple-system, BlinkMac")
    body.append("SystemFont,'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, ")
    body.append("'Open Sans', 'Helvetica Neue', sans-serif;}")
    body.append("body { background-color: ")
    body.append(bgcolor)
    body.append("; display: flex; justify-content: center; ")
    body.append("align-items: center; height: 100vh;")
    body.append("margin: 0; flex-direction: column;} h1 ")
    body.append("{ font-size: 3em; color: white;} </style> </head>")
    body.append("<body><h1>")
    body.append(name)
    body.append("</h1><h2>Welcome to your panel !</h2></body></html>")
    body.append("<br><a href=\"/python/ColorModifier.html\">")
    body.append("<button> Change Your Profile Color</button></a><br>")
    body.append("<a href=\"/python/LogOut.py\"><button> Log Out </button></a>")
    return


def define_not_found(body):
    body.append("<head><title> Unknown User")
    body.append("</title><style>* {font-family: system-ui,")
    body.append(" -apple-system, BlinkMacSystemFont,")
    body.append("'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, ")
    body.append("'Open Sans', 'Helvetica Neue', sans-serif;}")
    body.append("body { background-color: ")
    body.append("LightGoldenRodYellow")
    body.append("; display: flex; justify-content: center; ")
    body.append("align-items: center; height: 100vh;")
    body.append("margin: 0; flex-direction: column;} ")
    body.append("h1 { font-size: 3em; color: black;")
    body.append("} </style> </head>")
    body.append("<body><h1>")
    body.append("Oops ! You're not connected yet")
    body.append("</h1></body></html>")
    body.append("<br><a href=\"/python/connectionForm.html\">")
    body.append("<button> Create a new profile</button></a>")
    return


def main():
    cookieString = os.environ.get('HTTP_COOKIE')
    body = ["<!DOCTYPE html>"]
    if cookieString:
        cookie = SimpleCookie()
        cookie.load(cookieString)
        if 'bgcolor' in cookie and 'user' in cookie:
            define_html(cookie, body)
        else:
            define_not_found(body)
    else:
        define_not_found(body)
    print("HTTP/1.1 200 OK\r",)
    if "SERVER_NAME" in os.environ:
        serverName = os.environ.get('SERVER_NAME')
        print(f"Server: {serverName}")
    date = datetime.datetime.now().astimezone().strftime("%a, %d %b %G %T %Z")
    body_str = "".join(body)
    print(f"Date: {date}")
    print("Content-Type: Text/html")
    print(f"Content-Length: {len(body_str)}\r\n")
    print(f"{body_str}")
    return


if __name__ == "__main__":
    main()
