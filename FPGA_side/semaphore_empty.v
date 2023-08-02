
module semaphore_empty(
		input  wire CLK,
		input RST,
		input [15:0] in_op_node0,
		input [15:0] in_op_node1,
		output wire [15:0] out
		);
		
		// Function Tag = 1101
		
		reg [1:0] lock = 3;
		reg [1:0] next_lock;

		localparam stop_sequence  = 16'b1111110111111111; // 1101 tag 1111 1111 : 65023
		localparam start_sequence = 16'b1111110100000000; // 1101 tag 0000 prty : 64772

		reg [15:0] out_node;
		reg [31:0] coins = 0;
		
		// --------------------------
		//  Request priority
		// --------------------------
		//  1111 : highest 
		//   ...
		//
		//  0000 : access not requested
		// --------------------------
		
		// --------------------------
		//  Lock
		// --------------------------
		//  00 : locked to mcu 0
		//  01 : locked to mcu 1
		//  10 : not defined
		//  11 : not locked
		// --------------------------
		
		always @(*) begin : Combinational 
			
			case (lock)
			
			2'b00 : begin
				if(in_op_node0 == stop_sequence) begin
					next_lock <= 3;
				end
				else begin
					next_lock <= 0;
				end
			end
			
			2'b01 : begin
				if(in_op_node1 == stop_sequence) begin
					next_lock <= 3;
				end
				else begin
					next_lock = 1;
				end
			end
			
			2'b11 : begin
				if((((in_op_node0 ^ start_sequence) <= 15) && ((in_op_node0 ^ start_sequence) != 0)) && (((in_op_node1 ^ start_sequence) <= 15) && ((in_op_node1 ^ start_sequence) != 0))) begin
					if((in_op_node0 ^ start_sequence) >= (in_op_node1 ^ start_sequence)) begin
						next_lock <= 0;
					end
					else begin
						next_lock <= 1;
					end
				end
				else if(((in_op_node0 ^ start_sequence) <= 15) && ((in_op_node0 ^ start_sequence) != 0)) begin
					next_lock <= 0;
				end
				else if(((in_op_node1 ^ start_sequence) <= 15) && ((in_op_node1 ^ start_sequence) > 0)) begin
					next_lock <= 1;
				end
				else begin
					next_lock <= 3;
				end
			end			
			
			default : begin 
				next_lock = 3;	
			end
			
			endcase
		end
		
		 always @(posedge CLK or posedge RST) begin
        if (RST) begin
            // Initialize all the registers to their initial values
            lock <= 2'b11;        // Set to "not locked"
            out_node <= 16'b0;    // Set output to zero during RST
            coins <= 0;           // RST the coin count
        end else begin
				case(lock) 
				
					2'b00 : begin 	 
						if((in_op_node0 ^ start_sequence) <= 15) begin
							out_node <= 0;
							lock <= next_lock;
						end
						else if(in_op_node0 == 16'b0000110100010000)  begin // Incoming post
							if(coins != 10) begin
								coins <= coins + 1;
								out_node <= 16'b0000110100000001;
							end
							else begin
								out_node <= 16'b0001110100000001; // Set full flag
							end
							lock <= next_lock;
						end
						else if(in_op_node0 == 16'b0000110100100000)  begin // Incoming wait
							if(coins != 32'b0) begin
								coins <= coins - 1;
								out_node <= 16'b0000110100000001;
							end
							else begin
								out_node <= 16'b0010110100000001; // Set empty flag
							end
							lock <= next_lock;
						end
					end
					
					2'b01 : begin 
						if((in_op_node1 ^ start_sequence) <= 15) begin
							out_node <= 0;
							lock <= next_lock;
						end
						else if(in_op_node1 == 16'b0000110100010000)  begin // incoming post
							if(coins != 10) begin
								coins <= coins + 1;
								out_node <= 16'b0000110100000010;
							end
							else begin
								out_node <= 16'b0001110100000010; // Set full flag
							end
							lock <= next_lock;
						end
						else if(in_op_node1 == 16'b0000110100100000)  begin // incoming post
							if(coins != 32'b0) begin
								coins <= coins - 1;
								out_node <= 16'b0000110100000010;
							end
							else begin
								out_node <= 16'b0010110100000010; // Set empty flag
							end
							lock <= next_lock;
						end
					end	
					2'b11 : begin 
						out_node <= 0;
						lock <= next_lock;
					end
					
					default : begin
						out_node <= 0;
						lock <= next_lock;
					end
				
				endcase
			end
		end
		
		assign out = out_node;
	
endmodule
