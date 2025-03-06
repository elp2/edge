from PIL import Image
import numpy as np
import sys

def compare_images(file1):
    # Load images
    img1 = Image.open(file1).convert("RGB")
    img2 = Image.open("reference-dmg.png").convert("RGB")
    
    # Convert to numpy arrays for easier comparison
    arr1 = np.array(img1)
    arr2 = np.array(img2)
    
    # Check dimensions match
    if arr1.shape != arr2.shape:
        print(f"Image dimensions don't match: {arr1.shape} vs {arr2.shape}")
        return
    
    # Compare pixels
    differences = np.where(arr1 != arr2)
    if len(differences[0]) == 0:
        print("Images are identical")
        return
    
    # Print first few differences
    print(f"Found {len(differences[0])} different pixels")
    for i in range(min(10, len(differences[0]))):
        x, y = differences[1][i], differences[0][i]
        print(f"Difference at ({x}, {y}):")
        print(f"  File1: {arr1[y,x]}")
        print(f"  File2: {arr2[y,x]}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python compare.py file1.bmp")
        sys.exit(1)
    
    compare_images(sys.argv[1])