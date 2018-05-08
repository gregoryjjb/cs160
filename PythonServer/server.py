import tornado
import tornado.ioloop
import tornado.web
import tornado.httpserver
import os
import json
import logging
import re

curDir = os.getcwd()
toProcDir = curDir + "/to_process/"
doneDir = curDir + "/done/"
processDir = curDir + "/../CVProcessor/dist/Debug/GNU-Linux/"
upload_key = 'file'
id_key = 'videoid'

def main():
    logging.basicConfig(filename=curDir + "/logs/log.log", 
                        format='%(asctime)s --- %(levelname)s: %(message)s', 
                        datefmt='%m/%d/%Y %I:%M:%S %p',
                        level=logging.DEBUG
                       )
    # Server routes
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
        os.system("sudo rm -r frames && sudo rm -r processed") # delete folders that were created when processing app created
    debugMsg("Server shutdown")

# For debugging
def debugMsg(msg):
    logging.debug(msg)
    print(msg)

# Default route
class DefaultHandler(tornado.web.RequestHandler):
    def get(self):
        debugMsg("Server running, default route")
        self.write("Server running")
        self.finish()
        
# Returns status of the video
class GetVideoStatus(tornado.web.RequestHandler):
    @tornado.web.asynchronous
    def post(self):
        status = {}
        vid_id = self.get_argument(id_key)
        debugMsg("Getting video status for " + vid_id)
        
        # check if output file exists
        if(not os.path.isfile(vid_id + '.txt')):
            if(not os.path.isfile(doneDir + vid_id + ".mp4")):
               status['status'] = vid_id + ': error, video not found'
               debugMsg("Video not found")
            else:
                status['status'] = vid_id + ': processing complete'
                debugMsg("Processing complete")
        else:
            linesList = list()
            with open(curDir + "/" + vid_id + '.txt', 'r') as fh:
                linesList = fh.readlines()
            if(re.search('Metadata', linesList[-2])): 
                status['status'] = vid_id + ': initializing'
                debugMsg("Initializing")
            elif(re.search('Frame', linesList[-2])):
                status['status'] = vid_id + ': splitting'
                debugMsg("Splitting")
            elif(re.search('Triangles', linesList[-2])):
                status['status'] = vid_id + ': processing'
                debugMsg("Processing")
        self.write(json.dumps(status))
        self.finish()

# Receives video and calls processing application on it
class UploadVideo(tornado.web.RequestHandler):
    @tornado.web.asynchronous
    def post(self):
        debugMsg("Receiving video...")
        fileInfo = self.request.files[upload_key][0]
        vid_id = self.get_argument(id_key)
        extn = os.path.splitext(fileInfo['filename'])[1]
        filename = vid_id + extn
        debugMsg("Video " + vid_id + extn + " received, saving video...")
        with open(toProcDir + filename, 'wb') as f: # save file
            f.write(fileInfo['body'])
        debugMsg("Processing video...")
        # call processing application on video, pipe output to text file that can be parsed to obtain the processing status for a given video
        os.system("sudo " + processDir + "./cvprocessor -f " + toProcDir + filename + " -o " + doneDir + filename + "| grep -E '^Begin' > " + vid_id + ".txt")
        os.system("cd to_process && rm " + filename + "&& cd ..")
        os.system("sudo rm " + vid_id + ".txt") # delete the text file
        debugMsg("Processing complete")
        self.finish()

if __name__ == "__main__":
    main()
