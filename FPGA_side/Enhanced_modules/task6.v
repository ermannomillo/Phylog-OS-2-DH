module task6(
    input CLK,
    input [15:0] in_op,
    output wire [7:0] out_sorter
    );

    // Initial values

    reg [1:0] state = 2'b00;
    reg [1:0] next_state;
    reg [7:0] task_priority = 8'b00000000;
    reg [7:0] next_priority = 8'b00000000;
    reg [7:0] exe_hit = 8'b10000000;
    reg [7:0] next_exe_hit;
    reg [7:0] id_plus_prty;
    reg [31:0] r_counter = 32'b0;

    localparam task_id = 8'b00000111;

    // States
    // ---------------------
    // 00 | Ready
    // 01 | Suspended
    // 10 | Wait
    // 11 | Terminated
    // ---------------------

    always @(posedge CLK) begin
        r_counter <= r_counter + 1;

        if (r_counter == 10000) begin
            r_counter <= 0;
            //next_priority <= task_priority + 1;
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
        // 1100 | Kill                       
        // 0000 | Not defined
        // ---------------------

        case (in_op & 16'b0000111111110000)
            // ---------------------
            // Particular State change
            // ---------------------
            16'b0000011100010000: next_state = 2'b00; // Ready : 0001
            16'b0000011100100000: next_state = 2'b01; // Suspend : 0010
            16'b0000011100110000: next_state = 2'b10; // Wait : 0011
            16'b0000011101000000: next_state = 2'b11; // Kill : 0100
			
            // ---------------------------------------------------------
			
            16'b0000011101010000: next_priority = in_op[3:0]; // Increase priority : 0101
            16'b0000011101100000: next_exe_hit = in_op[3:0]; // Increase execution hit : 0110
			
            16'b0000011101110000: begin // Execute
                if (state == 2'b00 && exe_hit > 0) begin
                    next_exe_hit = exe_hit - 1;
                    r_counter <= 0;
                end
            end
			
            16'b0000011111110000: begin // Finish execution
                if (state == 2'b00 && exe_hit > 0) begin
                    next_exe_hit = exe_hit - 1;
                    r_counter <= 0;
                end
            end
			
            default: begin
                next_state = state;
                next_priority = task_priority;
                next_exe_hit = exe_hit;
            end
        endcase
    end

    always @(posedge CLK) begin
        if (state == 2'b00) begin
            id_plus_prty = {task_id, task_priority};
        end else begin
            id_plus_prty = 8'b00000000;
        end
        state <= next_state;
    end

    always @(posedge CLK) begin
        task_priority <= next_priority;
    end

    always @(posedge CLK) begin
        exe_hit <= next_exe_hit;
    end

    assign out_sorter = id_plus_prty;

endmodule
