
CURDIR = $(shell pwd)
DOCKER=docker run -it -v $(CURDIR):/root/modern:rw advent-of-code-2022 
CHALLENGE?=1
CPPFILES = challenges/*.cpp
run: static-analysis
		/bin/bash -c "echo ""; time ./test"

static-analysis: build
		$(DOCKER) cppcheck --std=c++20 $(CPPFILES) --enable=all -q -Icommon --error-exitcode=1  --suppress="missingIncludeSystem" && \
		$(DOCKER) clang-tidy $(CPPFILES) --checks="*,-fuchsia-*, -misc-no-recursion, -clang-diagnostic-error, -modernize-raw-string-literal, -bugprone-exception-escape, -cert-err58-cpp, -altera-*, -hicpp-use-nullptr, -modernize-use-nullptr, -readability-magic-numbers, -llvm*, -modernize-use-trailing-return-type, -cppcoreguidelines-avoid-magic-numbers, -modernize-use-nodiscard, -readability-named-parameter, -hicpp-named-parameter" --warnings-as-errors="*" -- -Icommon -std=c++20

build:
		$(DOCKER) g++ -g challenges/challenge$(CHALLENGE).cpp -Icommon -std=c++23 -o test -Werror -Wall -Wextra -pedantic -fmodules-ts

docker:
		docker build -t advent-of-code-2022 . 

debug:
		$(DOCKER) gdb ./test

shell:
		$(DOCKER) /bin/bash
