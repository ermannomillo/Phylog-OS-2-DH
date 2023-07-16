module UART_mutex(
		input wire CLK,
		input [15:0] in_op_node0,
		input [15:0] in_op_node1,
		input [7:0] in_peripheral,
		input [1:0] in_IRQ,
		output wire [7:0] out_peripheral,
		output wire [15:0] out_node,
		output wire reset,
		output wire out_IRQ_node0,
		output wire out_IRQ_node1
		);
		
		// Function tag 1100
		
		reg [1:0] lock = 3;
		reg [1:0] next_lock;
		
		localparam [15:0] stop_sequence = 64767;  // 1111 tag 1111 1111
		localparam [15:0] start_sequence = 64512; // 1111 tag 0000 prty	
		
		localparam [7:0] UART_sequence_IRQ = 78;
		
		reg [7:0] out_driver;
		reg [15:0] out_synchro;
		reg out_reset;
		
		reg IRQ_node0;
		reg IRQ_node1;
		
		// --------------------------
		//  Recency bit for request
		// --------------------------
		//  00 : most recent 
		//  11 : access not requested
		// --------------------------
		
		// --------------------------
		//  Lock
		// --------------------------
		//  00 : locked to mcu 0
		//    ...
		//  11 : not locked
		// --------------------------
		
	always @* begin : Combinational
        case (lock)
            2'b00:
                next_lock = (input_op_node0 == stop_sequence) ? 2'b11 : 2'b00;

            2'b01:
                next_lock = (input_op_node1 == stop_sequence) ? 2'b11 : 2'b01;

            2'b11:
                if (in_IRQ == 0) begin
                    if ((((input_op_node0 ^ start_sequence) <= 15) && ((input_op_node0 ^ start_sequence) != 0)) &&
                        (((input_op_node1 ^ start_sequence) <= 15) && ((input_op_node1 ^ start_sequence) != 0))) begin
                        next_lock = (input_op_node0 ^ start_sequence) >= (input_op_node1 ^ start_sequence) ? 2'b00 : 2'b01;
                    end else if ((input_op_node0 ^ start_sequence) <= 15 && (input_op_node0 ^ start_sequence) != 0) begin
                        next_lock = 2'b00;
                    end else if ((input_op_node1 ^ start_sequence) <= 15 && (input_op_node1 ^ start_sequence) > 0) begin
                        next_lock = 2'b01;
                    end else begin
                        next_lock = 2'b11;
                    end
                end else begin
                    next_lock = in_IRQ;
                end

            default:
                next_lock = 2'b11;
        endcase
    end

    always @(posedge CLK) begin : Sequential
        case (lock)
            2'b00:
                if ((input_op_node0 ^ start_sequence) <= 15) begin
                    out_driver <= 8'b0;
                    out_synchro <= 16'b0;
                    out_reset <= 1;
                    IRQ_node0 <= 0;
                    IRQ_node1 <= 0;
                end else begin
                    if (input_op_node0 != 0) begin
                        out_driver <= input_op_node0;
                        out_synchro <= {8'b00000001, in_peripheral};
                        IRQ_node0 <= (in_peripheral == UART_sequence_IRQ) ? 1 : 0;
                    end else begin
                        IRQ_node0 <= 0;
                        IRQ_node1 <= 0;
                    end
                    out_reset <= 1;
                end

            2'b01:
                if ((input_op_node1 ^ start_sequence) <= 15) begin
                    out_driver <= 8'b0;
                    out_synchro <= 16'b0;
                    out_reset <= 1;
                    IRQ_node0 <= 0;
                    IRQ_node1 <= 0;
                end else begin
                    if (input_op_node1 != 0) begin
                        out_driver <= input_op_node1;
                        out_synchro <= {8'b00000010, in_peripheral};
                        IRQ_node1 <= (in_peripheral == UART_sequence_IRQ) ? 1 : 0;
                    end else begin
                        IRQ_node0 <= 0;
                        IRQ_node1 <= 0;
                    end
                    out_reset <= 1;
                end

            2'b11:
                lock <= next_lock;
                out_driver <= 8'b0;
                out_synchro <= 16'b0;
                out_reset <= 0;
                IRQ_node0 <= 0;
                IRQ_node1 <= 0;

            default:
                lock <= next_lock;
                out_driver <= 8'b0;
                out_synchro <= 16'b0;
                out_reset <= 0;
                IRQ_node0 <= 0;
                IRQ_node1 <= 0;
        endcase
    end

    assign out_peripheral = out_driver;
    assign out_node = out_synchro;
    assign reset = out_reset;
    assign out_IRQ_node0 = IRQ_node0;
    assign out_IRQ_node1 = IRQ_node1;

endmodule
	