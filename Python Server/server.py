from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

class Server(BaseHTTPRequestHandler):
    def _set_headers(self):
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()

    def do_GET(self):
        self._set_headers()
        if self.path == '/':
        	self.wfile.write("<html><body><h1>CSCVOP<br/>Hello World</h1></body></html>")
        elif self.path == '/status':
        	self.wfile.write("<html><body><h1>Give video(id) status</h1></body></html>")
        elif self.path == '/video':
        	self.wfile.write("<html><body><h1>Return video(id)</h1></body></html>")
        
    def do_POST(self):
        self._set_headers()
        if self.path == '/upload':
        	self.wfile.write("<html><body><h1>Upload video</h1></body></html>")
        
def run(server_class=HTTPServer, handler_class=Server, port=5000):
    server_address = ('localhost', port)
    httpd = server_class(server_address, handler_class)
    print "Server Started....."
    try:
    	httpd.serve_forever()
    except KeyboardInterrupt:
    	print '\nKeyboard Interrupt, shutting down server'
    httpd.shutdown()

if __name__ == "__main__":
    from sys import argv

    if len(argv) == 2:
        run(port=int(argv[1]))
    else:
        run()
