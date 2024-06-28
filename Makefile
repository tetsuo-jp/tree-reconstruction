TARGET=tc

all: test01

src/$(TARGET): src/$(TARGET).c

src/gen_input: src/gen_input.hs
	(cd src; ghc -O2 gen_input.hs)

gen_inputs: src/gen_input
	for i in 01 02 03 04 05 06 07 08 09 10 11 12 13 14; do \
		./src/gen_input $$i | bzip2 -c > input/all_$$i.txt.bz2; \
	done

clean:
	rm -f src/$(TARGET)
	rm -f src/*.hi src/*.o src/gen_input

veryclean: clean
	rm -f input/all_??.txt.bz2

test01: src/$(TARGET)
	zcat input/all_01.txt.gz | ./src/$(TARGET) -d

test02: src/$(TARGET) #完全2分木
	echo 2,1,3 | ./src/tc -a -d
	echo 4,2,1,3,6,5,7 | ./src/tc -b -d
	echo 8,4,2,1,3,6,5,7,12,10,9,11,14,13,15 | ./src/tc -b -d

test03: gen_inputs src/$(TARGET)
	for f in 01 02 03 04; do \
		echo "Processing $${f} by Makinen M";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -d -m; done; \
		echo "Processing $${f} by Algorithm A";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -d -a; done; \
		echo "Processing $${f} by Algorithm B";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -d -b; done; \
	done

test04: src/$(TARGET)
	for f in 06; do \
		echo "Processing $${f} by Makinen M";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -m; done; \
		echo "Processing $${f} by Algorithm A";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -a; done; \
	done
