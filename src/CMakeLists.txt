cmake_minimum_required(VERSION 3.22.1)

project(HRMI)

execute_process(
    COMMAND make all-before
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_executable(
    ${PROJECT_NAME} 
    main.c
    interp.c
    parse.c
    collections/alist.c
    collections/hashtable.c
    collections/stack.c
    common/CMemLeak.c
    common/platformhelper.c
    common/utils.c
    compiler/ast.c
    compiler/astdata.c
    compiler/parser.tab.c
    compiler/scanner.yy.c
)

target_link_libraries(${PROJECT_NAME} m)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "hrmi")
