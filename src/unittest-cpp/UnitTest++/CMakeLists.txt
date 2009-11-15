
# Written to work with cmake 2.6
cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

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

