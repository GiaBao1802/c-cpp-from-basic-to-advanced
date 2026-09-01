CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Werror -MMD -MP
SRC := callback_dispatch.c
OBJ := $(SRC:.c=.o)

.PHONY: all clean
all: callback_demo

callback_demo: $(OBJ)
	$(CC) $(OBJ) -Wl,-Map=callback_demo.map -o $@

-include $(OBJ:.o=.d)

clean:
	$(RM) $(OBJ) $(OBJ:.o=.d) callback_demo callback_demo.map
