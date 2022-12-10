
CURDIR = $(shell pwd)
DOCKER=docker run -it -v $(CURDIR):/root/modern:rw advent-of-code-2022 
CHALLENGE?=1
CPPFILES = challenges/*.cpp
run: static-analysis
		/bin/bash -c "echo ""; time ./test"

static-analysis: build
		$(DOCKER) cppcheck --std=c++20 challenges/challenge$(CHALLENGE).cpp --enable=all -q -Icommon --error-exitcode=1  --suppress="missingIncludeSystem"

build:
		$(DOCKER) g++ -g challenges/challenge$(CHALLENGE).cpp -Icommon -std=c++23 -o test -Werror -Wall -Wextra -pedantic -fmodules-ts

docker:
		docker build -t advent-of-code-2022 . 

debug:
		$(DOCKER) gdb ./test

shell:
		$(DOCKER) /bin/bash
