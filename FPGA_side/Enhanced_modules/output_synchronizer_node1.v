module output_syncronizer_node1 (
	input [7:0] next_task,
	input [15:0] peripheral0,
	input [15:0] peripheral1,
	input [15:0] peripheral2,
	input [15:0] peripheral3,
	output wire [15:0] out
	);
		
	reg [15:0] out_node;
		
	// ------------------
	// state = 1 ready
	// state = 0 wait
	// ------------------
		
	// ------------------
	//  Op   |  Task id
	// [5:3] | [2:0]
	// ------------------
	
	always @(*) begin : Combinational
		if((peripheral1 & 16'b0000111100000000) == 16'b0000001000000000) begin 
			out_node <= peripheral1;
		end
		else if ((peripheral0 & 16'b0000111100000000) == 16'b0000001000000000) begin 
			out_node <= peripheral0;
		end
		else if ((peripheral2 & 16'b0000111100000000) == 16'b0000001000000000) begin 
			out_node <= peripheral0;
		end
		else if ((peripheral3 & 16'b0000111100000000) == 16'b0000001000000000) begin 
			out_node <= peripheral0;
		end
		else begin
			out_node <= next_task;
		end
	end

assign out = out_node;
	
endmodule