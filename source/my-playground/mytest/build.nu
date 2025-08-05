
{
    mkdir build
    cd build
    ps | where name =~ 'Max' | each {|x| kill $x.pid }
    cmake -G "Visual Studio 17 2022" ..
    cmake --build . --config Release
    let path = (echo `~/src/max-sdk/externals` | path expand | str replace -a "\\" "/")
    echo $"make sure (ansi purple_italic)($path)(ansi reset) is in maxsearchpath"
}

# "~/AppData/Roaming/Cycling '74/Max 9/Settings/maxsearchpaths.txt"
# max setsearchpath 5 ~/src/oss/max-sdk/externals userpath_5;
start ./playground.maxpat

