function(dcc_source_package_ignore_files source_root output)
    set(separator "[/\\\\]")
    set(non_separator "[^/\\\\]")
    # CPack matches absolute paths. Normalize both slash styles in the source
    # root before escaping regex metacharacters so Windows paths match the
    # same anchored patterns as POSIX paths.
    string(REGEX REPLACE "([][+.*()^$?|])" "[\\1]" root_regex "${source_root}")
    string(REPLACE "/" "${separator}" root_regex "${root_regex}")
    set(patterns
        "${separator}build${non_separator}*${separator}"
        "${separator}CMakeFiles${separator}"
        "${separator}CMakeCache[.]txt$"
        "${separator}cmake_install[.]cmake$"
        "${separator}install_manifest[.]txt$"
        "${separator}Testing${separator}"
        "${separator}site${separator}"
        "${separator}target${separator}"
        "${separator}dist${separator}"
        "${separator}[.]venv${separator}"
        "${separator}compile_commands[.]json$"
        "${separator}__pycache__${separator}"
        "[.]pyc$"
        "${separator}[.]DS_Store$"
        "${separator}[.]env($|[.])"
        "${separator}[.]git${separator}"
        "${separator}[.]git$"
        "${separator}[.]superpowers${separator}"
        "${separator}[.]worktrees${separator}"
        "${separator}include${separator}dcc${separator}sugar[.]h$"
        "${separator}include${separator}dcc${separator}sugar${separator}"
        "${separator}include${separator}dcc${separator}app${separator}legacy[.]h$"
        "${separator}include${separator}dcc${separator}rest${separator}core[.]h$"
        "${separator}include${separator}dcc${separator}rest${separator}core${separator}"
        "${separator}include${separator}dcc${separator}rest${separator}response_helpers[.]h$"
    )
    set(anchored)
    foreach(pattern IN LISTS patterns)
        list(APPEND anchored "^${root_regex}(${separator}.*)?${pattern}")
    endforeach()
    set(${output} "${anchored}" PARENT_SCOPE)
endfunction()
