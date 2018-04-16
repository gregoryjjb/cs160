import tornado
import tornado.ioloop
import tornado.web
import os
import json
import logging

curDir = os.getcwd()
toProcDir = curDir + "/to_process/"
doneDir = curDir + "/done/"
processDir = curDir + "/../CVProcessor/dist/Debug/GNU-Linux/"
key = 'file'

def debugMsg(msg):
    logging.debug(msg)
    print(msg)

class DefaultHandler(tornado.web.RequestHandler):
    def get(self):
        self.write("Python server running")
        logging.debug("Default route")
        
class GetVideoStatus(tornado.web.RequestHandler):
    def get(self):
        logging.debug("Video status")
        status = {}
        status['status'] = 'value'
        self.write(json.dumps(status))

class UploadVideo(tornado.web.RequestHandler):
    def post(self):
        logging.debug("Receiving video")
        fileInfo = self.request.files[key][0]
        extn = os.path.splitext(fileInfo['filename'])[1]
        #id = fileInfo['videoid'] # to be received from front end
        id = "video_id"
        filename = id + extn
        with open(toProcDir + filename, 'wb') as f: # save file
            f.write(fileInfo['body'])
        os.system("sudo " + processDir + "./cvprocessor -f " + toProcDir + filename + " -o " + doneDir + filename)
        os.system("cd to_process && rm " + filename + "&& cd ..")
        print("done processing")

application = tornado.web.Application([
        (r"/", DefaultHandler),
        (r"/upload", UploadVideo),
        (r"/status", GetVideoStatus),
        (r"/video/(.*)", tornado.web.StaticFileHandler, \
        {"path":r"" + doneDir})
        ], debug=True)
        
def main():
    logging.basicConfig(filename=curDir + "/logs/log.log", 
                        format='%(asctime)s --- %(levelname)s: %(message)s', 
                        datefmt='%m/%d/%Y %I:%M:%S %p',
                        level=logging.DEBUG
                       )
    port = 8000
    application.listen(port)
    debugMsg("Server started on port " + str(port) + "....")
    server = tornado.httpserver.HTTPServer(application)
    tornado.ioloop.IOLoop.instance().start()


if __name__ == "__main__":
    main()
