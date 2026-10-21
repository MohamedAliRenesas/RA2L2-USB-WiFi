from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import urlparse, parse_qs

state = {"blue": "?", "green": "?", "red": "?"}

class Handler(BaseHTTPRequestHandler):
    def reply(self, body, ctype="text/plain"):
        data = body.encode()
        self.send_response(200)                          # "200 OK"
        self.send_header("Content-Type", ctype)
        self.send_header("Content-Length", str(len(data)))  # the FSP client requires this
        self.end_headers()
        self.wfile.write(data)

    def do_GET(self):
        url = urlparse(self.path)
        if url.path == "/update":                        # called by the RA2L2
            q = parse_qs(url.query)
            for led in state:
                if led in q:
                    state[led] = "ON" if q[led][0] == "1" else "off"
            self.reply("ok")                             # tiny reply, fits the 256 B buffer
        else:                                            # the page you open in a browser
            rows = "".join(f"<li>{k}: {v}</li>" for k, v in state.items())
            self.reply("<html><head><meta http-equiv='refresh' content='2'></head><body>"
                       f"<h2>RA2L2 LEDs</h2><ul>{rows}</ul></body></html>", "text/html")

HTTPServer(("0.0.0.0", 8000), Handler).serve_forever()