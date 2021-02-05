from enum import Enum
from typing import List, Final


class Color(Enum):
	RED = 31
	GREEN = 32
	YELLOW = 33


def log(message: str, second_message: str = "", color: Color = Color.GREEN) -> None:
	sign: str = "+"
	if color == Color.YELLOW:
		sign = "?"
	elif color == Color.RED:
		sign = "-"
	print(f"\033[{str(color.value)}m[{sign}] {message}\033[0m", end='')
	if second_message:
		print(f" {second_message}", end='')
	if message[-1] != '\n':
		print()


def print_success() -> None:
	print(f"\033[{str(Color.GREEN.value)}m")
	print(
"""
╔══╗ ╔╗╔╗ ╔══╗ ╔══╗ ╔═══╗ ╔══╗ ╔══╗
║╔═╝ ║║║║ ║╔═╝ ║╔═╝ ║╔══╝ ║╔═╝ ║╔═╝
║╚═╗ ║║║║ ║║   ║║   ║╚══╗ ║╚═╗ ║╚═╗
╚═╗║ ║║║║ ║║   ║║   ║╔══╝ ╚═╗║ ╚═╗║
╔═╝║ ║╚╝║ ║╚═╗ ║╚═╗ ║╚══╗ ╔═╝║ ╔═╝║
╚══╝ ╚══╝ ╚══╝ ╚══╝ ╚═══╝ ╚══╝ ╚══╝
""")
	print("\033[0m")
