#Custom Shell Project

## Motivation

I built this project to strengthen my low-level programming skills. My ultimate goal is to master Edge AI, Robotics, Embedded Systems, and Cloud/Network computing—fields that all require a deep, foundational understanding of how software interacts directly with the operating system and hardware. This is my first project from a list i make before. 

## What i code

Started from Stephen Brennan's "Write a Shell in C" tutorial. My additions:
 - Fixed EOF handling (Ctrl+D now exits cleanly)
 - Rewrite the tokenizer (recognize operators (|, <, >, >>, &) even without spaces)
 - Rewrite the tokenizer (handle quotes ("hello world" = one argument))
 - Redirections (<, >, >>)
 - Pipes, including chains (cmd1 | cmd2 | cmd3)
 - Signal handling (Ctrl+C kills the running command, not the shell)
 - Background execution (&), with SIGCHLD reaping zombie processes
 - Wildcards (*, ?) expanded with glob()
 - cd with no argument goes to $HOME
 - Exit status of the last command ($?)
 - Environment variables ($HOME) and the export builtin
 - Command history and line editing (readline)

## Build
```
make
./shell
```
Needs readline (included on macOS, `libreadline-dev` on Linux).

## Source
I used many source to build this project. Here they are :
 - https://github.com/codecrafters-io/build-your-own-x
 - https://brennan.io/2015/01/16/write-a-shell-in-c/

## AI use
I coded the base and the tokenizer myself, using AI only to know what can i increase on a custom shell make by a student. 
