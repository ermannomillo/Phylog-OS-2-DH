module task3(
    input CLK,
	 input RST,
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
	 reg [31:0] next_r_counter = 32'b0;

    localparam task_id = 8'b00000100;

    // States
    // ---------------------
    // 00 | Ready
    // 01 | Suspended
    // 10 | Wait
    // 11 | Terminated
    // ---------------------



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
            16'b0000010000010000: next_state = 2'b00; // Ready : 0001
            16'b0000010000100000: next_state = 2'b01; // Suspend : 0010
            16'b0000010000110000: next_state = 2'b10; // Wait : 0011
            16'b0000010001000000: next_state = 2'b11; // Kill : 0100
			
            // ---------------------------------------------------------
			
            16'b0000010001010000: next_priority = in_op[3:0]; // Increase priority : 0101
            16'b0000010001100000: next_exe_hit = in_op[3:0]; // Increase execution hit : 0110
			
            16'b0000010001110000: begin // Execute
                if (state == 2'b00 && exe_hit > 0) begin
                    next_exe_hit = exe_hit - 1;
                    next_r_counter <= 0;
                end
            end
			
            16'b0000010011110000: begin // Finish execution
                if (state == 2'b00 && exe_hit > 0) begin
                    next_exe_hit = exe_hit - 1;
                    next_r_counter <= 0;
                end
            end
			
            default: begin
                next_state = state;
                next_priority = task_priority;
                next_exe_hit = exe_hit;
					 next_r_counter = r_counter;
            end
        endcase
    end

    always @(posedge CLK or posedge RST) begin
        if (RST) begin
            // RST the FSM to the initial state and values
            state <= 2'b00;
            task_priority <= 8'b00000000;
            exe_hit <= 8'b10000000;
        end else begin 
			  if (state == 2'b00) begin
					id_plus_prty = {task_id, task_priority};
			  end else begin
					id_plus_prty = 8'b00000000;
			  end
			  if (next_r_counter == 10000) begin
					r_counter <= 0;
					task_priority <= next_priority;
			  end else if (next_r_counter == 0) begin
					r_counter <= 0;
					task_priority <= next_priority + 1;	
			  end
			  state <= next_state;		  
			  exe_hit <= next_exe_hit;
			  r_counter <= r_counter + 1;
		  end
    end

    assign out_sorter = id_plus_prty;

endmodule
