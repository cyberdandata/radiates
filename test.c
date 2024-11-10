import json
from http.server import BaseHTTPRequestHandler, HTTPServer

# File to store the agenda
FILENAME = "agendum.json"

class AgendumHandler(BaseHTTPRequestHandler):
    def _set_headers(self, status=200, content_type="application/json"):
        """Helper function to set headers for responses."""
        self.send_response(status)
        self.send_header("Content-type", content_type)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.end_headers()

    def do_GET(self):
        """Handle GET request to fetch the agenda."""
        if self.path == "/agendum":
            try:
                with open(FILENAME, "r") as file:
                    data = json.load(file)
            except FileNotFoundError:
                data = {"events": []}  # Return empty if file doesn't exist
            except json.JSONDecodeError:
                data = {"error": "Data corrupted."}
                self._set_headers(500)
                self.wfile.write(json.dumps(data).encode("utf-8"))
                return

            self._set_headers()
            self.wfile.write(json.dumps(data).encode("utf-8"))
        else:
            self._set_headers(404, "text/plain")
            self.wfile.write(b"404 Not Found")

    def do_POST(self):
        """Handle POST request to update the agenda."""
        if self.path == "/agendum":
            content_length = int(self.headers["Content-Length"])
            post_data = self.rfile.read(content_length).decode("utf-8")

            try:
                # Parse JSON data from POST request
                data = json.loads(post_data)
                # Save data to file
                with open(FILENAME, "w") as file:
                    json.dump(data, file)
                response = {"status": "success", "message": "Agendum saved successfully."}
                self._set_headers()
            except json.JSONDecodeError:
                response = {"status": "error", "message": "Invalid JSON format."}
                self._set_headers(400)
            except Exception as e:
                response = {"status": "error", "message": str(e)}
                self._set_headers(500)

            # Send response
            self.wfile.write(json.dumps(response).encode("utf-8"))
        else:
            self._set_headers(404, "text/plain")
            self.wfile.write(b"404 Not Found")

    def do_OPTIONS(self):
        """Handle preflight requests for CORS."""
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

def run(server_class=HTTPServer, handler_class=AgendumHandler, port=8080):
    """Run the server."""
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting HTTP server on port {port}")
    httpd.serve_forever()

if __name__ == "__main__":
    run(port=8080)
