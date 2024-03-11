# Use the official Python image as the base image
FROM python:3.9-slim

# Install system dependencies
RUN apt-get update && apt-get install -y \
    git \
    curl \
    ca-certificates \
    --no-install-recommends && \
    rm -rf /var/lib/apt/lists/*

# Install avrdude
RUN apt-get update && apt-get install -y \
    avrdude \
    --no-install-recommends && \
    rm -rf /var/lib/apt/lists/*


# Install PlatformIO
RUN pip install -U platformio

# Install additional tools you might need (optional)

# Set the working directory in the container
WORKDIR /workspace

# Keep the container running
CMD ["sleep", "infinity"]
