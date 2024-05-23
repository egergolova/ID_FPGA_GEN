----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 21.05.2024 11:33:01
-- Design Name: 
-- Module Name: count_clock - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity count_clock is
  Port ( 
  init_st: in std_logic;
  quartz: in std_logic;
  allow: out std_logic
  );
end count_clock;

architecture Behavioral of count_clock is

signal count: std_logic_vector(15 downto 0):="0000000000000000";
signal go: std_logic;
begin
    process(init_st,quartz,go, count) is
    begin
        if rising_edge(init_st) then
            go<='1';
            count<="0000000000000000";
            allow<='1';
        end if;
        if rising_edge(quartz) then
            if (go='1') then
                count<=count+'1';
            end if;
        end if;
        if(count="0000000001100100") then
            count<="0000000000000000";
            allow<='0';
            go<='0';
        end if;
    end process;

end Behavioral;
