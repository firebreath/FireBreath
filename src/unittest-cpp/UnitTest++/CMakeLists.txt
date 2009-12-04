#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            Eclipse Public License - Version 1.0
#            http://www.eclipse.org/legal/epl-v10.html
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/

# Written to work with cmake 2.8
cmake_minimum_required (VERSION 2.8)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.8)

include(${CMAKE_DIR}/common.cmake)

project (UnitTest++)
message ("Generating project ${PROJECT_NAME}")

include_directories (
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

list (APPEND SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/src/AssertException.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Test.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/Checks.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TestRunner.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TestResults.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TestReporter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TestReporterStdout.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/ReportAssert.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TestList.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TimeConstraint.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/TestDetails.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/MemoryOutStream.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/DeferredTestReporter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/DeferredTestResult.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/XmlTestReporter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/CurrentTest.cpp
)

if (UNIX)
	list(APPEND SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/src/Posix/SignalTranslator.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/src/Posix/TimeHelpers.cpp
	)
elseif (WIN32)
	list(APPEND SOURCES
			src/Win32/TimeHelpers.cpp
	)
endif()

add_library (UnitTest++ STATIC ${SOURCES})

add_definitions(
    -D_CRT_SECURE_NO_DEPRECATE
    )

if (UNIX)
	set_target_properties(UnitTest++ PROPERTIES
			COMPILE_FLAGS "-g -Wall -W -ansi"
	)
endif()

