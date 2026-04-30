# Lambda
A from scratch engine-like toy environment developed in C++26. Primarily being worked on for the purposes of learning how to approach architectural problems for larger-scale projects.

## How to Use
The build system for this uses a Docker file that you can access via Make.
```
# Build the docker image; one off
> make docker-image

# Configure the project
> make config

# Build the runtime
> make build

# Build and run the unit tests
> make tests
```

## License
> I dedicate any and all copyright interest in this software to the
public domain. I make this dedication for the benefit of the public at
large and to the detriment of my heirs and successors. I intend this
dedication to be an overt act of relinquishment in perpetuity of all
present and future rights to this software under copyright law.

For more, see: [Unlicense](https://unlicense.org/).