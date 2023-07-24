module task2(
    input CLK,
	input [15:0] in_op_node0,
    input [15:0] in_op_node1,
    output wire [7:0] out_sorter_node0,
    output wire [7:0] out_sorter_node1
    );

    // Initial values
    reg [1:0] state_node0 = 2'b00;
    reg [1:0] next_state_node0;
    reg [3:0] priority_node0 = 4'b0000;

    reg [1:0] state_node1 = 2'b00;
    reg [1:0] next_state_node1;
    reg [3:0] priority_node1 = 4'b0000;

    reg [7:0] exe_hit = 8'b10000000;
    reg [7:0] next_exe_hit;

    reg [7:0] id_plus_prty_node0;
    reg [7:0] id_plus_prty_node1;

    // States
    // ---------------------
    // 00 | Ready
    // 01 | Suspended
    // 10 | Wait
    // 11 | Terminated
    // ---------------------

    reg [31:0] r_counter_node0 = 32'b0;
    reg [31:0] r_counter_node1 = 32'b0;

    localparam task_id = 8'b00000011;

    always @(posedge CLK) begin
        r_counter_node0 <= r_counter_node0 + 1;

        if (r_counter_node0 == 10000) begin
            r_counter_node0 <= 0;
            //next_state_node0 <= state_node0;
            //next_priority_node0 <= priority_node0 + 1;
        end
    end

    always @(posedge CLK) begin
        r_counter_node1 <= r_counter_node1 + 1;

        if (r_counter_node1 == 10000) begin
            r_counter_node1 <= 0;
            //next_state_node1 <= state_node1;
            //next_priority_node1 <= priority_node1 + 1;
        end
    end

    always @(*) begin
	 
	 
            // Operation cases
            // ---------------------
            // 1111 | Confirm execution
            // 0001 | Ready                       
            // 0010 | Suspend                     
            // 0011 | Wait                      
            // 0100 | Kill                    
            // 0101 | Increase priority           
            // 0110 | Increase execution hit
            // 1100 | Kill overall
            // 0000 | Not defined
            // ---------------------
		if (state_node0 != 2'b11) begin

            case (in_op_node0 & 16'b0000111111110000)
                // ---------------------
                // Particular State change
                // ---------------------
                16'b0000001100010000: next_state_node0 <= 2'b00; // Ready : 0001
                16'b0000001100100000: next_state_node0 <= 2'b01; // Suspend : 0010
                16'b0000001100110000: next_state_node0 <= 2'b10; // Wait : 0011
                16'b0000001101000000: next_state_node0 <= 2'b11; // Kill : 0100

                // ---------------------
                16'b0000001101010000: priority_node0 <= in_op_node0[3:0]; // Increase priority : 0101
                16'b0000001101100000: next_exe_hit <= in_op_node0[3:0]; // Increase execution hit : 0110

                16'b0000001101110000: begin // Execute
                    if (state_node0 == 2'b00 && exe_hit > 0) begin
                        next_exe_hit <= exe_hit - 1;
                        r_counter_node0 <= 0;
                    end
                end

                16'b0000001111000000: begin // Kill : 1100
                    next_state_node0 <= 2'b11;
						  //next_state_node1 <= 2'b11;
                end

                default: next_state_node0 <= state_node0;
            endcase
			end
    end

    always @(*) begin
		
            // Operation cases
            // ---------------------
            // 1111 | Confirm execution
            // 0001 | Ready                       
            // 0010 | Suspend  
            // 0011 | Wait    
            // 0100 | Kill  
            // 0101 | Increase priority 
            // 0110 | Increase execution hit
            // 1100 | Kill overall    
            // 0000 | Not defined
            // ---------------------
		if (state_node1 != 2'b11) begin
            case (in_op_node1 & 16'b0000111111110000)
                // ---------------------
                // Particular State change
                // ---------------------

                16'b0000001100010000: next_state_node1 <= 2'b00; // Ready : 0001
                16'b0000001100100000: next_state_node1 <= 2'b01; // Suspend : 0010
                16'b0000001100110000: next_state_node1 <= 2'b10; // Wait : 0011
                16'b0000001101000000: next_state_node1 <= 2'b11; // Kill : 0100

                // ---------------------
                16'b0000001101010000: priority_node1 <= in_op_node1[3:0]; // Increase priority : 0101
                16'b0000001101100000: next_exe_hit <= in_op_node1[3:0]; // Increase execution hit : 0110

                16'b0000001101110000: begin // Execute
                    if (state_node1 == 2'b00 && exe_hit > 0) begin
                        next_exe_hit <= exe_hit - 1;
                        r_counter_node1 <= 0;
                    end
                end

                16'b0000001111000000: begin // Kill : 1100
							next_state_node1 <= 2'b11;
							//next_state_node0 <= 2'b11;
                end

                default: next_state_node1 <= state_node1;
            endcase
			end

    end

    always @(posedge CLK) begin
        if (state_node0 == 2'b00) begin
            id_plus_prty_node0 <= {priority_node0, task_id};
        end else begin
            id_plus_prty_node0 <= 8'b00000000;
        end
        state_node0 <= next_state_node0;
    end

    always @(posedge CLK) begin
        if (state_node1 == 2'b00) begin
            id_plus_prty_node1 <= {priority_node1, task_id};
        end else begin
            id_plus_prty_node1 <= 8'b00000000;
        end
        state_node1 <= next_state_node1;
    end

    always @(posedge CLK) begin
        exe_hit <= next_exe_hit;
    end

    assign out_sorter_node0 = id_plus_prty_node0;
    assign out_sorter_node1 = id_plus_prty_node1;

endmodule
