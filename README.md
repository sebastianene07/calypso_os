This OS embedded project is started from scratch and its purpose is for
learning. I started by targeting the nordic nRF52 board because it has good
documentation and there are multiple examples out there on the web.

## Building

Example for building CATOS for Norfic NrF52 board:

```
make config MACHINE_TYPE=nrf52840
make -j
```

## Current features

1. Task scheduling and basic synchronization primitives

The scheduler is preemptive and each task has a fixed allocated time slot.
The scheduler keeps track of two lists g_tcb_waiting_list and g_tcb_list:

g_tcb_waiting_list - holds the tasks waiting for a semaphore
g_tcb_list         - holds the tasks in pending state

The g_current_tcb pointer points to the current running task.

## TODO's

1. Virtual file system support
  1.1 Add support for FatFS (generic filesystem http://elm-chan.org/fsw/ff/00index_e.html)
2. Refactorization & Driver lower half/upper half separation
3. Tickless kernel
4. Tasks prioritization

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

Contributions are welcome.
You can email me on: Sebastian Ene <sebastian.ene07@gmail.com>
