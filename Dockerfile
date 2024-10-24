# Use an official base image, e.g., Ubuntu
FROM ubuntu:latest

# Set the working directory in the container
WORKDIR /app

# Copy the C++ source file into the container
COPY . /app
RUN rm -f /app/CMakeCache.txt

# Install necessary packages (e.g., GCC)
RUN apt-get update && apt-get install -y g++ && apt-get install -y cmake

# Compile the C++ program
RUN cmake . && make

# Run the compiled program when the container starts
# CMD ["./bin/app"]
ENTRYPOINT ["./bin/app"]
