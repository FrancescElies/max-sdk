const toolkit_dir = path self .

def "nu-complete subprojects" [] {
    return (fd cmakelists ./source | lines | each {$in | path dirname })
}

export def "update subtrees" [] {
    git subtree pull --prefix source/my-playground/11globalForegroundWindow --squash https://github.com/FrancescElies/11globalForegroundWindow main
}


export def pre-build [] {
    print $"(ansi pi)pre-build(ansi reset)"
    cd $toolkit_dir
    if ("build" | path exists) {
        return
    }
    mkdir build
    cd build
    cmake -G "Visual Studio 17 2022" .. # generates visual studio projects
}

export def build [
    --filter(-k): string@"nu-complete subprojects" = ""  # a space-separated list of feature to install with Nushell
] {
    print $"(ansi pi)killing max-msp(ansi reset)"
    ps | find max | each { kill -f $in.pid }

    if not ("build" | path exists) {
        pre-build
    }

    print $"(ansi pi)build(ansi reset)"
    cd $toolkit_dir
    cmake --build . --config Release

}
