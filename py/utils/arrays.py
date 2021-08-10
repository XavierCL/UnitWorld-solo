from typing import Callable, Iterable, List, TypeVar, Union

import numpy as np

Nested = TypeVar("Nested")

def soft_accessor(
    arr: List[Nested],
    start: Union[int, None] = None,
    end: Union[int, None] = None,
    space: Union[int, None] = None
) -> List[Nested]:
    if start is None:
        pass
    elif start >= len(arr) or -start >= len(arr) + 1 or end == 0:
        return []

    if end is None:
        pass
    elif end > len(arr):
        end = len(arr)
    elif -end > len(arr) + 1:
        end = -len(arr) - 1

    return arr[slice(start, end, space)]

def assign(arr: np.ndarray, indices, values) -> np.ndarray:
    arr = np.copy(arr)
    arr[indices] = values
    return arr

def first(arr: Iterable[Nested], predicate: Callable[[Nested], bool]) -> Nested:
    for value in arr:
        if predicate(value):
            return value

    raise Exception("Could not find first value within array")
