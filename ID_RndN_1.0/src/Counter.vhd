----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 14.04.2024 18:13:06
-- Design Name: 
-- Module Name: Counter - Behavioral
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
use IEEE.NUMERIC_STD.ALL;
-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity Counter is
  Port (
  Cl:in std_logic;
  Allow: in std_logic;
  F:out std_logic;
  Num:out std_logic_vector(15 downto 0));
end Counter;

architecture Behavioral of Counter is
signal Count: std_logic_vector(15 downto 0);
signal Fi: std_logic:='0';
attribute dont_touch:string;
attribute dont_touch of behavioral: architecture is "true";
begin
process(Cl,Count,Fi, Allow) is
begin
    if(Allow='1') then
        if rising_edge(Cl) then
            Count<=Count+'1';
--            if(Count="111100011110000") then
--                Count<="0000000000000000";
--                Fi<=not Fi;
--            end if;
        end if;
        F<=Fi;
    end if;
    if falling_edge(Allow) then
        Num<=Count;
    end if; 
    if(Allow='0') then
        Count<="0000000000000000";
    end if;
end process;
end Behavioral;
