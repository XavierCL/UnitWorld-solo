from typing import List, TypeVar, Union

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
