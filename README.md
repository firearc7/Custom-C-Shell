# Custom C Shell Implementation

A feature-rich shell implementation in C that supports various built-in commands, process management, I/O redirection, and more.

## Features

### Built-in Commands

1. **hop (cd)**
   - Changes current directory
   - Supports `.`, `..`, `~`, and `-` flags
   - Displays full path after changing directory
   - Multiple arguments executed sequentially

2. **reveal (ls)**
   - Lists files/directories in lexicographic order
   - Supports `-a` and `-l` flags
   - Color coding: green for executables, white for files, blue for directories
   - Support for `.`, `..`, `~`, and `-` symbols

3. **log (history)**
   - Stores up to 15 commands
   - Supports command execution by index
   - `log purge` to clear history
   - `log execute <index>` to execute commands from history

4. **proclore (ps)**
   - Displays process information:
     - PID
     - Process Status (R/R+/S/S+/Z)
     - Process Group
     - Virtual Memory
     - Executable Path

5. **seek (find)**
   - Searches for files/directories
   - Supports `-d` (directories only), `-f` (files only), and `-e` (execute/enter) flags
   - Returns relative paths from target directory

### Process Management

- Background process support (`&` operator)
- Process status tracking
- `fg` and `bg` commands for process control
- Signal handling (Ctrl+C, Ctrl+D, Ctrl+Z)
- `ping` command for sending signals to processes
- `activities` command to list running processes

### Additional Features

- I/O Redirection (`>`, `>>`, `<`)
- Pipeline support (`|`)
- Shell configuration file (`.myshrc`)
- Command aliases
- Custom functions
- Internet-based man page fetching (`iMan`)

## Setup and Usage

1. Clone the repository

    ```bash
    git clone https://github.com/firearc7/Custom-C-Shell.git
    cd Custom-C-Shell
    ```

2. Build the shell

    ```bash
    make
    ```

3. Run the shell

    ```bash
    ./a.out
    ```

## Technical Implementation Details

- Written in C
- Modular code structure
- No third-party libraries used
- Custom signal handling
- Process management using fork() and exec() family
- File operations using system calls
- Socket programming for iMan implementation

## Assumptions and Limitations

### General

- Maximum filename length: 4000 characters
- Maximum command length: 4000 characters
- Error commands are stored in log
- Previous directory tracking is updated with each hop
- Log file is stored as absolute path in home directory

### Command Specific

- In .myshrc:
  - Aliases defined without 'alias' keyword
  - Functions defined with 'func' keyword
  - No spaces in aliases
  - Function aliases limited to single argument
  - Function aliases limited to single-word commands

### Process Management‎

- Maximum 4096 background processes
- Process states: R/Z/S considered as running, others as stopped
- Background processes ignored for built-in commands
- Process considered completed if not in proc directory
- Only command name stored in activities

### I/O and Pipes

- I/O redirection works anywhere in command string
- Pipes execute sequentially
- Background process applies only to last command in pipe chain
- For commands like `hop .. | wc`, both directory change and word count occur

### Display and UI

- Only filename/directory name is colored
- Time of last foreground process shown in prompt
- Error messages use perror
- Ctrl+C ineffective without foreground process
- Ctrl+D kills all processes without exit status
- When background process moved to foreground, next prompt shows "fg" as process name

## Error Handling

- Comprehensive error checking for system calls
- Informative error messages
- Shell continues running after command errors
- Permission denial errors handled appropriately

## Contributing

Feel free to submit issues and enhancement requests.
