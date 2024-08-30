PREFIX?=/usr/local
CFLAGS=-Wall -g -O2 -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)

SOURCES=$(wildcard src/**/**/*.c src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=build/libmork.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

all: mork tests

mork: $(TARGET) $(SO_TARGET)
	$(CC) -o bin/mork main.c $(CFLAGS) $(LIBS) $(TARGET)

dev: CFLAGS=-Wall -g -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@
$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

build:
	@mkdir -p build
	@mkdir -p bin

# The Unit Tests
.PHONY: tests
tests: CFLAGS += $(OBJECTS)
tests: $(TESTS)
	sh ./tests/runtests.sh

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_]gets\(|[^_]strcpy\(|[^_]strcat\(|[^_]sprintf\(|[^_]gets\(|[^_]scanf\(' $(SOURCES) || true
