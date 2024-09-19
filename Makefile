PREFIX?=/usr/local
CFLAGS=-Wall -g -O2 -Wextra -Isrc -rdynamic -DNDEBUG $(OPTFLAGS)
LIBS=-ldl $(OPTLIBS)

SOURCES=$(wildcard src/**/**/*.c src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

# Headers are not used in the build process, but are used in the install process
HEADERS=$(wildcard src/**/**/*.h src/**/*.h src/*.h)

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TARGET=build/libmork.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

all: $(TARGET) $(SO_TARGET) tools tests

tools: dbcli

demo: $(TARGET) $(SO_TARGET)
	$(CC) -o bin/demo example/main.c $(CFLAGS) $(LIBS) $(TARGET) example/gamedata.c

dbcli: $(TARGET) $(SO_TARGET)
	$(CC) -o bin/dbcli tools/dbcli.c $(CFLAGS) $(LIBS) $(TARGET)

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
	rm -rf bin build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log tests/*.db
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Checker
check:
	@echo Files with potentially dangerous functions.
	@egrep '[^_]gets\(|[^_]strcpy\(|[^_]strcat\(|[^_]sprintf\(|[^_]gets\(|[^_]scanf\(' $(SOURCES) || true

# Install
install: all
# Copy header files into /usr/local/include
# Headers should keep their directory structure
	mkdir -p $(PREFIX)/include/mork
	cp -R src/* $(PREFIX)/include/mork
# Make sure we only keep headers in the include folder
	@rm -f $(PREFIX)/include/mork/**/**/*.c
	@rm -f $(PREFIX)/include/mork/**/*.c
	@rm -f $(PREFIX)/include/mork/*.c
	@rm -f $(PREFIX)/include/mork/**/**/*.o
	@rm -f $(PREFIX)/include/mork/**/*.o
	@rm -f $(PREFIX)/include/mork/*.o
# Copy bundled static library into /usr/local/lib
	install $(TARGET) $(PREFIX)/lib

uninstall:
	rm -rf $(PREFIX)/include/mork
	rm -f $(PREFIX)/lib/$(TARGET)
