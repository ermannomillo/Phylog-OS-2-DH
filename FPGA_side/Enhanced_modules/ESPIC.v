module ESPIC(
		input wire CLK,
		input wire ext_signal,
		input [15:0] in_op_node0,
		input [15:0] in_op_node1,
		input [1:0] exe_flag_task0,
		input [1:0] exe_flag_task1,
		output wire out_node_IRQ0,
		output wire [1:0] out_mutex_IRQ1,
		output wire out_IRQ2_node0,
		output wire out_IRQ2_node1
		);
		
		
		reg [2:0] priority_IRQ1_node0 = 2;
		reg [2:0] priority_IRQ1_node1 = 4;
		
		reg [31:0] counter_IRQ0;
		reg [31:0] counter_IRQ1;
		reg [31:0] counter_IRQ2_node0;
		reg [31:0] counter_IRQ2_node1;
		reg [1:0] next_out_IRQ1 = 0;
		reg next_out_IRQ2_0 = 0;
		reg next_out_IRQ2_1 = 0;
		
		initial counter_IRQ0       = 0;
		initial counter_IRQ1       = 0;
		initial counter_IRQ2_node0 = 0;
		initial counter_IRQ2_node1 = 0;
		
		reg out_IRQ0;
		reg out_IRQ1;
		reg out_IRQ2_0;
		reg out_IRQ2_1;
		
		

	// -----------------------------------------
	//
	// IRQ0 : Send a 1 ms signal every 5000 ms 
	//
	// -----------------------------------------
	
		always @(posedge CLK) begin
		
			if((counter_IRQ0 >= 999999900) && (counter_IRQ0 <= 1000000000) )  begin
				counter_IRQ0 <= counter_IRQ0 + 1;
				out_IRQ0 = 1;
			end
			else if(counter_IRQ0 <= 999999900) begin
				counter_IRQ0 <= counter_IRQ0 + 1;
				out_IRQ0 <= 0;		
			end 
			else begin
				counter_IRQ0 <= 0;
				out_IRQ0 <= 0;
			end		
			
		end 
		
		
	// -----------------------------------------
	//
	// IRQ1 : Scheduled external signal bound
	//
	// -----------------------------------------	
		
		always @(posedge ext_signal) begin
		
			if(priority_IRQ1_node0 >= priority_IRQ1_node1) begin
				next_out_IRQ1 <= 2'b01;
			end 
			else begin
				next_out_IRQ1 <= 2'b10;
			end
		
		end
		
		always @(CLK) begin 
		
			case(out_IRQ1) 
					
				2'b01 : begin 				
					if(counter_IRQ1 <= 1000) begin 
						out_IRQ1 <= next_out_IRQ1;
						counter_IRQ1 <= counter_IRQ1 + 1;
					end else begin
						out_IRQ1 <= 0;
						counter_IRQ1 <= 0;
					end
				end
					
				2'b10 : begin 
					
					if(counter_IRQ1 <= 1000) begin 
						out_IRQ1 <= next_out_IRQ1;
						counter_IRQ1 <= counter_IRQ1 + 1;
					end else begin
						out_IRQ1 <= 0;
						counter_IRQ1 <= 0;
					end
				end
				
				default : begin
					
					counter_IRQ1 <= 0;
					out_IRQ1 <= next_out_IRQ1;
						
				end
				
			endcase	
			
		end
		
		
		
		always @(*) begin 
		
			if((in_op_node0 & 16'b0011111111110000) == 16'b0010111100010000 )  begin
				priority_IRQ1_node0 <= in_op_node0 & 16'b0000000000001111;
			end
		
		end
		
		
		always @(*) begin 
		
			if((in_op_node1 & 16'b0011111111110000) == 16'b0010111100010000 )  begin
				priority_IRQ1_node1 <= in_op_node1 & 16'b0000000000001111;
			end
		
		end
	

		
	// -----------------------------------------
	//
	// IRQ2 : Command-rised memory bound
	//
	// -----------------------------------------
	
		always @(posedge CLK) begin
		
			case(in_op_node0)
			
				16'b0011111100010001 : begin 
					next_out_IRQ2_0 <= 1;
				end
				
				16'b0011111100010010 : begin 
					next_out_IRQ2_1 <= 1;
				end
				
			endcase

		
			case(in_op_node1)
			
				16'b0011111100010001 : begin 
					next_out_IRQ2_0 <= 1;
				end
				
				16'b0011111100010010 : begin 
					next_out_IRQ2_1 <= 1;
				end
				
			endcase
	
		end
		
		always @(CLK) begin 
		
			if(out_IRQ2_0) begin
				if(counter_IRQ2_node0 <= 1000) begin 
						out_IRQ2_0 <= 1;
						counter_IRQ2_node0 <= counter_IRQ2_node0 +1;
				end else begin
					out_IRQ2_0 <= 0;
					counter_IRQ2_node0 <= 0;
				end
			end
			else begin 
				out_IRQ2_0 <= next_out_IRQ2_0;
				counter_IRQ2_node1 <= 0;
			end

		
			if(out_IRQ2_1) begin
				if(counter_IRQ2_node1 <= 1000) begin 
						out_IRQ2_1 <= 1;
						counter_IRQ2_node1 <= counter_IRQ2_node0 +1;
				end else begin
					out_IRQ2_1 <= 0;
					counter_IRQ2_node1 <= 0;
				end
			end
			else begin 
				out_IRQ2_0 <= next_out_IRQ2_0;
				counter_IRQ2_node1 <= 0;
			end
		end

		assign out_node_IRQ0 = out_IRQ0;
		assign out_mutex_IRQ1 = out_IRQ1;
		assign out_IRQ2_node0= out_IRQ2_0;
		assign out_IRQ2_node1= out_IRQ2_1;
		
endmodule
