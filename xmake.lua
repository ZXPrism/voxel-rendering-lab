set_project("voxel-rendering-lab")

add_rules("mode.debug", "mode.release")

add_repositories("my-xmake-repo https://github.com/ZXPrism/my-xmake-repo.git")
add_requires("gfx-utils")

target("voxel-rendering-lab")
    set_languages("cxx20")
    set_kind("binary")
    set_warnings("all", "error", "extra", "pedantic")

    add_includedirs("src")
    add_files("src/**.cpp")
    add_packages("gfx-utils")

    after_build(function (target)
        os.cp(target:targetfile(), "bin/")
        os.cp("assets/", "bin/")
    end)
target_end()
