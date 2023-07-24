
module RAM_mutex(
		input wire CLK,
		input [15:0] in_op_node0,
		input [15:0] in_op_node1,
		output wire [15:0] out_node
		);
		
		// Function Tag = 1100
		
		reg [1:0] lock = 3;
		reg [1:0] next_lock;

		reg [15:0] stop_sequence  = 64767;  // 1111 tag 1111 1111
		reg [15:0] start_sequence = 64512; // 1111 tag 0000 prty
		
		reg [7:0] matrix [255:0];
		reg [7:0] frame_pointer = 0;
		reg [7:0] stack_pointer = 0;
		
		reg [7:0] next_frame_pointer = 0;
		reg [7:0] next_stack_pointer = 0;
		
		reg [15:0] out_synchro;
		
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
		
		// --------------------------------
		//  Main Functions
		// --------------------------------
		//  00 : Read                  : C
		//  01 : Write                 : R
		//	11 : Add new frame         : U
		//  10 : Garbage working frame : D
		// --------------------------------
	
		always @(*) begin 		
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
	
		always @(*) begin
			case(lock) 
				
					2'b00 : begin 	 
						if((in_op_node0 ^ start_sequence) <= 15) begin
	
						end
						else begin
							case (in_op_node0 & 16'b0011111100000000)	
								7168 : begin // XX01 1100 XXXX XXXX : Write
									matrix[stack_pointer] <= in_op_node0 & 16'b0000000011111111;
									next_stack_pointer <= stack_pointer + 1;
								end
							
								11264 : begin // XX10 1100 XXXX XXXX : Garbage working frame
									next_stack_pointer <= frame_pointer;
									next_frame_pointer <= matrix[frame_pointer];
								end
								
								15360 : begin // XX11 1100 XXXX XXXX : Add new frame
									matrix[stack_pointer] <= frame_pointer;
									next_stack_pointer <= stack_pointer + 1;
									next_frame_pointer <= stack_pointer;
								end
								
							endcase
						end
					end
					
					2'b01 : begin 
						if((in_op_node1 ^ start_sequence) <= 15) begin
				
						end
						else begin
							case (in_op_node1 & 16'b0011111100000000)
						
								7168 : begin // XX01 1100 XXXX XXXX : Write
									matrix[stack_pointer] <= in_op_node1 & 16'b0000000011111111;
									next_stack_pointer <= stack_pointer + 1;
									
								end
									
								11264 : begin // XX10 1100 XXXX XXXX : Garbage last frame
									next_stack_pointer <= frame_pointer;
									next_frame_pointer <= matrix[frame_pointer];

								end
								
								15360 : begin // XX11 1100 XXXX XXXX : Add new frame
									matrix[stack_pointer] <= frame_pointer;
									next_stack_pointer <= stack_pointer + 1;
									next_frame_pointer <= stack_pointer;
								end
								
							endcase
						end
					end	

					 default: begin
						next_stack_pointer <= stack_pointer;
						next_frame_pointer <= frame_pointer;
					 end
					
				endcase
		end
		

		always @(negedge CLK ) begin
			frame_pointer <= next_frame_pointer;
			stack_pointer <= next_stack_pointer;
		end

		always @(posedge CLK ) begin : Sequential
			case(lock) 
				
					2'b00 : begin 
						if((in_op_node0 ^ start_sequence) <= 15) begin
							out_synchro <= 16'b0000000100000000;
							lock <= next_lock;
						end
						else if((in_op_node0 & 16'b0011111100000000) == 3072) begin  // XX00 1100 XXXX XXXX : Read
							out_synchro <= { 8'b00000001, matrix[(in_op_node0 & 16'b0000000011111111) + frame_pointer]};
							lock <= next_lock;
						end
						else begin 
							out_synchro <= 16'b0000000100000000;
							lock <= next_lock;
						end
					end
					
					2'b01 : begin 
						if((in_op_node1 ^ start_sequence) <= 15) begin
							out_synchro <=  16'b0000001000000000;
							lock <= next_lock;
						end
						else if((in_op_node1 & 16'b0011111100000000) == 3072) begin  // XX00 1100 XXXX XXXX : Read
							out_synchro <= { 8'b00000010, matrix[(in_op_node0 & 16'b0000000011111111)+ frame_pointer]};
							lock <= next_lock;
						end
						else begin 
							out_synchro <= 16'b0000001000000000;
							lock <= next_lock;
						end
					end
					
					2'b11 : begin 
						out_synchro <= 16'b0;
						lock <= next_lock;
					end
					
					default : begin
						out_synchro <= 16'b0;
						lock <= next_lock;
					end
				
			endcase
		end
		
		assign out_node = out_synchro;
		
endmodule
