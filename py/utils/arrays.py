from typing import TypeVar

import numpy as np

Nested = TypeVar("Nested")

def assign(arr: np.ndarray, indices, values) -> np.ndarray:
    arr = np.copy(arr)
    arr[indices] = values
