ifdef FIXTURES
  INFO_ENVIRONMENT="CodeGrade"
  CC=clang-10
  CXX=clang-10
  MC_EXPORT="-fsanitize=address"
  ASAN_EXPORT="symbolize=0"
  CFLAGS=-x c++ -c -Wall -Wpedantic -Werror -std=c++17 $(MC_FLAGS) $(INC_PARAMS) -O1  -Wno-self-assign-overloaded
  LDFLAGS=-lstdc++ -lm -fuse-ld=lld $(MC_FLAGS)
  LINTER=clang-tidy-10
else ifeq ($(shell uname -s),Linux)
  INFO_ENVIRONMENT="Linux/WSL"
  INFO_RELEASE=$(shell lsb_release -d)
  CC=clang-10
  CXX=clang-10
  MC_EXPORT="-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls"
  ASAN_EXPORT="symbolize=1 ASAN_SYMBOLIZER_PATH=$(shell which llvm-symbolizer)"
  CFLAGS=-x c++ -c -Wall  -Wpedantic -Werror -std=c++17 $(MC_FLAGS) $(INC_PARAMS) -g -D_GLIBCXX_DEBUG -O1 -fno-elide-constructors  -Wno-self-assign-overloaded
  LDFLAGS=-lstdc++ -lm $(MC_FLAGS)
  LINTER=clang-tidy
else
  INFO_ENVIRONMENT="MacOS"
  CC=clang
  CXX=clang
  MC_EXPORT="-fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls"
  ASAN_EXPORT="symbolize=1 ASAN_SYMBOLIZER_PATH=$(shell which llvm-symbolizer)"
  CFLAGS=-x c++ -c -Wall  -Wpedantic -Werror -std=c++17 $(MC_FLAGS) $(INC_PARAMS) -g -D_GLIBCXX_DEBUG -O1 -fno-elide-constructors  -Wno-self-assign-overloaded
  LDFLAGS=-lstdc++ -lm $(MC_FLAGS)
  LINTER=clang-tidy
endif

ifndef HEADER
  HEADER=true
endif

ifeq (${HEADER},true)
  $(info +---------------------------------------------------------------------)
  $(info | Environment detected AS ${INFO_ENVIRONMENT})
  $(info | $(INFO_RELEASE))
  $(info | * compiler: $(CC))
  $(info | * linker: $(CXX))
  $(info | * linter: $(LINTER))
  $(info | * home: $(HOME))
  $(info | * fixtures: $(FIXTURES))
  $(info | * memcheck compile and link flags set to: $(MC_EXPORT))
  $(info | * asan flags set to: $(ASAN_EXPORT))
  $(info +---------------------------------------------------------------------)
endif

INCLUDE_DIRS= include test/include
INC_PARAMS=$(foreach d, $(INCLUDE_DIRS), -I$d)

SRC:=$(shell find src -name '*.cpp')
OBJECTS=$(SRC:.cpp=.o)

TEST_SRC=$(shell find test -name '*.cpp')
TEST_OBJECTS=$(TEST_SRC:.cpp=.o)
TEST_EXECUTABLE=runtests

APP_SRC=main.cpp
APP_OBJECTS=$(APP_SRC:.cpp=.o)
APP_EXECUTABLE=app

ZIP_FILE=dv1626-assignment1-assembler.zip

all: clean $(SRC) $(APP_SRC) $(APP_EXECUTABLE) $(TEST_SRC) $(TEST_EXECUTABLE)

$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(OBJECTS)
	@echo "Linking test executable ($@)"
	@$(CXX) $(LDFLAGS) $(TEST_OBJECTS) $(OBJECTS) -o $@

$(APP_EXECUTABLE): $(APP_OBJECTS) $(OBJECTS)
	@echo "Linking app executables"
	@$(CXX) $(LDFLAGS) $(APP_OBJECTS) $(OBJECTS) -o $@

.cpp.o:
	@echo "Compiling object file ($@)"
	@$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	@echo "Cleaning recreatable files:"
	@echo "  * App executables"
	@-rm -f $(APP_EXECUTABLE)
	@echo "  * Test executables"
	@-rm -f $(TEST_EXECUTABLE)
	@echo "  * Zip files"
	@-rm -f $(ZIP_FILE)
	@echo "  * Data files from compiler"
	@find . -name "*.gcda" -print0 | xargs -0 rm -f
	@find . -name "*.gcno" -print0 | xargs -0 rm -f
	@echo "  * Object files"
	@find . -name "*.o" -print0 | xargs -0 rm -f	

.PHONY: test
test: clean all
	@echo Running tests
	./$(TEST_EXECUTABLE)

.PHONY: memcheck
memcheck:
	MC_FLAGS=$(MC_EXPORT) $(MAKE) all HEADER=false
	ASAN_OPTIONS=$(ASAN_EXPORT) ./$(TEST_EXECUTABLE)

.PHONY: static-code-analysis
static-code-analysis:
	$(LINTER) --header-filter=".*.hpp" --warnings-as-errors='*' --checks="cppcoreguidelines-*, bugprone*, diagnostic-*, -clang-diagnostic-c++17-extensions, analyzer-*, readability-*, -cppcoreguidelines-pro-bounds-pointer-arithmetic, -cppcoreguidelines-special-member-functions, -cppcoreguidelines-owning-memory" ${SRC} ${APP_SRC} -- ${INC_PARAMS}

.PHONY: zip
zip:
	${MAKE} clean HEADER=false
	@echo "\nCreating zip target ${ZIP_FILE}\n"
	@${MAKE} $(ZIP_FILE) HEADER=false

$(ZIP_FILE):
	@zip -r  $(ZIP_FILE) . -x '*.git*' -x '*.zip' -x '*.md' -x '.vs*'