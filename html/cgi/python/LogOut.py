import os
import datetime


def logOut():
    print("Content-type: text/html")
    body = "<!DOCTYPE html>"
    body += "<html><head><title>Bye Bye!</title>"
    body += "</title><link rel=\"icon\" type=\"image/x-icon\" href=\"py.jpg\">"
    body += "<style>* "
    body += "{font-family: system-ui, -apple-system, BlinkMacSystemFont,"
    body += "'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, 'Open Sans', "
    body += "'Helvetica Neue', sans-serif;} body { background-color: "
    body += "CornflowerBlue; display: flex; justify-content: center; "
    body += "align-items: center; height: 100vh; margin: 0; flex-direction: "
    body += "column;} h1 { font-size: 3em; color: black; } </style></head>"
    body += "<body><h1>See Ya!</h1>"
    body += "<br><a href=\"/python/welcome.py\"><button>Go back to main page"
    body += "</button></a></body></html>"
    if "SERVER_NAME" in os.environ:
        serverName = os.environ.get('SERVER_NAME')
        print(f"Server: {serverName}")
    datetimenow = datetime.datetime.now()
    date = datetimenow.astimezone().strftime("%a, %d %b %Y %H:%M:%S %Z")
    print(f"Date: {date}")
    time = "Thu, 01-Jan-1970 00:00:10 GMT"
    print(f"Set-Cookie: user=blbkvvv;Expires={time};")
    print(f"Set-Cookie: bgcolor=spspspsp;Expires={time};")
    print(f"Set-Cookie: python_img=sasvcb;Expires={time};")
    print(f"Content-Length: {len(body)}\r\n")
    print(f"{body}")


if __name__ == "__main__":
    logOut()
