import tornado
import tornado.ioloop
import tornado.web
import os

class DefaultHandler(tornado.web.RequestHandler):
    def get(self):
        self.write("hello world")
        
class GetVideoStatus(tornado.web.RequestHandler):
    def get(self):
        self.write("status of video")
        
class GetVideo(tornado.web.RequestHandler):
    def get(self):
        self.write("here are some videos")

class UploadVideo(tornado.web.RequestHandler):
    def post(self):
        fileInfo = self.request.files['fff'][0]
        extn = os.path.splitext(fileInfo['filename'])[1]
        with open('new' + extn, 'wb') as f:
            f.write(fileInfo['body'])
        print("done")
        self.write("file uploaded successfully")

application = tornado.web.Application([
        (r"/", DefaultHandler),
        (r"/upload", UploadVideo),
        (r"/status", GetVideoStatus),
        (r"/video", UploadVideo),
        ], debug=True)
        
def main():
    port = 8000
    application.listen(port)
    print("Listening on port: " + str(port) + "....")
    server = tornado.httpserver.HTTPServer(application)
    tornado.ioloop.IOLoop.instance().start()


if __name__ == "__main__":
    main()
