from typing import Callable, Iterable, List, Tuple, TypeVar, Union

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

def assignInline(arr, indices, values):
    arr[indices] = values
    return arr

def first(arr: Iterable[Nested], predicate: Callable[[Nested], bool]) -> Nested:
    for value in arr:
        if predicate(value):
            return value

    raise Exception("Could not find first value within array")

def combineMeanStdAndCount(mean1: Nested = 0, std1: Nested = 1, count1: int = 1, mean2: Nested = 0, std2: Nested = 1, count2: int = 1) -> Tuple[Nested, Nested, int]:
    return (
        (mean1 * count1 + mean2 * count2) / (count1 + count2),
        ((count1 - 1) * std1 ** 2 + (count2 - 1) * std2 ** 2) / (count1 + count2 - 1) + (count1 * count2 * (mean1 - mean2) ** 2) / ((count1 + count2) * (count1 + count2 - 1)),
        count1 + count2
    )
