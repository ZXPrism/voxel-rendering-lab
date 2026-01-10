set_project("voxel-rendering-lab")

add_rules("mode.debug", "mode.release")
add_requires("glad", "glm", "libsdl3", "stb", "spdlog")
-- add_defines("MACRO_NAME=1")
-- add_defines("MACRO_NAME=\"SOME STRING\"")

target("vox")
    set_languages("cxx20")
    set_kind("binary")
    set_warnings("all", "extra", "pedantic", "error")

    add_includedirs("src")
    add_files("src/**.cpp")
    add_packages("glad", "glm", "libsdl3", "stb", "spdlog")

    if is_plat("windows") then
        add_cxflags("/utf-8", {force = true})
        add_cxxflags("/utf-8", {force = true})
        add_cxxflags("/wd4068", {force = true}) -- for #pragma clang
    end

    after_build(function (target)
        os.cp(target:targetfile(), "bin/")
        os.cp("assets", "bin/")
        os.cp("src/shader", "bin/assets")
    end)
target_end()
