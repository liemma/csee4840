// CSEE 4840 Lab 1: Run and Display Collatz Conjecture Iteration Counts
//
// Spring 2023
//
// By: <your name here>
// Uni: <your uni here>

module lab1( input logic        CLOCK_50,  // 50 MHz Clock input
	     
	     input logic [3:0] 	KEY, // Pushbuttons; KEY[0] is rightmost

	     input logic [9:0] 	SW, // Switches; SW[0] is rightmost

	     // 7-segment LED displays; HEX0 is rightmost
	     output logic [6:0] HEX0, HEX1, HEX2, HEX3, HEX4, HEX5,

	     output logic [9:0] LEDR // LEDs above the switches; LED[0] on right
	     );

	logic 			clk, go, done;   
	logic [31:0] 		start;
	logic [15:0] 		count;
	logic [23:0]		counter10M = 0;
	logic [11:0] 		n = 0;
	logic			inc, dec, restore;
	logic [31:0]		n_display;
	assign clk = CLOCK_50;

	range #(256, 8) // RAM_WORDS = 256, RAM_ADDR_BITS = 8)
	      r ( .* ); // Connect everything with matching names

	// Replace this comment and the code below it with your own code;
	// The code below is merely to suppress Verilator lint warnings
	always @(posedge clk)
	begin
		if (go) 
		begin
			start[9:0] <= SW[9:0];
			n[9:0] <= SW[9:0];
			LEDR <= 10'b0;
		end else begin
			if (!done)
			begin
				start <= 0;
			end else begin
				LEDR <= 10'b1111111111;
 				if (restore)
				begin
					/*if (SW[9:0] < n)
						start <= 0;
					else if (SW[9:0] > n + 255)
						start <= 255;
					else start <= SW[9:0] - n;*/
					start <= 0;
					counter10M <= 0;
				end
				else if (inc)
				begin
					
					if (counter10M == 10000000)
						counter10M <= 0;
					else counter10M <= counter10M + 24'b1;
					if (counter10M == 0 && start < 256)
						start <= start + 1;
				end
				else if (dec)
				begin
					if (counter10M == 10000000)
						counter10M <= 0;
					else counter10M <= counter10M + 24'b1;
					if (counter10M == 0 && start > 0)
						start <= start - 1;
				end
				else counter10M <= 0;
			end
		end
	end
	assign go = !KEY[3] & (&KEY[2:0]);
	assign inc = !KEY[0];
	assign dec = !KEY[1];
	assign restore = !KEY[2];
	hex7seg trans0(count[3:0], HEX0[6:0]);
	hex7seg trans1(count[7:4], HEX1[6:0]);
	hex7seg trans2(count[11:8], HEX2[6:0]);
	hex7seg trans3(n_display[3:0], HEX3[6:0]);
	hex7seg trans4(n_display[7:4], HEX4[6:0]);
	hex7seg trans5(n_display[11:8], HEX5[6:0]);
	assign n_display = n + start;

endmodule
