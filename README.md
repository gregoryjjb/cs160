# CS160 Computer Vision Pipeline

A video processing application using OpenFace and FFMPEG. Primarily authored by @deanljohnson.

The CVProcessor takes either a video file or RTSP stream as an input, and outputs the video with delaunay triangles applied to detected faces. It can output to a video file, RTSP stream, or STDOUT, and is capable of real-time streaming face detection.

The application was designed to work in tandem with a [server](https://github.com/gregdumb/cs160-server) and [web UI](https://github.com/gregdumb/cs160-server), allowing video upload conversion and real-time streaming using a webcam.

## Installation

Clone the repo and run `sudo bash install.sh`. This will install all required dependencies, as well as build the project. It takes roughly 20 minutes on a high end machine with a good internet connection.

The compiled output will be in `CVProcessor/dist/your system info`. We only tested in Debian-based systems.

---

## Python Server

A network API comes bundled with the application (primary author @dang3).

### Installation

* Install/switch to Python 3. The server was developed and tested in Python 3.5.2.
* The server uses the tornado framework. Install tornado `pip install tornado`

### API Routes
1. Method: `GET` <br />
   Route: `/` <br />
   If the server is running, `Server running` should appear.
   
2. Method: `GET` <br />
   Route: `/video/(.*)` <br />
   Searches the `\done` directory for the specified video file. For example, `/video/12345.mp4` will search for and return       `12345.mp4` if the file exists. Otherwise, a `404` response will be sent.
   
3. Method: `POST` <br />
   Route: `/upload` <br />
   Used to upload a video to the server which will temporarily be cloned to the `\to_process` directory. The video file is then    processed with the output file from the processing application being placed in the `\done` directory which can be accessed through the `/video/(.*)` GET request. After the processing, the uploaded file is deleted.  <br />
   Use the `file` key when uploading video files. Every uploaded video should have a videoid. Within the same request, the client should 
   specify videoid as a plain string using the `videoid` key within the body of the request. Each unique video should have its own 
   videoid  or else video files will be overwritten.
   
4. Method: `POST` <br />
   Route: `/status` <br />
   Returns the status of a video. Within the body of the request, use the key `videoid` and specify an id for the video.
   
   
