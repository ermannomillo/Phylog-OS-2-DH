
module LED_mutex(
		input  wire CLK,
		input RST,
		input [15:0] in_op_node0,
		input [15:0] in_op_node1,
		output wire [7:0] out_peripheral
		);
		
		// Function Tag = 1010
		
		reg [1:0] lock = 3;
		reg [1:0] next_lock;

		localparam stop_sequence  = 64255; // 1111 tag 1111 1111 
		localparam start_sequence = 64000; // 1111 tag 0000 prty

		reg [7:0] out_driver;
		
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
		//  00 : locked to node 0
		//    ...
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
            // RST the FSM and other registers to their initial values
            lock <= 2'b11;
            out_driver <= 0;
				
			end else begin 
				case(lock) 
					
						2'b00 : begin 	 
							if((in_op_node0 ^ start_sequence) <= 15) begin
								out_driver <= 0;
								lock <= next_lock;
							end
							else begin
								if(in_op_node0 != 0) begin
									out_driver <= in_op_node0;
								end
								lock <= next_lock;
							end
						end
						
						2'b01 : begin 
							if((in_op_node1 ^ start_sequence) <= 15) begin
								out_driver <= 0;
								lock <= next_lock;
							end
							else begin
								if(in_op_node1 != 0) begin
									out_driver <= in_op_node1;
								end
								lock <= next_lock;
							end
						end	
						2'b11 : begin 
							out_driver <= 0;
							lock <= next_lock;
						end
						
						default : begin
							lock <= next_lock;
						end
					
				endcase
			end
		end
		
		assign out_peripheral = out_driver;
	
endmodule
