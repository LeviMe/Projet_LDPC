
library ieee;
use ieee.std_logic_1164.all;
use std.textio.all;

entity test_clk_divider is
port (
	clk : in std_logic :='0';
	clk2_out : out std_logic :='0'
);
end test_clk_divider;

architecture behavioral of test_clk_divider is

signal count_clk, count_clk_0, count_clk_2 : integer range 0 to 32;
signal clk2: std_logic:='0';
signal latch_clk2 : std_logic_vector(2 downto 0) := b"000";

begin
	clk_divider : process(clk)
	begin
	if (rising_edge(clk)) then
		count_clk_0 <= count_clk_0 + 1;
		if(count_clk_0 = 31) then
			count_clk_0 <= 0;
			clk2 <= not clk2;
			latch_clk2 <= clk2 & latch_clk2(2 downto 1);
		end if;
		clk2_out <= clk2;
	end if;
	end process clk_divider;
end behavioral;



--library ieee; 
--use ieee.std_logic_1164.all;
--use std.textio.all;
--
--entity test_clk_divider is 
--port (
--	clk : in std_logic :='0';
--	clk2_out : out std_logic :='0'
--);
--end test_clk_divider; 
--
--architecture behavioral of test_clk_divider is
--	
--	signal count_clk, count_clk_0, count_clk_2 : integer range 0 to 4;
--	signal clk2: std_logic:='0';
--	signal latch_clk2 : std_logic_vector(1 downto 0) := b"00";
--
--begin
--	clk_divider : process(clk)
--	begin
--	if (rising_edge(clk)) then
--		count_clk_0 <= count_clk_0 + 1;
--		count_clk_2 <= count_clk_0 mod 4;
--		count_clk <= count_clk_2;
--		clk2 <= not clk2;
--	end if;
--		clk2_out <= clk2;
--
--	end process clk_divider;
--	
--end behavioral;
