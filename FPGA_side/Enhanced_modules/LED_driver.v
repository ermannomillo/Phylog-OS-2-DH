module LED_driver(
		input [7:0] in,
		output reg led0,
		output reg led1,
		output reg led2,
		output reg led3,
		output reg led4,
		output reg led5,
		output reg led6,
		output reg led7	
		);
		
		always @(in) begin
			if(in & 8'b00000001) begin
				led0 <= 1;
			end
			else begin
				led0 <= 0;
			end
			
			if(in & 8'b00000010) begin
				led1 <= 1;
			end
			else begin
				led1 <= 0;
			end
			
			if(in & 8'b00000100) begin
				led2 <= 1;
			end
			else begin
				led2 <= 0;
			end
			
			if(in & 8'b00001000) begin
				led3 <= 1;
			end
			else begin
				led3 <= 0;
			end
			
			if(in & 8'b00010000) begin
				led4 <= 1;
			end
			else begin
				led4 <= 0;
			end
			
			if(in & 8'b00100000) begin
				led5 <= 1;
			end
			else begin
				led5 <= 0;
			end
			
			if(in & 8'b01000000) begin
				led6 <= 1;
			end
			else begin
				led6 <= 0;
			end
			
			if(in & 8'b10000000) begin
				led7 <= 1;
			end
			else begin
				led7 <= 0;
			end
		
		end

endmodule