#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/EyeLike/findEyeCenter.o \
	${OBJECTDIR}/EyeLike/findEyeCorner.o \
	${OBJECTDIR}/EyeLike/helpers.o \
	${OBJECTDIR}/EyeLike/main.o \
	${OBJECTDIR}/FFMPEGProcessing.o \
	${OBJECTDIR}/OpenFaceProcessing.o \
	${OBJECTDIR}/SystemHelper.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/lib/x86_64-linux-gnu -lUtilities -lLandmarkDetector `pkg-config --libs opencv` `pkg-config --libs dlib-1` -lboost_filesystem -lboost_system -ltbb -lpthread   

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cvprocessor

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cvprocessor: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cvprocessor ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/EyeLike/findEyeCenter.o: EyeLike/findEyeCenter.cpp 
	${MKDIR} -p ${OBJECTDIR}/EyeLike
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/boost -I/usr/local/include/OpenFace `pkg-config --cflags opencv` `pkg-config --cflags dlib-1` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EyeLike/findEyeCenter.o EyeLike/findEyeCenter.cpp

${OBJECTDIR}/EyeLike/findEyeCorner.o: EyeLike/findEyeCorner.cpp 
	${MKDIR} -p ${OBJECTDIR}/EyeLike
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/boost -I/usr/local/include/OpenFace `pkg-config --cflags opencv` `pkg-config --cflags dlib-1` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EyeLike/findEyeCorner.o EyeLike/findEyeCorner.cpp

${OBJECTDIR}/EyeLike/helpers.o: EyeLike/helpers.cpp 
	${MKDIR} -p ${OBJECTDIR}/EyeLike
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/boost -I/usr/local/include/OpenFace `pkg-config --cflags opencv` `pkg-config --cflags dlib-1` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EyeLike/helpers.o EyeLike/helpers.cpp

${OBJECTDIR}/EyeLike/main.o: EyeLike/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/EyeLike
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/boost -I/usr/local/include/OpenFace `pkg-config --cflags opencv` `pkg-config --cflags dlib-1` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EyeLike/main.o EyeLike/main.cpp

${OBJECTDIR}/FFMPEGProcessing.o: FFMPEGProcessing.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/boost -I/usr/local/include/OpenFace `pkg-config --cflags opencv` `pkg-config --cflags dlib-1` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FFMPEGProcessing.o FFMPEGProcessing.cpp

${OBJECTDIR}/OpenFaceProcessing.o: OpenFaceProcessing.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/boost -I/usr/local/include/OpenFace `pkg-config --cflags opencv` `pkg-config --cflags dlib-1` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OpenFaceProcessing.o OpenFaceProcessing.cpp

${OBJECTDIR}/SystemHelper.o: SystemHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/boost -I/usr/local/include/OpenFace `pkg-config --cflags opencv` `pkg-config --cflags dlib-1` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SystemHelper.o SystemHelper.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I/usr/include/boost -I/usr/local/include/OpenFace `pkg-config --cflags opencv` `pkg-config --cflags dlib-1` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cvprocessor

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
