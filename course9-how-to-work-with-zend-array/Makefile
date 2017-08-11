# PHP环境变量
PHP_INCLUDE = `php-config --includes`
PHP_LIBS = `php-config --libs`
PHP_LDFLAGS = `php-config --ldflags`
PHP_INCLUDE_DIR = `php-config --include-dir`
PHP_EXTENSION_DIR = `php-config --extension-dir`

# 编译器
CC = gcc

# 编译参数
CFLAGS = -g -O0 -gdwarf-2 -fPIC -I. ${PHP_INCLUDE} -I${PHP_INCLUDE_DIR}

# 链接参数
LDFLAGS = -shared -L${PHP_EXTENSION_DIR} ${PHP_LIBS} ${PHP_LDFLAGS}

# 源代码
SRC = myext.c
# 头文件
HEADER = myext.h

# .o目标文件
OBJECTS = $(SRC:.c=.o)

.PHONY: all test clean

all: myext.so
	@echo "done"

myext.so: $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o:%.cpp $(HEADER)
	$(CC) -o $@ -c $< $(CFLAGS)

install: myext.so
	cp myext.so ${PHP_EXTENSION_DIR}/

test:
	php test.php

clean:
	rm -f *.so
	rm -f *.o
