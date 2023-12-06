mkdir build
cd build
ps | where name =~ 'Max' | each {|x| kill $x.pid } 
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
let path = (echo `~/src/max-sdk/externals` | path expand | str replace -a "\\" "/")
echo $"make sure (ansi purple_italic)($path)(ansi reset) is in maxsearchpath"
start ./playground.maxpat
