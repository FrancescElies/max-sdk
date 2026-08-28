const toolkit_dir = path self .

const maxmsp = if $nu.os-info.name == "windows" {
    "C:/Program Files/Cycling '74/Max 9/Max.exe"
} else if $nu.os-info.name == "macos" {
    "/Applications/Max.app/Contents/MacOS/Max"
} else {
    error make {msg: $"max not available for ($nu.os-info.name)" }
}

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
    --open-max
] {
    print $"(ansi pi)killing max-msp(ansi reset)"
    ps | find max | each { kill -f $in.pid }

    if not ("build" | path exists) {
        pre-build
    }

    print $"(ansi pi)build(ansi reset)"
    cd $toolkit_dir
    cd build
    cmake --build . --config Release

    if $open_max {
        run-external $maxmsp
    }
}

export def docs-to-md [] {
    uvx --from pymupdf4llm python bin/docs-to-md.py
}
