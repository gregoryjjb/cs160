# All dependencies
FROM debian:stretch AS default

# Environment
ENV PROJECT_DIR="/usr/var/app/processing"
ENV SCRIPTS_DIR="${PROJECT_DIR}/scripts"
ENV DEPENDENCIES_DIR="${PROJECT_DIR}/dependencies"
ENV DEPENDENCIES_BUILD_DIR="${DEPENDENCIES_DIR}/build"
#ENV CVPROCESSOR_DIR="{PROJECT_DIR}/CVProcessor"

WORKDIR $DEPENDENCIES_DIR

# Install essential dependencies

# TODO: DRY method of copy + chmod + run script
# NOTE: Don't try and copy all the scripts in at first because
#       docker will invalidate the copy stage and have to re-
#       run ALL of the scripts!

COPY scripts/install-essential-dependencies.sh $SCRIPTS_DIR/
RUN chmod +x $SCRIPTS_DIR/*.sh && $SCRIPTS_DIR/install-essential-dependencies.sh

# Copy bundled dependencies (for OpenFace + its dependencies)
COPY dependencies $DEPENDENCIES_DIR

# Build and install OpenFace + its dependencies
COPY scripts/setup-dependency-builds.sh $SCRIPTS_DIR/
RUN chmod +x $SCRIPTS_DIR/*.sh && $SCRIPTS_DIR/setup-dependency-builds.sh
WORKDIR $DEPENDENCIES_BUILD_DIR

# OpenCV
COPY scripts/build-opencv.sh $SCRIPTS_DIR/
RUN chmod +x $SCRIPTS_DIR/*.sh && $SCRIPTS_DIR/build-opencv.sh

# dlib
COPY scripts/build-dlib.sh $SCRIPTS_DIR/
RUN chmod +x $SCRIPTS_DIR/*.sh && $SCRIPTS_DIR/build-dlib.sh

# OpenFace
COPY scripts/build-openface.sh $SCRIPTS_DIR/
RUN chmod +x $SCRIPTS_DIR/*.sh && $SCRIPTS_DIR/build-openface.sh

# Clean up
WORKDIR $PROJECT_DIR
RUN rm -rf $DEPENDENCIES_DIR
RUN rm -rf $BUILD_DIR

# ===================================================== #

# CVProcessor proper
FROM default AS CVProcessor

# Copy processing server files
WORKDIR $PROJECT_DIR
COPY CVProcessor CVProcessor

# Build processing server
WORKDIR $PROJECT_DIR/CVProcessor
COPY scripts/build-processing.sh $SCRIPTS_DIR/
RUN chmod +x $SCRIPTS_DIR/*.sh && $SCRIPTS_DIR/build-processing.sh

# Install more dependencies
COPY scripts/install-server-dependencies.sh $SCRIPTS_DIR/
RUN chmod +x $SCRIPTS_DIR/*.sh && $SCRIPTS_DIR/install-server-dependencies.sh

# Copy Python server files
WORKDIR $PROJECT_DIR
COPY PythonServer PythonServer

# Setup Python server
WORKDIR $PROJECT_DIR/PythonServer
COPY scripts/setup-python-server.sh $SCRIPTS_DIR/
RUN chmod +x $SCRIPTS_DIR/*.sh && $SCRIPTS_DIR/setup-python-server.sh

# Ready to rumble
WORKDIR $PROJECT_DIR
COPY docker-entrypoint.sh .
RUN chmod +x docker-entrypoint.sh
ENTRYPOINT ["./docker-entrypoint.sh"]
