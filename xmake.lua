set_project("voxel-rendering-lab")

add_rules("mode.debug", "mode.release")
add_requires("spdlog", "glm", "glad", "glfw", "stb")
add_requires("imgui", {configs = { glfw = true, opengl3 = true }})

target("voxel-rendering-lab")
    set_languages("cxx20")
    set_kind("binary")
    set_warnings("all", "error", "extra", "pedantic")

    add_includedirs("src")
    add_files("src/**.cpp")
    add_packages("spdlog", "glm", "glad", "glfw", "imgui","stb")

    after_build(function (target)
        os.cp(target:targetfile(), "bin/")
        os.cp("assets/", "bin/")
    end)
target_end()
