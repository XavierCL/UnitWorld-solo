from typing import Any, Callable, Iterable, List, Optional, Tuple, TypeVar, Union

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

def combineMeanStdAndCount(mean1: Nested = 0, std1: Nested = 1, count1: int = 1, mean2: Nested = 0, std2: Nested = 1, count2: int = 1, minStd: Optional = None) -> Tuple[Nested, Nested, int]:
    if count1 < 1 and count2 < 1:
        return (mean1 + mean2) / 2, 1 if minStd is None else minStd, 0
    elif count1 < 1:
        return mean2, std2, count2
    elif count2 < 1:
        return mean1, std1, count1

    intermediateStd = np.sqrt(
        ((count1 - 1) * std1 ** 2 + (count2 - 1) * std2 ** 2) / (count1 + count2 - 1) + (count1 * count2 * (mean1 - mean2) ** 2) / ((count1 + count2) * (count1 + count2 - 1))
    )

    return (
        (mean1 * count1 + mean2 * count2) / (count1 + count2),
        np.max([intermediateStd, np.zeros_like(intermediateStd) if minStd is None else np.ones_like(intermediateStd) * minStd], axis=0),
        count1 + count2
    )

def sort(arr: List[Nested], key: Optional[Callable[[Nested], Any]] = None) -> List[Nested]:
    copy = arr[:]
    copy.sort(key)
    return copy

def firstIndex(arr: Iterable[Nested], value: Nested) -> int:
    for index, current in enumerate(arr):
        if current == value:
            return index

    return -1

def mad(arr: Iterable, median: Optional[float] = None, axis: Optional[int] = None, returnMedian=False, minMad: Optional = None) -> Union[float, Tuple[float, float]]:
    if median is None:
        median = np.median(arr, axis)
    ad = np.median(np.abs(arr - median), axis)
    ad = ad if minMad is None else np.max([np.ones_like(ad) * minMad, ad], axis=0)

    if returnMedian:
        return ad, median

    return ad
