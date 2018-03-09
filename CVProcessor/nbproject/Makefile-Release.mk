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
CND_CONF=Release
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
	${OBJECTDIR}/EyeLike/findEyes.o \
	${OBJECTDIR}/EyeLike/helpers.o \
	${OBJECTDIR}/EyeLikeProcessing.o \
	${OBJECTDIR}/FFMPEGProcessing.o \
	${OBJECTDIR}/OpenFaceProcessing.o \
	${OBJECTDIR}/OutputWriter.o \
	${OBJECTDIR}/SystemHelper.o \
	${OBJECTDIR}/VideoProcessing.o \
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
LDLIBSOPTIONS=`pkg-config --libs opencv`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cvprocessor

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cvprocessor: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/cvprocessor ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/EyeLike/findEyeCenter.o: EyeLike/findEyeCenter.cpp 
	${MKDIR} -p ${OBJECTDIR}/EyeLike
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EyeLike/findEyeCenter.o EyeLike/findEyeCenter.cpp

${OBJECTDIR}/EyeLike/findEyeCorner.o: EyeLike/findEyeCorner.cpp 
	${MKDIR} -p ${OBJECTDIR}/EyeLike
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EyeLike/findEyeCorner.o EyeLike/findEyeCorner.cpp

${OBJECTDIR}/EyeLike/findEyes.o: EyeLike/findEyes.cpp 
	${MKDIR} -p ${OBJECTDIR}/EyeLike
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EyeLike/findEyes.o EyeLike/findEyes.cpp

${OBJECTDIR}/EyeLike/helpers.o: EyeLike/helpers.cpp 
	${MKDIR} -p ${OBJECTDIR}/EyeLike
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EyeLike/helpers.o EyeLike/helpers.cpp

${OBJECTDIR}/EyeLikeProcessing.o: EyeLikeProcessing.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EyeLikeProcessing.o EyeLikeProcessing.cpp

${OBJECTDIR}/FFMPEGProcessing.o: FFMPEGProcessing.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/FFMPEGProcessing.o FFMPEGProcessing.cpp

${OBJECTDIR}/OpenFaceProcessing.o: OpenFaceProcessing.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OpenFaceProcessing.o OpenFaceProcessing.cpp

${OBJECTDIR}/OutputWriter.o: OutputWriter.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/OutputWriter.o OutputWriter.cpp

${OBJECTDIR}/SystemHelper.o: SystemHelper.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SystemHelper.o SystemHelper.cpp

${OBJECTDIR}/VideoProcessing.o: VideoProcessing.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/VideoProcessing.o VideoProcessing.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 `pkg-config --cflags opencv` -std=c++11  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

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
