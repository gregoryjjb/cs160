import tornado
import tornado.ioloop
import tornado.web
import os
import json
import logging

curDir = os.getcwd()
toProcDir = curDir + "/to_process"
doneDir = curDir + "/done"
key = 'file'

def debugMsg(msg):
    logging.debug(msg)
    print(msg)

class DefaultHandler(tornado.web.RequestHandler):
    def get(self):
        self.write("Python server running")
        
class GetVideoStatus(tornado.web.RequestHandler):
    def get(self):
        self.write("status of video")

class UploadVideo(tornado.web.RequestHandler):
    def post(self):
        fileInfo = self.request.files[key][0]
        extn = os.path.splitext(fileInfo['filename'])[1]
        with open(toProcDir + '/new' + extn, 'wb') as f:
            f.write(fileInfo['body'])
        debugMsg("File cloned successfully")

application = tornado.web.Application([
        (r"/", DefaultHandler),
        (r"/upload", UploadVideo),
        (r"/status", GetVideoStatus),
        (r"/video/(.*)", tornado.web.StaticFileHandler, \
        {"path":r"/home/dennis/Desktop/HW/CS160/cs160/Python Server/done/"})
        ], debug=True)
        
def main():
    logging.basicConfig(filename=curDir + "/logs/log.log", 
                        format='%(asctime)s --- %(levelname)s: %(message)s', 
                        datefmt='%m/%d/%Y %I:%M:%S %p',
                        level=logging.DEBUG
                       )
    port = 8000
    application.listen(port)
    logging.debug("Server started on port " + str(port))
    print("Listening on port: " + str(port) + "....")
    server = tornado.httpserver.HTTPServer(application)
    tornado.ioloop.IOLoop.instance().start()


if __name__ == "__main__":
    main()
