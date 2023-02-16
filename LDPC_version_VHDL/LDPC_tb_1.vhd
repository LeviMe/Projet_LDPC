
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_textio.all;
use std.textio.all;

entity LDPC_tb_1 is 
end LDPC_tb_1; 


architecture tb of LDPC_tb_1 is 

signal input :std_logic_vector(127 downto 0); 
signal output : std_logic_vector(255 downto 0); 
signal clk,clk2, valid_in : std_logic := '0';
signal valid_out, busy : std_logic := '0';
signal compteur : integer range 0 to 2048 :=0;
signal exp_output  : std_logic_vector(255 downto 0) := x"6afc7ead19b7f8b1bc34686193755626f5ea237904bd6e24b41212b8e73d62c5";

begin 
	testb: entity work.encoder2 port map (
	input=> input, output=> output, clk => clk, clk2_out => clk2, busy => busy,
	valid_in=>valid_in, valid_out => valid_out);  -- tous les ports n'ont pas à etre connectés pour que le module compile. l'ordre n'importe pas si déclaration explicite.

clock_process: process 

constant period : time := 100 ps;
begin 
	clk <=  not clk ; 
	wait for period/2;
	compteur <= compteur + 1;  
	if compteur = 1000 then assert false severity failure; end if; 	
end process;
	
	testb_proc : process
	begin
	report "demarrage";
	while (1=1) loop
		wait for 10 ps;
		if compteur = 10 then 
			input <= x"f5ea237904bd6e24b41212b8e73d62c5";
			valid_in <= '1';
		end if;
		if compteur = 32 then 
			valid_in <= '0';
		end if;
		
		if valid_out = '1' then 
		--	report to_hstring(output);
			wait for 10 ps;
		--	report to_hstring(exp_output xor output);
			wait for 4000 ps;
			assert false report "simulation completed" severity failure;
		end if;
	end loop;
	
	end process; 
	
end tb;
		