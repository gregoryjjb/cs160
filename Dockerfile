FROM debian:stretch

WORKDIR /usr/var/app/processing/

COPY install-dependencies.sh .
RUN chmod +x *.sh && ./install-dependencies.sh

COPY download-openface.sh .
RUN chmod +x *.sh && ./download-openface.sh

COPY install-openface.sh .
RUN chmod +x *.sh && ./install-openface.sh

ENV PYTHON_SERVER="Python Server"
COPY "CVProcessor" ${PYTHON_SERVER} ./

COPY install-processing.sh .
RUN chmod +x *.sh && ./install-processing.sh

ENTRYPOINT ["docker-entrypoint.sh"]
