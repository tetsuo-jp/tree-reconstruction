TARGET=tc

all: test

src/$(TARGET): src/$(TARGET).c

test01: src/$(TARGET)
	zcat input/all_01.txt.gz | ./src/$(TARGET) -d

test: src/$(TARGET)
	for f in 01 02 03 04; do \
		echo "Processing $${f} by Makinen M";\
		zcat input/all_$${f}.txt.gz | while read line; do echo "$$line" | ./src/tc -d -m; done; \
		echo "Processing $${f} by Algorithm A";\
		zcat input/all_$${f}.txt.gz | while read line; do echo "$$line" | ./src/tc -d -a; done; \
		echo "Processing $${f} by Algorithm B";\
		zcat input/all_$${f}.txt.gz | while read line; do echo "$$line" | ./src/tc -d -b; done; \
	done

clean:
	rm -f src/$(TARGET)

test02: #完全2分木
	echo 2,1,3 | ./src/tc -a -d
	echo 4,2,1,3,6,5,7 | ./src/tc -b -d
	echo 8,4,2,1,3,6,5,7,12,10,9,11,14,13,15 | ./src/tc -b -d
