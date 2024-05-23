----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 14.02.2024 13:02:49
-- Design Name: 
-- Module Name: bistable - Structural
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

entity bistable is
  Port (Q: out std_logic;
  C:in std_logic );
end bistable;

architecture structural of bistable is

component X_OR is
  Port ( A,B: in std_logic;
  F: out std_logic);
end component;

signal inv0_s: std_logic;
signal inv1_s: std_logic;
attribute DONT_TOUCH : string;
attribute DONT_TOUCH of structural : architecture is "true";
attribute ALLOW_COMBINATORIAL_LOOPS : string;
attribute ALLOW_COMBINATORIAL_LOOPS of structural : architecture is "true";
begin
U1: X_OR port map(A=>inv1_s,B=>C,F=>inv0_s);
inv1_s<=inv0_s;
Q<=inv1_s;
end structural;
