from os import environ
import time


def find_cookie(_to_find):
    """function to find cookie"""
    if 'HTTP_COOKIE' in environ:
        splitted = environ['HTTP_COOKIE']
        cookies = splitted.split("; ")
        for cookie in cookies:
            (key, value) = cookie.split('=')
            if key.lower() == _to_find.lower():
                return value
    return ''


def main():
    """main cgi"""
    print("Content-Type: text/html;charset=utf-8")
    if not find_cookie("first"):
        print(f"Set-Cookie: First={time.asctime(time.localtime())}")
    print(f"Set-Cookie: Last={time.asctime(time.localtime())}")
    try:
        i = int(find_cookie('Count')) + 1
    except Exception:
        i = 0
    s = str(i)
    print(f"Set-Cookie: Count={s}")
    print()
    print("<!DOCTYPE html>")
    print("<html><body><table border=\"0\">")
    if find_cookie('first'):
        f_visit = "<tr><td>Time First Visit</td><td>:</td><td>"
        print(f"{f_visit}{find_cookie('first')}</tr>")
    if find_cookie('last'):
        f_visit = "<tr><td>Time Last Visit</td><td>:</td><td>"
        print(f"{f_visit}{find_cookie('last')}</tr>")
    try:
        i = int(find_cookie('Count')) + 1
    except Exception:
        i = 0
    s = str(i)
    print('<tr><td>Counter</td><td>:</td><td>' + s + '</tr>')
    print('</table></body></html>')


if __name__ == "__main__":
    try:
        main()
    except Exception:
        None
