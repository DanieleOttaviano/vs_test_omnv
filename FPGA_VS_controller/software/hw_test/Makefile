CC ?= gcc
ARCH ?= arm64
CROSS_COMPILE ?= aarch64-linux-gnu-

all: shared_page conf_port hw_test

shared_page: shared_page.c shared_page.h
	$(CROSS_COMPILE)$(CC) -o shared_page shared_page.c -lm

conf_port: conf_port.c conf_port.h
	$(CROSS_COMPILE)$(CC) -o conf_port conf_port.c -lm

hw_test: hw_test.c
	$(CROSS_COMPILE)$(CC) -o hw_test hw_test.c -lm

# test: locusta_user_test

# test_no_locusta: locusta_user_test_no_locusta

# lib: locusta_user.o

# locusta_user_test: locusta_user.h locusta_user.o locusta_user_test.c
# 	$(CROSS_COMPILE)$(CC) -o locusta_user_test locusta_user.o locusta_user_test.c -lm

# locusta_user_test_no_locusta: locusta_user.h locusta_user.o locusta_user_test.c
# 	$(CROSS_COMPILE)$(CC) -DNO_LOCUSTA -o locusta_user_test_no_locusta locusta_user.o locusta_user_test.c -lm

# locusta_user.o: locusta_user.h locusta_user.c
# 	$(CROSS_COMPILE)$(CC) -c locusta_user.c

clean:
	rm shared_page conf_port
