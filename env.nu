#
# `x` is the entry point command for project specific commands
#

# compiles a compile_commands.json file
# This file can be used by clangd to provide lsp
export def "x clang-commands-json" [] {
  mkdir build
  powershell -File ./clang-power-tools/ClangPowerTools/ClangPowerTools/Tooling/v1/clang-build.ps1  -export-jsondb
}

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
  # git subtree add --prefix clang-power-tools --squash https://github.com/Caphyon/clang-power-tools master
  # git subtree add --prefix source/my-playground/11globalForegroundWindow --squash https://github.com/FrancescElies/11globalForegroundWindow main
  git subtree pull --prefix source/my-playground/11globalForegroundWindow --squash https://github.com/FrancescElies/11globalForegroundWindow main
  git subtree pull --prefix clang-power-tools --squash https://github.com/Caphyon/clang-power-tools master
}
