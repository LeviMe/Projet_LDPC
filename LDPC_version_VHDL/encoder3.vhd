
library ieee; 
use ieee.std_logic_1164.all;

entity encoder3 is 

generic (
	constant L0 :  std_logic_vector(127 downto 0):= x"7546f665fbf07eec75902581e7ebd072";
	constant L32 : std_logic_vector(127 downto 0):= x"c196638a4bfe987179a486f304419ca2";
	constant L64 : std_logic_vector(127 downto 0):= x"a1ecd4631cd81a9d8f0db2506ddad1d2";
	constant L96 : std_logic_vector(127 downto 0):= x"f5ea237904bd6e24b41212b8e73d62c5"
);

port (
	input :  in std_logic_vector(127 downto 0);
	output : out std_logic_vector(255 downto 0);
	clk, valid_in : in std_logic :='0';
	valid_out, busy : out std_logic :='0'
);

end encoder3; 

architecture behavioral of encoder3 is
	 function sum_xor (
		 in_vec    : in std_logic_vector)
		 return std_logic is
		 variable s : std_logic := '0';
		 begin
			 for i in in_vec'low to in_vec'high loop 
					s := s xor in_vec(i);
			 end loop;
		 return s;  
	  end function sum_xor;
	  
 	function shift_circ_r(
		L: in std_logic_vector; index : in integer)
		return std_logic_vector is
		variable size: integer := L'Length;
		variable res : std_logic_vector(size-1 downto 0) := (others => '0');
		begin
			res := L(size-1-index downto 0) & L(size-1 downto size-index);
		return res;
	end function shift_circ_r;
	
	
	function circular_perm4(L : in std_logic_vector; index : in integer := 0)
	return  std_logic_vector is	
		variable  res : 	std_logic_vector(127 downto 0) := (others => '0');
		variable  mot :  	std_logic_vector(31  downto 0) := (others => '0');
		variable  mot_per :  	std_logic_vector(31  downto 0) := (others => '0');
		begin 
			decomp : for i in 0 to 3 loop
							mot := L((i+1)*32 -1 downto i*32); 
							mot_per := shift_circ_r(mot, index); 
							res((i+1)*32 - 1 downto i*32) := mot_per;
			end loop decomp;
			return res;
	end function;
	
	signal state :     integer range 0 to 50 := 0;
	signal index : 	 integer range 0 to 50 := 0;
	signal s_droite :  std_logic_vector(127 downto 0) := (others => '0');
	signal count : integer range 0 to 1 :=0;
	
begin

	main : process(clk)
	variable V_0_31 : 	std_logic_vector(127 downto 0) := L0;
	variable V_32_63 :   std_logic_vector(127 downto 0) := L32;
	variable V_64_95 : 	std_logic_vector(127 downto 0) := L64;
	variable V_96_127 :  std_logic_vector(127 downto 0) := L96;
	
	begin
	count <= (count +1) mod 2; 
	
	if (rising_edge(clk)) then
		if (count =0) then 
		if (valid_in = '1' and state = 0) then
			busy <= '1'; state <= 1; index <= 0; 
		end if;
		
		if (0<state and state<33) then	
			V_0_31 := circular_perm4(V_0_31,  1); V_32_63  := circular_perm4(V_32_63, 1); 
			V_64_95:= circular_perm4(V_64_95, 1); V_96_127 := circular_perm4(V_96_127,1); 
			state <= state + 1; index <= index + 1;
		end if;
		
		if (0<=state and index<31) then 
			s_droite(index)	 	 <=  sum_xor(input and V_0_31);
			s_droite(index + 32)  <=  sum_xor(input and V_32_63);
			s_droite(index + 64)  <=  sum_xor(input and V_64_95); 
			s_droite(index + 96)  <=  sum_xor(input and V_96_127);
		end if;
		
		if (state = 33) then 
			output <= s_droite & input;
			valid_out <= '1'; state <= state + 1;
		end if;
		
		if (state = 34) then 
			state <= 0; busy <='0'; valid_out <= '0';
		end if;
	end if;
				
	end if;
	end process main;
	
	
end behavioral;
