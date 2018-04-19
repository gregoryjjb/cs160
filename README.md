# Python Server
Installation
-------
* Install/switch to Python 3. The server was developed and tested in Python 3.5.2.
* Install tornado `pip install tornado`

## Requests
1. Method: `GET` <br />
   Route: `/` <br />
   If the server is running, `Server running` should appear.
   
2. Method: `GET` <br />
   Route: `/video/(.*)` <br />
   Searches the `\done` directory for the specified video file. For example, `/video/12345.mp4*` will search for and return       `12345.mp4` if the file exists. Otherwise, a `404` response will be sent.
   
3. Method: `POST` <br />
   Route: `/upload` <br />
   Used to upload a file to the server which will temporarily be cloned to the `\to_process` directory. The video file is then    processed with the output file from the processing application being placed in the `\done` directory which can be accessed      through the `/video/(.*)` GET request. After the processing, the uploaded file is deleted.
   
4. Method: `GET` <br />
   Route: `/status` <br />
   To do
   
   
