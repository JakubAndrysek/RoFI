from __future__ import annotations

import math
from typing import Union, overload


class Angle:
    pi: float = math.pi

    def __init__(self, rads: float) -> None:
        self._rads: float = rads

    @classmethod
    def from_rad(cls, rads: float) -> Angle:
        return cls(rads)

    @classmethod
    def from_deg(cls, degs: float) -> Angle:
        return cls(cls.pi * degs / 180)

    def __iadd__(self, other: Angle) -> Angle:
        self._rads += other._rads
        return self

    def __isub__(self, other: Angle) -> Angle:
        self._rads -= other._rads
        return self

    def __imul__(self, value: float) -> Angle:
        self._rads *= value
        return self

    def __itruediv__(self, value: float) -> Angle:
        self._rads /= value
        return self

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, Angle):
            return NotImplemented
        return self._rads == other._rads

    def __ne__(self, other: object) -> bool:
        return not (self == other)

    def to_deg(self) -> float:
        return 180 * self._rads / self.pi

    def to_rad(self) -> float:
        return self._rads

    def __neg__(self) -> Angle:
        return Angle(-self._rads)

    def __add__(self, other: Angle) -> Angle:
        return Angle(self._rads + other._rads)

    def __sub__(self, other: Angle) -> Angle:
        return Angle(self._rads - other._rads)

    @overload
    def __mul__(self, value: float) -> Angle: ...

    @overload
    def __mul__(self, value: int) -> Angle: ...

    def __mul__(self, value: Union[float, int]) -> Angle:
        return Angle(self._rads * value)

    @overload
    def __rmul__(self, value: float) -> Angle: ...

    @overload
    def __rmul__(self, value: int) -> Angle: ...

    def __rmul__(self, value: Union[float, int]) -> Angle:
        return self * value

    @overload
    def __truediv__(self, value: float) -> Angle: ...

    @overload
    def __truediv__(self, value: int) -> Angle: ...

    def __truediv__(self, value: Union[float, int]) -> Angle:
        return Angle(self._rads / value)

    def __str__(self) -> str:
        return f"{self._rads:.6f}"

    def __repr__(self) -> str:
        return f"Angle({self._rads:.6f})"


def clamp(value: Angle, min_val: Angle, max_val: Angle) -> Angle:
    return Angle(max(min_val.to_rad(), min(value.to_rad(), max_val.to_rad())))


# Custom literal-like functions for degrees and radians
def deg(value: float) -> Angle:
    return Angle.from_deg(value)


def rad(value: float) -> Angle:
    return Angle.from_rad(value)


if __name__ == "__main__":
    # Example usage with type hints
    angle1: Angle = Angle.from_deg(45)
    print(f"Angle in degrees: {angle1.to_deg()}")
    angle2: Angle = deg(45)  # Using the custom deg() function
    print(f"Angle in radians: {deg(45)}")
    angle3: Angle = rad(math.pi / 4)

    result: Angle = angle1 + angle2
    result *= 2

    print(f"Result in degrees: {result.to_deg()}")
    print(f"Result in radians: {result.to_rad()}")
