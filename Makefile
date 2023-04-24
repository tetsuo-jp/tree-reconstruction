TARGET=tc

all: test

src/$(TARGET): src/$(TARGET).c

test01: src/$(TARGET)
	zcat input/all_01.txt.gz | ./src/$(TARGET) -d

test: src/$(TARGET)
	for f in 01 02 03 04; do \
		echo "Processing $${f} by Makinen M";\
		zcat input/all_$${f}.txt.gz | while read line; do echo "$$line" |./src/tc -d -m; done; \
		echo "Processing $${f} by Algorithm A";\
		zcat input/all_$${f}.txt.gz | while read line; do echo "$$line" |./src/tc -d -a; done; \
	done

clean:
	rm -f src/$(TARGET)
