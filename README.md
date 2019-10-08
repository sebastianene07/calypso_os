This OS embedded project is started from scratch and its purpose is for
low power devices with running with low memory.

## Building

Example for building Calypso OS for Norfic NrF52 board:

```
make config MACHINE_TYPE=nrf52840
make -j
```

## Current features

### 1. Task scheduling and basic synchronization primitives

The scheduler is preemptive and each task has a fixed allocated time slot.
The scheduler keeps track of two lists g_tcb_waiting_list and g_tcb_list:  &nbsp;

g_tcb_waiting_list - holds the tasks waiting for a semaphore &nbsp;

g_tcb_list         - holds the tasks in pending state        &nbsp;

The g_current_tcb pointer points to the current running task. &nbsp;
A task transitions from running to waiting for semaphore &nbsp;
when it tries to acquire a semaphore that has a value <= 0. &nbsp;
The task is placed in the g_tcb_waiting_list and it's context &nbsp;
is saved on the stack. &nbsp;

### 2. Dynamic memory allocation

The allocator is implemented as part of a submodule in s_alloc. It can be
configure to run with multipple heap objects depending on the needs.
It supports the following operations: s_alloc, s_free, s_realloc
and the code is tested in a sample program (main.c) that verify the integrity
of the allocated blocks and monitors the fragmentation. The test also 
verifies that the allocated regions do not overlap.
In the current configuration it supports up to 2 ^ 31 - 1 blocks of memory
where the block size is defined by the code that it's using the allocator.
The allocation is done in O(N) space time (searching through the list of free
nodes) whereas the free operation can take O(N^2) because of the block merging
and sorting (this can be improved to O(NlogN)). 

### 3. Virtual File System

The virtual file system contains a tree like structure with nodes that allows
us to interract with the system resources. The current nodes are:

```              root node
                    "/"
             --------------------
           |    |    |    |    |
          dev  mnt  bin  otp  home
       /   |
 ttyUSB0  rtc0
```

A task contains a list of ```struct opened_resource_s``` which is essentially
the struct file from Linux and has the same role.
The registered nodes in the virtual file sytem are described by
``` struct vfs_node_s ``` and these keep informations such as:
- device type
- supported operations
- name
- private data

The open device flow:

```
open(..) -> vfs_get_matching_node(..) -------
            /\                              |
         Extract the node from the VFS      |
         and call into ops open_cb(..)      |
                                            |
                                      sched_allocate_resource() -----
                                      /\                            |
                               Allocate a new opened_resource_s     |
                               in the calling process.              |
                               Return the index of the              |
                               opened_resource_s structure as       |
                               the file descriptor.                 |
                                                                    |
                -----------------------------------------------------
                |-> vfs_node_open()
                      /\
                   Call the appropriate node open function.
```

## TODO's

```
1. Virtual file system support
  1.1 Add support for FatFS http://elm-chan.org/fsw/ff/00index_e.html (DONE)
  1.1.1 Need SD/MMC driver support to get FatFS fully functional

  1.2 Add functional tests for memory allocator                       (DONE)
  1.4 The read/write device flow                                      (DONE)
  1.5 Polling from devices

2. Refactorization & Driver lower half/upper half separation          (DONE)
3. Tickless kernel
4. Tasks prioritization option

```

## Porting Guide (TODO)

- 

## Contributions

Contributions are welcome.

You can email me on: Sebastian Ene <sebastian.ene07@gmail.com>
