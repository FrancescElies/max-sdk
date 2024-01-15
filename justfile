set shell := ["nu", "-c"]

# generates visual studio projects
[windows]
pre-build:
  mkdir build
  cd build; cmake -G "Visual Studio 17 2022" ..

# builds visual studio projects
[windows]
build:
  cd build; cmake --build . --config Release

# generates and builds with visual studio
[windows]
build-all: pre-build build

update-subtrees:
  git subtree pull --prefix source/my-playground/11globalForegroundWindow --squash https://github.com/FrancescElies/11globalForegroundWindow main
