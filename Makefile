TARGET=tc
GHC:=ghc

all: src/$(TARGET)

src/$(TARGET): src/$(TARGET).c
	cc -O2 -o $@ $<

src/gen_input: src/gen_input.hs
	(cd src; $(GHC) -O2 gen_input.hs)

gen_inputs: input/all_01.txt.bz2
input/all_01.txt.bz2:
	for i in 01 02 03 04 05 06 07 08 09 10 11 12 13 14; do \
		./src/gen_input $$i | bzip2 -c > input/all_$$i.txt.bz2; \
	done

clean:
	rm -f src/$(TARGET)
	rm -f src/*.hi src/*.o src/gen_input

veryclean: clean
	rm -f input/all_??.txt.bz2

test01: src/$(TARGET)
	bzcat input/all_01.txt.bz2 | ./src/$(TARGET) -d

test02: src/$(TARGET) #完全2分木
	echo 2,1,3 | ./src/tc -n -d
	echo 4,2,1,3,6,5,7 | ./src/tc -b -d
	echo 8,4,2,1,3,6,5,7,12,10,9,11,14,13,15 | ./src/tc -b -d

test03: gen_inputs src/$(TARGET)
	for f in 01 02 03 04; do \
		echo "Processing $${f} by Makinen M 2000";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -d -m; done; \
		echo "Processing $${f} by Algorithm N";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -d -n; done; \
		# echo "Processing $${f} by Algorithm B";\
		# bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -d -b; done; \
		echo "Processing $${f} by Algorithm C";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -d -c; done; \
	done

test04: src/$(TARGET)
	for f in 06; do \
		echo "Processing $${f} by Makinen M 2000";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -m; done; \
		echo "Processing $${f} by Algorithm N";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -n; done; \
	done

mmm:
	for f in 12 13 14; do \
		time sh -c "bzcat input/all_$$f.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc -mt; done > output_expected/$$f.txt";\
	done


test05: src/$(TARGET)
	for f in 01 02 03 04 05 06 07 08 09 10 11 12 13 14; do \
		echo "size = $$f ---------------------------";\
		for algo in m n c; do \
			ALGO=$$(echo $$algo | tr 'a-z' 'A-Z'); \
			echo "Processing $$f by Algorithm $${ALGO}";\
			time sh -c "bzcat input/all_$$f.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc -$${algo}t; done > output/test05_$${ALGO}_size$$f.txt";\
			bzip2 -c output/test05_$${ALGO}_size$$f.txt > output/test05_$${ALGO}_size$$f.txt.bz2;\
			rm -f output/test05_$${ALGO}_size$$f.txt;\
			zcmp output_expected/$$f.txt.bz2 output/test05_$${ALGO}_size$$f.txt.bz2;\
		done;\
	done

test06: src/$(TARGET)
	for f in 01 02 03 04 05 06 07 08 09 10 11 12 13 14; do \
		echo "size = $$f ---------------------------";\
		for algo in m n c; do \
			ALGO=$$(echo $$algo | tr 'a-z' 'A-Z'); \
			echo "Processing $$f by Algorithm $${ALGO}";\
			time sh -c "bzcat input/all_$$f.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc -$${algo}; done";\
		done;\
	done


diff:
	for f in 01 02 03 04 05 06 07 08 09 10 11 12 13 14; do \
		echo "size = $$f ---------------------------";\
		ALGO=C;\
		zcmp output_expected/$$f.txt.bz2 output/test05_$${ALGO}_size$$f.txt.bz2;\
	done

test07: src/$(TARGET)
	for f in 01 02 03 04 05 06 07 08 09 10 11 12 13 14; do \
		echo "size = $$f ---------------------------";\
		for algo in c; do \
			ALGO=$$(echo $$algo | tr 'a-z' 'A-Z'); \
			echo "Processing $$f by Algorithm $${ALGO}";\
			time sh -c "bzcat input/all_$$f.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc -$${algo}t; done > output/test05_$${ALGO}_size$$f.txt";\
			bzip2 -c output/test05_$${ALGO}_size$$f.txt > output/test05_$${ALGO}_size$$f.txt.bz2;\
			rm -f output/test05_$${ALGO}_size$$f.txt;\
			zcmp output_expected/$$f.txt.bz2 output/test05_$${ALGO}_size$$f.txt.bz2;\
		done;\
	done

test07tmp: src/$(TARGET)
	for f in 12 13 14; do \
		echo "size = $$f ---------------------------";\
		for algo in m n; do \
			ALGO=$$(echo $$algo | tr 'a-z' 'A-Z'); \
			echo "Processing $$f by Algorithm $${ALGO}";\
			time sh -c "bzcat input/all_$$f.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc -$${algo}t; done > output/test05_$${ALGO}_size$$f.txt";\
			bzip2 -c output/test05_$${ALGO}_size$$f.txt > output/test05_$${ALGO}_size$$f.txt.bz2;\
			rm -f output/test05_$${ALGO}_size$$f.txt;\
			zcmp output_expected/$$f.txt.bz2 output/test05_$${ALGO}_size$$f.txt.bz2;\
		done;\
	done

.PHONEY: clean
