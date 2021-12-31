import numpy as np

def distance2(v1: np.ndarray, v2: np.ndarray) -> np.ndarray:
    return np.sum((v2 - v1)**2, axis=-1)

def distance(v1: np.ndarray, v2: np.ndarray) -> np.ndarray:
    return np.sqrt(distance2(v1, v2))