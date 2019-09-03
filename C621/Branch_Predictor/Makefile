SOURCE	:= Main.c Trace.c Branch_Predictor.c
CC	:= gcc
TARGET	:= Main

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) -o $(TARGET) $(SOURCE) -lm

clean:
	rm $(TARGET)
