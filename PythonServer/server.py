import tornado
import tornado.ioloop
import tornado.web
import tornado.httpserver
import os
import json
import logging
import random

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
        self.finish()
        
class GetVideoStatus(tornado.web.RequestHandler):
    def get(self):
        logging.debug("Video status")
        status = {}
        status['status'] = 'value'
        self.write(json.dumps(status))
        self.finish()

class UploadVideo(tornado.web.RequestHandler):
    @tornado.web.asynchronous
    def post(self):
        logging.debug("Receiving video")
        fileInfo = self.request.files[key][0]
        extn = os.path.splitext(fileInfo['filename'])[1]
        #id = fileInfo['videoid'] # to be received from front end
        id = "video_id"
        #filename = id + extn
        filename = str(random.randint(1,100)*5) + extn # generate random file name to test processing
                                                       # multiple processes
        with open(toProcDir + filename, 'wb') as f: # save file
            f.write(fileInfo['body'])
        os.system("sudo " + processDir + "./cvprocessor -f " + toProcDir + \
                 filename + " -o " + doneDir + filename)
        os.system("cd to_process && rm " + filename + "&& cd ..")
        print("done processing")
        self.finish()
        
def main():
    logging.basicConfig(filename=curDir + "/logs/log.log", 
                        format='%(asctime)s --- %(levelname)s: %(message)s', 
                        datefmt='%m/%d/%Y %I:%M:%S %p',
                        level=logging.DEBUG
                       )
                       
    app = tornado.web.Application([
        (r"/", DefaultHandler),
        (r"/upload", UploadVideo),
        (r"/status", GetVideoStatus),
        (r"/video/(.*)", tornado.web.StaticFileHandler, \
        {"path":r"" + doneDir})
        ])
    server = tornado.httpserver.HTTPServer(app)
    server.bind(8000)
    debugMsg("Server started on port 8000....")
    server.start(0) # spawning 4 subprocesses
    try:
        tornado.ioloop.IOLoop.current().start()
    except KeyboardInterrupt:
        tornado.ioloop.IOLoop.instance().stop()
    print("shut down")

if __name__ == "__main__":
    main()
