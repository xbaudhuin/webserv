# Use the official Nginx base image
FROM nginx:latest

RUN apt-get update && apt-get upgrade -y && apt-get install vim -y
# Copy your custom nginx.conf to the container

RUN mkdir -p /etc/share/nginx/post

COPY nginx.conf /etc/nginx/nginx.conf

# Copy your website content to the default Nginx public directory
COPY index.html /usr/share/nginx/html/index.html

# Expose port 80 to the host
EXPOSE 4247

# Start Nginx when the container launches
CMD ["nginx", "-g", "daemon off;"]
