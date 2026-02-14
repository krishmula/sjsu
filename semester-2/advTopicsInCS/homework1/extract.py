import os
import tarfile


# Step 1: Extract the tar.gz file
def extract_targz(tar_path, extract_to="data/WISDM"):
    """
    Extract .tar.gz file
    """
    print(f"Extracting {tar_path}...")

    # Create directory if it doesn't exist
    os.makedirs(extract_to, exist_ok=True)

    # Open and extract
    with tarfile.open(tar_path, "r:gz") as tar:
        tar.extractall(path=extract_to)
        print(f"Extracted to: {extract_to}")

    # List extracted files
    print("\nExtracted files:")
    for root, dirs, files in os.walk(extract_to):
        for file in files:
            print(os.path.join(root, file))


# Extract your WISDM file
extract_targz("WISDM_ar_latest.tar.gz", "data/WISDM")
