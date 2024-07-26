function(global_include DIRS)
    if(NOT DIRS OR DIRS STREQUAL "")
        set(DIRS "${CMAKE_SOURCE_DIR}")
    endif()

    # 初始化目录计数
    set(directory_count 0)

    # 输出目录列表
    message("输出目录列表")

    # 遍历传入的目录列表
    foreach(DIR IN LISTS DIRS)
        # 使用 find 命令查找所有非以点开头的目录，并将结果存储在 found_directories 变量中
        execute_process(
            COMMAND find "${DIR}" -type d -not -path "*/.*"
            # WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE found_directories
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )

        # 将找到的目录按行分割成列表
        string(REPLACE "\n" ";" directories_list "${found_directories}")

        # 遍历所有找到的目录并进行处理
        foreach(directory IN LISTS directories_list)
            # 输出子目录
            message("${directory}")

            # 包含子目录
            include_directories("${directory}")

            # 如果子目录匹配 CMAKE_ANDROID_ARCH_ABI，则链接目录
            if(directory MATCHES "${CMAKE_ANDROID_ARCH_ABI}")
                link_directories("${directory}")
            endif()

            # 增加目录计数
            math(EXPR directory_count "${directory_count} + 1")
        endforeach()
    endforeach()

    # 输出目录总数
    message("一共 ${directory_count} 个目录")
endfunction()
