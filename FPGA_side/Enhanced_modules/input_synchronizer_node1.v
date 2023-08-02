module input_synchronizer_node1 (
	input [15:0] idx_op,
	output reg rst_sig,
	output reg [15:0] ESPIC_op,
	output reg [15:0] task0_op,
	output reg [15:0] task1_op,
	output reg [15:0] task2_op,
	output reg [15:0] task3_op,
	output reg [15:0] task4_op,
	output reg [15:0] task5_op,
	output reg [15:0] peripheral0,
	output reg [15:0] peripheral1,
	output reg [15:0] peripheral2,
	output reg [15:0] peripheral3,
	output reg [15:0] peripheral4
	);
	
		
	// ---------------------
	// Task ID | Operation
	// [7:4]   | [3:0]
	// ---------------------
	
	
	always @(*) begin : Combinational
			
				task0_op <= 0;
				task1_op <= 0;
				task2_op <= 0;
				task3_op <= 0;
				task4_op <= 0;
				task5_op <= 0;
				peripheral0 <= 0;
				peripheral1 <= 0;
				peripheral2 <= 0;
				peripheral3 <= 0;
				peripheral4 <= 0;
				ESPIC_op <= 0;
				rst_sig <= 1;
	
				case (idx_op & 16'b0000111100000000)
					
					16'b0000111100000000 : begin // For security reason, mantain 0 reserved maybe conf
								ESPIC_op <= idx_op;
								if(idx_op == 16'b1) begin 
									rst_sig <= 1;
								end else begin
									rst_sig <= 0;
								end
							end
					16'b0000000100000000 : begin // 0001
								task0_op <= idx_op;
							end
					16'b0000001000000000 : begin // 0010
								task1_op <= idx_op;
							end
					16'b0000001100000000 : begin // 0011
								task2_op <= idx_op;
							end
			// ------------------------------------------		
			//		EXCLUSIVE TASKS	
			// ------------------------------------------	
					16'b0000010000000000 : begin // 0100
								task3_op <= idx_op;
							end	
					16'b0000010100000000 : begin // 0101
								task4_op <= idx_op;
							end
					16'b0000011000000000 : begin // 0110
								task5_op <= idx_op;
							end
			// ------------------------------------------		
			//		PERIPHERALS	
			// ------------------------------------------
					16'b0000101000000000 : begin // 1010
								peripheral0 <= idx_op;
								end
								
					16'b0000101100000000 : begin // 1011
								peripheral1 <= idx_op;
								end
								
					16'b0000110000000000 : begin // 1100
								peripheral2 <= idx_op;
								end
					16'b0000110100000000 : begin // 1011
								peripheral3 <= idx_op;
								end
								
					16'b0000111000000000 : begin // 1100
								peripheral4 <= idx_op;
								end
					default : begin 
						task0_op <= 0;
						task1_op <= 0;
						task2_op <= 0;
						task3_op <= 0;
						task4_op <= 0;
						task5_op <= 0;
						peripheral0 <= 0;
						peripheral1 <= 0;
						peripheral2 <= 0;
						peripheral3 <= 0;
						peripheral4 <= 0;
						ESPIC_op <= 0;
						rst_sig <= 0;
					end
		
				endcase
	end

endmodule