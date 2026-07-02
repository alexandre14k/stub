add_rules("mode.debug", "mode.release")

add_requires("fmt", "gtk4")

target("stub")
    set_kind("binary")
    set_languages("c++17")
    add_includedirs("src")
    add_files("src/*.cpp")

    local libfiles = os.files("lib/*.a") or {}
    if #libfiles > 0 then
        add_files(libfiles)
    end

    add_packages("fmt", "gtk4")