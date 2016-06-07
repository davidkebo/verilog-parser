# Verilog parser makefile

verilog-parser: verilog-parser.o functions.o
	gcc -o verilog-parser verilog-parser.o functions.o

verilog-parser.o: functions.h verilog-parser.c
	gcc -O -c verilog-parser.c

functions.o: functions.h functions.c
	gcc -O -c functions.c

clean:
	rm -f verilog-parser verilog-parser.o functions.o
