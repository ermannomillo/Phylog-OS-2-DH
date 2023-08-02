module UART_mutex(
		input wire CLK,
		input RST,
		input [15:0] in_op_node0,
		input [15:0] in_op_node1,
		input [7:0] in_peripheral,
		input [1:0] in_IRQ,
		output wire [7:0] out_peripheral,
		output wire [15:0] out_node,
		output wire rst_sig,
		output wire out_IRQ_node0,
		output wire out_IRQ_node1
		);
		
		// Function tag = 1011
		
		reg [1:0] lock = 3;
		reg [1:0] next_lock;
		
		localparam [15:0] stop_sequence  = 64256;  // 1111 tag 1111 1111
		localparam [15:0] start_sequence = 64511; // 1111 tag 0000 prty	
		
		localparam [7:0] UART_sequence_IRQ = 78;
		
		reg [7:0] out_driver;
		reg [15:0] out_synchro;
		reg out_RST;
		
		reg IRQ_node0;
		reg IRQ_node1;
		
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
		
	always @* begin : Combinational
        case (lock)
            2'b00: next_lock = (in_op_node0 == stop_sequence) ? 2'b11 : 2'b00;

            2'b01: next_lock = (in_op_node1 == stop_sequence) ? 2'b11 : 2'b01;

            2'b11: begin
                if (in_IRQ == 0) begin
                    if ((((in_op_node0 ^ start_sequence) <= 15) && ((in_op_node0 ^ start_sequence) != 0)) &&
                        (((in_op_node1 ^ start_sequence) <= 15) && ((in_op_node1 ^ start_sequence) != 0))) begin
                        next_lock = (in_op_node0 ^ start_sequence) >= (in_op_node1 ^ start_sequence) ? 2'b00 : 2'b01;
                    end else if ((in_op_node0 ^ start_sequence) <= 15 && (in_op_node0 ^ start_sequence) != 0) begin
                        next_lock = 2'b00;
                    end else if ((in_op_node1 ^ start_sequence) <= 15 && (in_op_node1 ^ start_sequence) > 0) begin
                        next_lock = 2'b01;
                    end else begin
                        next_lock = 2'b11;
                    end
                end else begin
                    next_lock = in_IRQ;
                end
				end

            default: next_lock = 2'b11;
        endcase
    end

    always @(posedge CLK or posedge RST) begin
        if (RST) begin
            // RST the FSM and other registers to their initial values
            lock <= 2'b11;
            out_driver <= 8'b0;
            out_synchro <= 16'b0;
            out_RST <= 1;
            IRQ_node0 <= 0;
            IRQ_node1 <= 0;
			end else begin 
			  case (lock)
					2'b00: begin
						 if ((in_op_node0 ^ start_sequence) <= 15) begin
							  out_driver <= 8'b0;
							  out_synchro <= 16'b0;
							  out_RST <= 1;
							  IRQ_node0 <= 0;
							  IRQ_node1 <= 0;
						 end else begin
							  if (in_op_node0 != 0) begin
									out_driver <= in_op_node0;
									out_synchro <= {8'b00000001, in_peripheral};
									IRQ_node0 <= (in_peripheral == UART_sequence_IRQ) ? 1 : 0;
							  end else begin
									IRQ_node0 <= 0;
									IRQ_node1 <= 0;
							  end
							  out_RST <= 1;
						 end
					end

					2'b01: begin
						 if ((in_op_node1 ^ start_sequence) <= 15) begin
							  out_driver <= 8'b0;
							  out_synchro <= 16'b0;
							  out_RST <= 1;
							  IRQ_node0 <= 0;
							  IRQ_node1 <= 0;
						 end else begin
							  if (in_op_node1 != 0) begin
									out_driver <= in_op_node1;
									out_synchro <= {8'b00000010, in_peripheral};
									IRQ_node1 <= (in_peripheral == UART_sequence_IRQ) ? 1 : 0;
							  end else begin
									IRQ_node0 <= 0;
									IRQ_node1 <= 0;
							  end
							  out_RST <= 1;
						 end
						 
					end

					2'b11 :   begin
						 out_driver <= 8'b0;
						 out_synchro <= 16'b0;
						 lock <= next_lock;
						 out_RST <= 0;
						 IRQ_node0 <= 0;
						 IRQ_node1 <= 0;
						 
					end

					default: begin
						 out_driver <= 8'b0;
						 out_synchro <= 16'b0;
						 lock <= next_lock;
						 out_RST <= 0;
						 IRQ_node0 <= 0;
						 IRQ_node1 <= 0;
					end
			  endcase
			end
    end

    assign out_peripheral = out_driver;
    assign out_node = out_synchro;
    assign rst_sig = out_RST;
    assign out_IRQ_node0 = IRQ_node0;
    assign out_IRQ_node1 = IRQ_node1;

endmodule
	