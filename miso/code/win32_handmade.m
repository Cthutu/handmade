-|                  |
 | Handmade Hero    |
 |                  |-

#import "Windows" as Win32

main: ()
{
    Win32.MessageBoxW(null, "This is Handmade Hero.", "Handmade Hero", Win32.MB_OK | Win32.MB_ICONINFORMATION)
    return 0
}

