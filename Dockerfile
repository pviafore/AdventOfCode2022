FROM ubuntu:kinetic
RUN apt-get update
RUN apt-get install -y build-essential gdb cppcheck clang-tidy clang
RUN mkdir /root/modern
WORKDIR /root/modern
CMD /bin/bash
