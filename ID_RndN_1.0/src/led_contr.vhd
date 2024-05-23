----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 15.04.2024 22:48:23
-- Design Name: 
-- Module Name: led_contr - Structural
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

entity led_contr is
  Port (Contr:in std_logic;
  Gen_rand: in std_logic;
  Led: out std_logic;
  Outp: out std_logic_vector(15 downto 0)
   );
end led_contr;

architecture behavioral of led_contr is
component Counter is
  Port (
  Cl:in std_logic;
  Allow: in std_logic;
  F:out std_logic;
  Num:out std_logic_vector(15 downto 0));
end component;

component bistable is
  Port (Q: out std_logic;
  C:in std_logic );
end component;
type   numbers is array(0 to 15) of std_logic_vector(15 downto 0);
signal A: std_logic;
--signal Outp: numbers;
attribute dont_touch:string;
attribute dont_touch of behavioral: architecture is "true";
begin
    U1: bistable port map(C=>Contr, Q=>A);
    U2: Counter port map(Cl=>A,Allow=>Gen_rand, F=>Led, Num=>Outp);
    Led<=A;
end behavioral;
