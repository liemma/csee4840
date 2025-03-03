module hex7seg(input logic [3:0] a, 
	       output logic [6:0] y);
	//comment gfedcba
	always_comb
		case(a)
			4'b0000: y=7'b0111111;
			4'b0001: y=7'b0000110;
			4'b0010: y=7'b1011011;
			4'b0011: y=7'b1001111;
			4'b0100: y=7'b0100110;
			4'b0101: y=7'1101101;
			4'b0110: y=7'1111101;
			4'b0111: y=7'0000111;
			4'b1000: y=7'1111111;
			4'b1001: y=7'1101111;
