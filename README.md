This OS embedded project is started from scratch and its purpose is for
learning. I started by targeting the nordic nRF52 board because it has good
documentation and there are multiple examples out there on the web.

## Building

Example for building CATOS for Norfic NrF52 board:

```
make config MACHINE_TYPE=nrf52840
make -j
```

## TODO's

1. Task scheduling, synchronization primitives
2. Driver lower half/upper half separation
3. File system support

Virtual File System

The virtual file system contains all the registered nodes that we can interract
with.
A task contains an array of struct file_s which keep track of the opened
files. A file is the abstraction of any device in the same way as Linux
provides the.

struct file_s
{
  struct vfs_node_s *node;
  uint32_t seek_pos;
}

The open device flow:

open(..) -> file_open(..dev_name..)
                /\
         Allocate a new file_s structure
         in the calling process. Search
         for the registered device_name
         of type vfs_node_s and call the
         open function on that node.
         Return the index of the file_s
         structure from the current task.

                -> vfs_node_open()
                      /\
                   Call the appropriate node open function.

The read/write device flow:

Polling from devices:

## Contents

1. Getting Started
2. Porting Guide (TODO)
3. Tools

## Contributions

Contributions are really welcome. You can email me on: Sebastian Ene <sebastian.ene07@gmail.com>
