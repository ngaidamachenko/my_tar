CC = gcc
CFLAGS = -Wall -Wextra -Werror -g3 -fsanitize=address
SRCS = my_tar.c
TARGET = my_tar

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

fclean: clean
	rm -f $(EXEC)

re: fclean all

.PHONY: all clean fclean re