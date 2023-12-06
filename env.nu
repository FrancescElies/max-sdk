#
# `x` is the entry point command for project specific commands
#

# builds the externals
export def "x build" [] {
  mkdir build
  cd build
  if $nu.os-info.name == "windows" {
    if (ls | is-empty) { cmake -G "Visual Studio 17 2022" .. }
    cmake --build . --config Release
  } else {
    error make {msg: "not implemented" }
  }
}

# updates externals from other repositories
export def "x update-subtrees" [] {
  # Added with
  # git subtree add --prefix source/my-playground/11globalForegroundWindow --squash https://github.com/FrancescElies/11globalForegroundWindow main
  git subtree pull --prefix source/my-playground/11globalForegroundWindow --squash https://github.com/FrancescElies/11globalForegroundWindow main
}
