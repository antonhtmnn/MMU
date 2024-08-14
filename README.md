## MMU-and-Allocator

This repository contains the implementation of a custom memory allocator and a Memory Management Unit (MMU) as part of a system programming course at the Technical University of Berlin. The project involves managing memory allocation using a linked list-based approach and translating virtual addresses to physical addresses.

### Table of Contents

- [Project Description](#project-description)
- [Installation](#installation)
- [Makefile Usage](#makefile-usage)
- [Usage](#usage)
- [Memory Allocator](#memory-allocator)
- [Memory Management Unit (MMU)](#memory-management-unit-mmu)
- [Project Structure](#project-structure)

### Project Description

The MMU and Allocator project is designed to simulate memory allocation and management within a software-based environment. The project includes the implementation of a custom allocator based on a doubly linked list and a memory management unit that handles the translation of virtual addresses to physical addresses.

### Installation

To set up the project on your local machine, follow these steps:

1. Clone the repository:
    ```bash
    git clone https://github.com/antonhtmnn/MMU.git
    ```
2. Navigate to the `MMU-and-Allocator` directory:
    ```bash
    cd MMU-and-Allocator/
    ```
3. Compile the code using the provided Makefile:
    ```bash
    make
    ```

### Makefile Usage

The provided Makefile automates the build and testing process. It uses the clang C-compiler. If you do not have clang installed, you can install it with:
```bash
sudo pacman -S clang
```

#### Make targets

- **`make build`** or **`make all`** or just **`make`**: Compiles `src/calloc.c` and `src/mmu.c`. No executable is created at this stage.
- **`make run`** or **`make all_tests`**: Builds all tests and runs them.
- **`make build_tests`**: Compiles all tests without executing them.
- **`make list_tests`**: Lists all available tests.
- **`make test_*`**: Runs only the specified test. The target name is the same as the test file name (without `.c`). Example: `make test_calloc_alloc`
- **`make gdb_test_*`**: Runs the specified test in gdb. Example: `make gdb_test_example_simple`
- **`make clean`**: Cleans up the build directory.

### Usage

To run the MMU and Allocator implementations, use the compiled binaries from the `src` directory. You can also run the tests provided in the `tests` directory to verify the functionality.

### Memory Allocator

The custom allocator is implemented based on the following specifications:

- **Doubly Linked List Structure:** The allocator uses a doubly linked list to manage memory blocks. Each block contains a pointer to the next and previous blocks, along with a size attribute.
- **Next-Fit Allocation Strategy:** The allocator uses a next-fit strategy to allocate memory, which continues the search from where the last allocation occurred.
- **Merging Free Blocks:** When a block is freed, adjacent free blocks are merged to avoid fragmentation.

Functions implemented include:
- `my_calloc()`: Allocates memory and initializes it to a specified value.
- `my_calloc_free()`: Frees the allocated memory and merges adjacent free blocks if possible.

### Memory Management Unit (MMU)

The MMU is responsible for translating virtual addresses to physical addresses. The implementation includes:

- **Page Table Management:** Each process has a page table that maps virtual addresses to physical addresses.
- **Access Rights and Presence Bits:** The MMU checks access rights and the presence of pages in memory before translating addresses.
- **Process Switching:** The `switch_process()` function updates the page table base register when a process switch occurs.

Functions implemented include:
- `switch_process(int proc_id)`: Switches the current process by updating the page table base register.
- `mmu_check_request()`: Validates memory access requests and translates virtual addresses to physical addresses.
- `mmu_translate()`: Performs the actual translation of a virtual address to a physical address.

### Project Structure

The project is structured as follows:

```
MMU-and-Allocator/
├── include/
│   ├── calloc.h
│   ├── mmu.h
├── src/
│   ├── calloc.c   # Implementation of the custom allocator
│   ├── mmu.c      # Implementation of the Memory Management Unit (MMU)
├── Makefile       # Makefile for building the project
├── tests/         # Contains test files. New tests should be added here.
└── build/         # This directory is automatically created during compilation and contains all object and executable files.
```
