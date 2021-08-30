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

def assignShape(shape: Tuple, indices, values) -> Tuple:
    arr = np.array(shape, dtype=object)
    arr[indices] = values
    return tuple(arr)

def assignInline(arr, indices, values):
    arr[indices] = values
    return arr

def first(arr: Iterable[Nested], predicate: Callable[[Nested], bool]) -> Nested:
    for value in arr:
        if predicate(value):
            return value

    raise Exception("Could not find first value within array")

def combineMeanStdAndCount(means: np.ndarray, stds: np.ndarray, counts: np.ndarray, minStd: Optional[float] = None) -> Tuple[float, float, int]:
    count = np.sum(counts, axis=0)
    minStd = 1 if minStd is None else minStd
    meanConversionShape = assignShape(assignShape(means.shape, None, np.newaxis), 0, slice(None))
    if count < 1:
        return np.mean(means, axis=0), minStd, 0

    mean = np.sum((counts[meanConversionShape] * means), axis=0) / count
    variances = stds**2
    means2 = means**2
    std = np.sqrt(np.sum(counts[meanConversionShape] * (means2 + variances[meanConversionShape]), axis=0) / count - mean ** 2)

    return (
        mean,
        np.maximum(std, minStd),
        count
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
