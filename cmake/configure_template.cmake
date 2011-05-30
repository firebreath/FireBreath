#/**********************************************************\ 
#Original Author: Richard Bateman (taxilian)
#
#Created:    Nov 20, 2009
#License:    Dual license model; choose one of two:
#            New BSD License
#            http://www.opensource.org/licenses/bsd-license.php
#            - or -
#            GNU Lesser General Public License, version 2.1
#            http://www.gnu.org/licenses/lgpl-2.1.html
#            
#Copyright 2009 PacketPass, Inc and the Firebreath development team
#\**********************************************************/


set(__CFG_foreach_regex "@foreach *\\(([^)]+)\\)")
set(__CFG_endforeach_regex "@endforeach")

function(_configure_template_foreach f_contents outputfile)
    set(f_contents ${${f_contents}})
    set(_fe_body "")
    list(GET f_contents 0 fe_line)
    string(REGEX MATCH ${__CFG_foreach_regex} FOUND_FOREACH ${fe_line})

    if (FOUND_FOREACH)
        set(FIRST 1)
        list(REMOVE_AT f_contents 0)
        string(REPLACE " " ";" tokens ${CMAKE_MATCH_1})
        list(LENGTH tokens matchLen)
        set (SUBST_SRC "")
        set (SUBST_DST "")
        while (matchLen GREATER 1)
            LIST(GET tokens 0 src)
            LIST(REMOVE_AT tokens 0)
            LIST(GET tokens 0 dst)
            LIST(REMOVE_AT tokens 0)

            LIST(APPEND SUBST_SRC ${src})
            LIST(APPEND SUBST_DST ${dst})
            list(LENGTH tokens matchLen)
        endwhile()

        set(foreach_block_text "")
        set(SCAN_DONE 0)
        while (NOT SCAN_DONE AND NOT f_contents STREQUAL "")
            list(GET f_contents 0 line)
            list(REMOVE_AT f_contents 0)

            string(REGEX MATCH ${__CFG_endforeach_regex} FOUND_ENDFOREACH "${line}")
            if (FOUND_ENDFOREACH)
                set(SCAN_DONE 1)
            else()
                if (NOT FIRST)
                    list(APPEND foreach_block_text "\n")
                endif()
                if (FIRST)
                    SET(FIRST 0)
                endif()
                list(APPEND foreach_block_text "${line}")
            endif()
        endwhile()
        set(output_text "")
        set(temp "")
        foreach(curText ${foreach_block_text})
            set(temp "${temp}${curText}")
        endforeach()
        set(foreach_block_text "${temp}")
        list(GET SUBST_SRC 0 SUBST_VAR1)
        LIST(LENGTH ${SUBST_VAR1} LOOPNUM)
        MATH(EXPR LOOPNUM "${LOOPNUM} - 1")
        LIST(LENGTH SUBST_SRC SUBST_LEN)
        MATH(EXPR SUBST_LEN "${SUBST_LEN} - 1")
        foreach (i RANGE ${LOOPNUM})
            foreach (n RANGE ${SUBST_LEN})
                LIST(GET SUBST_SRC ${n} CUR_SRC)
                LIST(GET SUBST_DST ${n} CUR_DST)
                LIST(LENGTH ${CUR_SRC} CUR_SRC_LEN)
                if (${CUR_SRC} STREQUAL "" OR CUR_SRC_LEN LESS ${i})
                    set(CUR_SRC_VAL "")
                else()
                    LIST(GET ${CUR_SRC} ${i} CUR_SRC_VAL)
                endif()
                set(${CUR_DST} ${CUR_SRC_VAL})
            endforeach()

            string(CONFIGURE "${foreach_block_text}" curblock_text ESCAPE_QUOTES)
            if (output_text)
                set(output_text "${output_text}\n${curblock_text}")
            else()
                set(output_text "${curblock_text}")
            endif()
        endforeach()
        string(REPLACE ";" "\n" output_text "${output_text}")
        string(REPLACE "~!~" ";" output_text "${output_text}")
        string(REPLACE "~$~" "\\" output_text "${output_text}")
        file(APPEND "${outputfile}" "${output_text}" "\n")
    endif()

    set (file_contents ${f_contents} PARENT_SCOPE)
endfunction()

function(configure_template_force filename outputfile)
    get_filename_component(outputfile "${outputfile}" ABSOLUTE)
    get_filename_component(filename "${filename}" ABSOLUTE)
    #message ("Configuring template ${filename} to ${outputfile}")
    file(WRITE "${outputfile}" "")
    # Read the file into a loop
    FILE(READ "${filename}" file_contents)
    STRING(REGEX REPLACE ";" "~!~" file_contents "${file_contents}")
    STRING(REGEX REPLACE "\\\\" "~$~" file_contents "${file_contents}")
    STRING(REGEX REPLACE "\r" "" file_contents "${file_contents}")
    STRING(REGEX REPLACE "\n" ";" file_contents "${file_contents}")
    #message("File contents: ${file_contents}")
    list(LENGTH file_contents linesLeft)
    while(linesLeft GREATER 0 AND NOT file_contents STREQUAL "")
        list(GET file_contents 0 line)
        string(REGEX MATCH ${__CFG_foreach_regex} FOUND_FOREACH "${line}")
        if (FOUND_FOREACH)
            _configure_template_foreach(file_contents ${outputfile})
        else()
            list(REMOVE_AT file_contents 0)
            string(CONFIGURE "${line}" line ESCAPE_QUOTES)
            string(REPLACE ";" "\n" line "${line}")
            string(REPLACE "~!~" ";" line "${line}")
            string(REPLACE "~$~" "\\" line "${line}")
            file(APPEND "${outputfile}" "${line}" "\n")
            #message("line: ${line}")
        endif()
        list(LENGTH file_contents linesLeft)
    endwhile()
endfunction()

function(configure_template filename outputfile)
    get_filename_component(outputfile_name "${outputfile}" NAME)
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/_tmp)
        file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/_tmp)
    endif()

    configure_template_force(${filename} "${CMAKE_CURRENT_BINARY_DIR}/_tmp/${outputfile_name}")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_BINARY_DIR}/_tmp/${outputfile_name}" "${outputfile}"
        )
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/_tmp/${outputfile_name})

endfunction()
