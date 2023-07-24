	module bitonic_sorter(
		 
		input [7:0] a1,a2,a3,a4,a5,a6,
		output wire [7:0] max
		
		);
		
		wire [7:0] mxx1,mxx2,mxx3,mxx4,mx1,mx2,mx3,mx4,mx5,mx6,mx7,mx8,mx9,mx10,mx11,mx12,mx13,mx14,mx15,mx16;
		wire [7:0] mnn1,mnn2,mnn3,mnn4,mn1,mn2,mn3,mn4,mn5,mn6,mn7,mn8,mn9,mn10,mn11,mn12,mn13,mn14,mn15,mn16;
		wire [7:0] id_plus_prty0, id_plus_prty1;
		wire [7:0] a7 = 0, a8 = 0;
		
		reg [7:0] out;

		// 1st level
		BN m21(a1,a2,mnn1,mxx1);
		BN1 m22(a3,a4,mnn2,mxx2);
		BN m23(a5,a6,mnn3,mxx3);
		BN1 m24(a7,a8,mnn4,mxx4);
		
		// 2nd level
		BN m1(mnn1,mnn2,mn1,mx1);
		BN m2(mxx1,mxx2,mn2,mx2);
		BN1 m3(mnn3,mnn4,mn3,mx3);
		BN1 m4(mxx3,mxx4,mn4,mx4);
		
		// 3rd level
		BN m5(mn1,mn2,mn5,mx5);
		BN m6(mx1,mx2,mn6,mx6);
		BN1 m7(mn3,mn4,mn7,mx7);
		BN1 m8(mx3,mx4,mn8,mx8);

		// 4th level
		BN m9(mn5,mn7,mn9,mx9);
		BN m10(mx5,mx7,mn10,mx10);
		BN m11(mn6,mn8,mn11,mx11);
		BN m12(mx6,mx8,mn12,mx12);

		// 5th level
		BN m15(mx9,mx11,mn15,mx15);
		BN m16(mx10,mx12,mn16,mx16);

		// 6th level
		BN m20(mx15,mx16,id_plus_prty0,id_plus_prty1);
		
		always @(*) begin
			if((id_plus_prty1 & 8'b11110000 ) != 0) begin 
			
				out <= id_plus_prty1 & 8'b00001111;
				
			end else begin 
			
				out <= 0;
			
			end
		end
		
	
		assign max = out;
		
	endmodule

	module BN1(
		input [7:0] A,B,
		output [7:0] max,min
		);
		
		wire LT1,GT1,EQ1;
		comp16 m1(A,B,LT1,GT1,EQ1);
		mux16 mx1(A,B,LT1,max);
		mux16 mx2(B,A,LT1,min);
	endmodule

	module BN(A,B,max,min
		 );
		input [7:0] A,B;
		output [7:0] max,min;
		wire LT1,GT1,EQ1;
		comp16 m1(A,B,LT1,GT1,EQ1);
		mux16 mx1(A,B,LT1,min);
		mux16 mx2(B,A,LT1,max);
	endmodule

	module mux16(

		 input [7:0] A,B,
		 input sel,
		 output [7:0] Y
		);

		assign Y = (sel)? B : A;
	endmodule

	module comp16(
		 input [7:0] A1,B1,
		 output reg LT1,GT1,EQ1
		 );
		 
		 always @ (A1,B1) begin
			if (A1>B1) begin
				LT1 <= 0; GT1 <= 1; EQ1 <= 0;
			end
			else if (A1<B1) begin
				LT1 <= 1; GT1 <= 0; EQ1 <= 0;
			end
			else begin
				LT1 <= 0; GT1 <= 0; EQ1 <= 1;
			end
		 end
	endmodule