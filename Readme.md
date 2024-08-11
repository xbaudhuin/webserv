# Webserv Usage Guide

**Version:** 1.0  
**Date:** August 2024

## Table of Contents
- [Name](#name)
- [Synopsis](#synopsis)
- [Description](#description)
- [Directives](#directives)
- [Server Block Directives](#server-block-directives)
- [Location Block Directives](#location-block-directives)
- [Examples](#examples)
- [Author](#author)

## Name
`./webserv [CONF]`

## Synopsis
The program only accepts `.conf` files as input, regardless of their location:

```bash
{NAME}.conf  # Configuration file for webserv
```

## Description
The `.conf` file is used to configure the `webserv`. This file contains all the necessary parameters to configure the web server. **Anything** that isn't a directive will be considered an **error** and therefore be **refused**.

## Directives
Each directive **must** end with a `;` and each server block is **delimited** by `{}` like this:

```bash
server { ... }
```

## Server Block Directives

### `listen { [PORT]/[HOST]/[HOST:PORT] };`
Defines the port on which the server listens for incoming connections and/or to which host the server is based. The host is IPv4 compliant **only**. If **no** listen directive is **set**, the server listens to port `4243` and host `localhost` (127.0.0.1) by default.

### `server_name {PARAM}... ;`
Specifies the **server name(s)** for the server. If **no** `server_name` is set, the name of the server is `webserv`.

### `root {PATH} ;`
Specifies the **root directory** for the server. The current directory is used by default.

### `error_page {ERROR_CODE}... {URI} ;`
Specifies the **error page** for **error codes** for the server.

### `client_max_body_size {VALUE}[SIZE] ;`
Specifies the **body size limit** for the server. It's interpreted in bytes, but adding `[k/K]`, `[m/M]`, or `[g/G]` at the end of your limit will be interpreted and converted to bytes. **There is no body size limit by default**.

### `location [OPTION '='] {FILE | DIRECTORY} { DIRECTIVES }`
Specifies a **location** in which the `=` option can be used to specify an **exact match**, and it must be followed by a **FILE** or **DIRECTORY**. This is where the **URI** will be looked for.

## Location Block Directives

### `root {PATH} ;`
Specifies the **root directory** for the location, it **replaces** the server root for this **specific** location. The root of the server is used by default.

### `alias {PATH} ;`
Specifies the **alias** for the location, where files/directories will be looked for instead of the **FILE** or **DIRECTORY** that was passed as a parameter for the server block. Using **root** instead of **alias** is preferred if the alias ends with the parameter passed to the **location** block.

### `client_max_body_size {VALUE}[SIZE] ;`
Specifies the body size limit for the location. It's interpreted in bytes, but adding `[k/K]`, `[m/M]`, or `[g/G]` at the end of your limit will be interpreted and converted to bytes. **There is no body size limit by default**.

### `autoindex {"on" | "off"} ;`
Specifies whether the **directory listing** is **on** or **off**. **It's off by default**.

### `index {FILE}... ;`
Specifies (a) file(s) to look for in particular in said location. If no index is set, an **index.html** file will be looked for.

### `return {CODE} {ADDRESS} ;`
**Redirects** to a particular address.

### `path_info {"on" | "off"}`
Indicates if we split the received URI to get the path info from it.

### `cgi {EXTENSION} {FILE}`
Specifies a CGI **to execute**. The supported extensions are: `[.py]`, `[.rb]`, `[.php]`, `[.pl]`. It also enables the execution of other CGI of the **same** extension.

### `upload_location {PATH} ;`
Specifies **where** the upload of a file will be done. If **no** upload path is set and the **POST** method is **allowed**, the directory of the location will be **used**.

### `set_method { "POST" | "GET" | "DELETE" } { "on" "off" }`
Specifies whether the method(s) asked for are **on** or not. **Every method** is on if no method is set otherwise.

## Examples
The following is an example configuration:

```bash
server {
    server_name webserv;
    listen 127.0.0.1:4243;
    root ./html;
    error_page 404 403 400 /418.html;
    client_max_body_size 1g;

    location / {
        index site.html;
        client_max_body_size 100k;
    }

    location = /418.html {
    }

    location /coucou {
        alias ./tester/cgi/;
    }

    location /cgi/python/ {            
        cgi .py welcome.py;        
        upload_location /cookies/;
        path_info on;
        set_method POST DELETE GET on;
    }
}
```

## Author
Written by **thole**.
