# Platform Engine

Platform engine is a work in progress using the [r-tech1](https://github.com/kazzmir/r-tech1) library.

## Background
Create an engine to easily script up a platformer in Python.

## Requirements

 * Scons
 * [r-tech1](https://github.com/kazzmir/r-tech1) library
 * Python development libraries
 * libz

## Setup

```bash
# Basic build
$ scons

# With debugging symbols
$ export DEBUG=1
$ scons

# To run
$ ./platformer
```

## Example Definition Script
The configuration or definitions have not been finalized. Below is just an example of what it looks like now: 

```Scheme
; Test of mario bros.

(platformer
    ; The name of the platformer game
    (name "Super Mario Bros.")
    
    ; A few font mappings derived from a bitmap
    ;@(font 
        (name smb) 
        (ignore-case 1)
        (filename "platformer/smb/images/smb-font.png")
        (character-map "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ :-.")
        )
    (font 
        (name small) 
        (filename "platformer/smb/images/font1.png")
        (character-map "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&`'*#=[]\"")
        )
    (font
        (name large) 
        (filename "platformer/smb/images/font2.png")
        (character-map "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.,!?-+/():;%&`'*#=[]\"<>()^~|_@$\\")
        )
    
    ; Starting world defnition file
    (world "platformer/smb/worlds/world-1-1.txt")
)
```
