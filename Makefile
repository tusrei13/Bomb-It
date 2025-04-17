CC = g++
CFLAGS = -Iinclude -std=c++17 -Wall
LDFLAGS = -Llib -lSDL2 -lSDL2_image -lSDL2_mixer
SRC = $(wildcard *.cpp) # Tự động tìm tất cả các file .cpp
OBJ = $(SRC:.cpp=.o)
TARGET = BombIT.exe

all: $(TARGET)

$(TARGET): $(OBJ)
    $(CC) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    del /Q $(OBJ) $(TARGET) # Sử dụng lệnh del thay cho rm trên Windows

run: all
    $(TARGET) # Chạy file thực thi trực tiếp trên Windows