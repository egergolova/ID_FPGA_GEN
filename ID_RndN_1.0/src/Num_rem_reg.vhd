----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 22.05.2024 10:59:41
-- Design Name: 
-- Module Name: Num_rem_reg - Behavioral
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity Num_rem_reg is
  Port (Allow: in std_logic;
  Num: in std_logic_vector(15 downto 0);
  Res:out std_logic_vector(15 downto 0) );
end Num_rem_reg;

architecture Behavioral of Num_rem_reg is
begin
    process(Allow, Num) is
    begin
        if falling_edge(Allow) then
            Res<=Num;
        end if;
    end process;
end Behavioral;
