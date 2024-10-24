TARGET=tc
GHC:=stack ghc -- -Wall -O2

all: src/$(TARGET)

src/$(TARGET): src/$(TARGET).c
	cc -O2 -o $@ $<

src/tc_c: src/tc_c.c
	cc -O2 -o $@ $<

src/gen_input: src/gen_input.hs
	(cd src; $(GHC) -O2 gen_input.hs)

# gen_inputs: input/all_01.txt.bz2
# input/all_01.txt.bz2:
# 	for i in 01 02 03 04 05 06 07 08 09 10 11 12 13 14; do \
# 		./src/gen_input $$i | bzip2 -c > input/all_$$i.txt.bz2; \
# 	done

gen_inputs: input/all_01.txt.bz2 input/all_02.txt.bz2 input/all_03.txt.bz2 input/all_04.txt.bz2 input/all_05.txt.bz2 input/all_06.txt.bz2 input/all_07.txt.bz2 input/all_08.txt.bz2 input/all_09.txt.bz2 input/all_10.txt.bz2 input/all_11.txt.bz2 input/all_12.txt.bz2 input/all_13.txt.bz2 input/all_14.txt.bz2
input/all_%.txt.bz2: src/gen_input
	./src/gen_input $* | bzip2 -c > $@

clean:
	rm -f src/$(TARGET)
	rm -f src/*.hi src/*.o src/gen_input

veryclean: clean
	rm -f input/all_??.txt.bz2

test01: src/$(TARGET)
	bzcat input/all_01.txt.bz2 | ./src/$(TARGET) -d

# small complete binary trees
test02: src/$(TARGET)
	echo 2,1,3 | ./src/tc -n -d
	echo 4,2,1,3,6,5,7 | ./src/tc -b -d
	echo 8,4,2,1,3,6,5,7,12,10,9,11,14,13,15 | ./src/tc -b -d

# small inputs 01-04
test03: gen_inputs src/$(TARGET)
	for f in 01 02 03 04; do \
		echo "Processing $${f} by Makinen M 2000";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -d -m; done; \
		echo "Processing $${f} by Algorithm N";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -d -n; done; \
		echo "Processing $${f} by Algorithm C";\
		bzcat input/all_$${f}.txt.bz2 | while read line; do echo "$$line" | ./src/tc -d -c; done; \
	done

# generate all outputs
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

diff05:
	for f in 01 02 03 04 05 06 07 08 09 10 11 12 13 14; do \
		echo "size = $$f ---------------------------";\
		ALGO=C;\
		zcmp output_expected/$$f.txt.bz2 output/test05_$${ALGO}_size$$f.txt.bz2;\
	done

test05c: src/$(TARGET)
	for f in 10; do \
		echo "size = $$f ---------------------------";\
		for algo in c m n; do \
			ALGO=$$(echo $$algo | tr 'a-z' 'A-Z'); \
			echo "Processing $$f by Algorithm $${ALGO}";\
			time sh -c "bzcat input/all_$$f.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc -$${algo}t; done > output/test05_$${ALGO}_size$$f.txt";\
			bzip2 -c output/test05_$${ALGO}_size$$f.txt > output/test05_$${ALGO}_size$$f.txt.bz2;\
			rm -f output/test05_$${ALGO}_size$$f.txt;\
			zcmp output_expected/$$f.txt.bz2 output/test05_$${ALGO}_size$$f.txt.bz2;\
		done;\
	done

cp_test05: src/tc_c
	for f in 01 02 03 04 05 06 07 08 09 10 11 12 13 14; do \
		echo "size = $$f ---------------------------";\
		ALGO=CP; \
		echo "Processing $$f by Algorithm $${ALGO}";\
		time sh -c "bzcat input/all_$$f.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc_c -ct; done > output/test05_$${ALGO}_size$$f.txt";\
		bzip2 -c output/test05_$${ALGO}_size$$f.txt > output/test05_$${ALGO}_size$$f.txt.bz2;\
		rm -f output/test05_$${ALGO}_size$$f.txt;\
		zcmp output_expected/$$f.txt.bz2 output/test05_$${ALGO}_size$$f.txt.bz2;\
	done

cp_diff06:
	for f in 01 02 03 04 05 06 07 08 09 10 11 12 13 14; do \
		echo "size = $$f ---------------------------";\
		ALGO=CP;\
		zcmp output_expected/$$f.txt.bz2 output/test05_$${ALGO}_size$$f.txt.bz2;\
	done

time_cp: src/tc_c
	f=11; ALGO=CP; \
	sudo sync; sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches';\
	echo "====Processing $$f by Algorithm None (mesuring IO time)";\
	time sh -c "bzcat input/all_$${f}.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc_c -x; done"; \
	sudo sync; sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches';\
	echo "====Processing $$f by Algorithm $${ALGO}";\
	time sh -c "bzcat input/all_$${f}.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc_c -c; done";

time_c: src/${TARGET}
	f=11; ALGO=C; \
	sudo sync; sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches';\
	echo "====Processing $$f by Algorithm None (mesuring IO time)";\
	time sh -c "bzcat input/all_$${f}.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc -x; done"; \
	sudo sync; sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches';\
	echo "====Processing $$f by Algorithm $${ALGO}";\
	time sh -c "bzcat input/all_$${f}.txt.bz2 | while read line; do echo \"\$$line\" | ./src/tc -c; done";

.PHONEY: clean gen_inputs
