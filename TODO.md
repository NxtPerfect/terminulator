- [x] Create a window
- [x] Accept input
- [x] Show output
    - [x] XLookupString isn't real
    - [x] show what button was pressed on the screen
- [x] Write text on screan
    - [x] Parse special characters like space, enter, backspace, comma, colon, semicolon
- [x] On backspace, remove last character
- [/] On enter go to next line and run command
    - [x] Send output of command to program
    - [x] Restructure current typing architecture
        - Handle user input as one function, writing as normal
        - Then for command output, have seperate function
        that doesn't interfere with writing buffer
        it simply writes the output and starts new prompt
        for user, meaning the output isn't related to input
        - [x] Prints out command output, clears buffer
        then type out another command, it won't catch
        the output from previous command, as input
    - [x] When trying to copy to output buffer
    i get error of accessing bad memory
    - [x] DrawString doesn't use offset at all
[Source](https://www.x.org/releases/current/doc/libX11/libX11/libX11.html#Output_Methods)
