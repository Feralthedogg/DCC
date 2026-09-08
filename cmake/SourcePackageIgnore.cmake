function(dcc_source_package_ignore_files source_root output)
    # CPack matches absolute paths. Escape the source root using character
    # classes so generated CPack config needs no extra backslash escaping.
    string(REGEX REPLACE "([][+.*()^$?|])" "[\\1]" root_regex "${source_root}")
    set(patterns
        "/build[^/]*/"
        "/CMakeFiles/"
        "/CMakeCache[.]txt$"
        "/cmake_install[.]cmake$"
        "/install_manifest[.]txt$"
        "/Testing/"
        "/site/"
        "/target/"
        "/dist/"
        "/[.]venv/"
        "/compile_commands[.]json$"
        "/__pycache__/"
        "[.]pyc$"
        "/[.]DS_Store$"
        "/[.]env($|[.])"
        "/[.]git/"
        "/[.]git$"
        "/[.]superpowers/"
        "/[.]worktrees/"
        "/include/dcc/sugar[.]h$"
        "/include/dcc/sugar/"
        "/include/dcc/app/legacy[.]h$"
        "/include/dcc/rest/core[.]h$"
        "/include/dcc/rest/core/"
        "/include/dcc/rest/response_helpers[.]h$"
    )
    set(anchored)
    foreach(pattern IN LISTS patterns)
        list(APPEND anchored "^${root_regex}(/.*)?${pattern}")
    endforeach()
    set(${output} "${anchored}" PARENT_SCOPE)
endfunction()
