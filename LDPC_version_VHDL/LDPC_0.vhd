
library ieee; 
use ieee.std_logic_1164.all;
use IEEE.std_logic_textio.all;
use std.textio.all;
 
entity LDPC_0 is 
 
generic (
	constant K : integer := 128;
	constant N : integer := 256
);

port (
	clk : in std_logic;
	res : out std_logic
);
 end LDPC_0;
 
 architecture behavioral of LDPC_0 is 
		function egalite ( 
			in_vec_1, in_vec_2  : in std_logic_vector) return std_logic is
		 variable s : std_logic := '0';
		 begin
			 for i in in_vec_1'low to in_vec_1'high loop 
					s := s or (in_vec_1(i) xor in_vec_2(i));
			 end loop;
		 return not s;  
	  end function egalite;
 
		signal inp_sig :std_logic_vector(127 downto 0) := x"f5ea237904bd6e24b41212b8e73d62c5"; 
		signal outp_sig : std_logic_vector(255 downto 0);
		signal exp_output : std_logic_vector(255 downto 0) := x"6afc7ead19b7f8b1bc34686193755626f5ea237904bd6e24b41212b8e73d62c5";

		component encoder
		port (
		input :  in std_logic_vector(127 downto 0);
		output : out std_logic_vector(255 downto 0)--;
		--clk, valid_in : in std_logic :='0';
		--valid_out, busy : out std_logic :='0'
	);
		end component;
			
begin
	connection: encoder port map(input => inp_sig, output => outp_sig); -- , clk=> clk, valid_in => '1');
	-- le port map: toujours les identifiants du composants suivis des indentifiants signaux du module.
	res <= egalite(outp_sig, exp_output); 
end behavioral;
