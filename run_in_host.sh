#!/bin/bash
docker build -t compilers_image .
docker run --rm --entrypoint ./test.sh -v "${PWD}:/code" -w "/code" --name "compilers_env" compilers_image

