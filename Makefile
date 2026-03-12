CC = cc
CFLAGS = -O2 -lm
TARGET = haiku

all: $(TARGET)

$(TARGET): haiku.c
	$(CC) haiku.c $(CFLAGS) -o $(TARGET)

no-web: haiku.c
	$(CC) haiku.c $(CFLAGS) -DHAIKU_NO_WEB -o $(TARGET)

test: $(TARGET)
	echo "what is resonance" | ./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all no-web test clean
