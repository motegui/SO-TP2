x64BareBones is a basic setup to develop operating systems for the Intel 64 bits architecture.

The final goal of the project is to provide an entry point for a kernel and the possibility to load extra binary modules separated from the main kernel.

Environment setup:
1- Install the following packages before building the Toolchain and Kernel:

nasm qemu gcc make

2- Build the Toolchain

Execute the following commands on the x64BareBones project directory:

  user@linux:$ cd Toolchain
  user@linux:$ make all

3- Build the Kernel

From the x64BareBones project directory run:

  user@linux:$ make all

4- Run the kernel

From the x64BareBones project directory run:

  user@linux:$ ./run.sh

Another way is to compile using the docker.sh script. This will create a docker image with all the required dependencies and compile the kernel.
  
    user@linux:$ ./docker.sh
  
  Then you can run the kernel using the run.sh script.
  
    user@linux:$ ./run.sh

Be aware that you may need to change your audio device on the run.sh script.