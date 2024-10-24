# Use an official base image, e.g., Ubuntu
FROM ubuntu:latest

# Set the working directory in the container
WORKDIR /usr/src/app

# Copy the C++ source file into the container
COPY . .
RUN rm -f CMakeCache.txt

# Install necessary packages (e.g., GCC)
RUN apt-get update && apt-get install -y g++ && apt-get install -y cmake

# Compile the C++ program
RUN cmake . && make
# RUN ls -a
# RUN cd ./src && ls -a
# RUN cd ./bin && ls -a


# Run the compiled program when the container starts
CMD ["./bin/app", "input.txt"]
# ENTRYPOINT ["cp /usr/src/files/input.txt . && echo \"the file is input.txt\" && ls"]
