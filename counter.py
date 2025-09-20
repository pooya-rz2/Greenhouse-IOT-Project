import http.server
import socketserver
import socket
import requests

PORT = 8080
PUSHOVER_USER_KEY = 'ueutrujnsnc6tbsbhdy6n24fb9pbxc'
PUSHOVER_API_TOKEN = 'a7i4kbgdp515kjbrxmsdeq6dryoxom'
notification_threshold = 5
request_count = 0

# Function to get the local IP address
def get_local_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # Connect to an external host
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    except Exception:
        ip = '127.0.0.1'
    finally:
        s.close()
    return ip

class MyHandler(http.server.SimpleHTTPRequestHandler):
    def do_POST(self):
        global request_count

        content_length = int(self.headers['Content-Length'])  # Get the size of data
        post_data = self.rfile.read(content_length)  # Get the data itself
        request_count += 1
        print(f"Received POST request:\nData: {post_data.decode('utf-8')}\n         --- counter: {request_count} ---")

        if request_count % notification_threshold == 0 and not request_count == 0:
            send_pushover_notification(request_count)

        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(b"Data received!")

def send_pushover_notification(request_count):
    message = f"The server has received {request_count} clicks."
    payload = {
        'token': PUSHOVER_API_TOKEN,
        'user': PUSHOVER_USER_KEY,
        'message': message,
    }
    try:
        response = requests.post("https://api.pushover.net/1/messages.json", data=payload)
        if response.status_code == 200:
            print("Pushover notification sent successfully.")
        else:
            print(f"Failed to send Pushover notification: {response.status_code}")
    except Exception as e:
        print(f"An error occurred while sending Pushover notification: {e}")

# Get the server's local IP address
server_ip = get_local_ip()
print(f"Server IP address: {server_ip}")

with socketserver.TCPServer((server_ip, PORT), MyHandler) as httpd:
    print(f"Serving at port {PORT}")
    httpd.serve_forever()